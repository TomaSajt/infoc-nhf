# Programozói dokumentáció

## Elkészítés és futtatás

A program elkészítéséhez telepítsd a következő programokat/csomagokat:

- CMake
- SDL3
- SDL3_ttf

Ha telepítve van a `nix` csomagkezelő, akkor könnyen beléphetsz a `nix-shell` paranccsal egy olyan fejlesztői könyezetbe,
amelyben az buildeléshez szükséges programok és könyvtárak elérhetőek.

A program CMake-et használ build-system-ként.

Könnyen futtatható a program a `./br.sh` helper script által, ami re-buildeli és futtatja a kódot.

## Projekt struktúra

### Segédfájlok
- [CMakeLists.txt](./CMakeLists.txt)
  - A projekt leírása a CMake build-system számára
  - A `debugmalloc.h` ezen a fileon belül force-`#include`-olva van minden forráskód fileba automatikusan
- [fonts/](./fonts/)
  - A program által használt betűtípusok

### Forráskód felbontása:
- [main.c](./main.c)
  - Meghajtó kód
- [state.h](./state.h)
  - Az applikáció állapotár tároló adatstruktúra definíciója
- [draw.c](./draw.c)
  - Grafikus segédfüggvények
  - Használja a harmadik féltől származó `SDL_gfx` könyvtárat
- [hover.c](./hover.c)
  - Kód, ami eldönti, hogy mi van jelenleg a kurzor alatt, vagy mi jöjjön létre, ha pontot akarunk létrehozni a kurzor alatt
- [view.c](./view.c)
  - A sík látható részét módósító függvények és koordináta-rendszer átváltások logikája
- [save/data.c](./save/data.c)
  - A `.geom` fileformátumot feldolgozó és író logika
- [save/dialog.c](./save/dialog.c)
  - A mentés és betöltés logikát meghívó függvények és párbeszédablakok logikája
- [mode/defs.c](./mode/defs.c)
  - Nagy statikus vtable a módokhoz (így nem kell mindenhol switch-et használni)
- [mode/util.c](./mode/util.c)
  - A kiválasztott módot váltó segédfüggvények
- [mode/*.c](./mode/)
  - A módok viselkedéseinek leírása
- [geom/defs.h](./geom/defs.h)
  - A geometriai elemek definíciói
- [geom/defs.c](./geom/defs.c)
  - A geometriai elemek létrehozását elősegítő függvények definíciói
- [geom/state.c](./geom/state.c)
  - A vászon elemeit kezelő függvények
- [geom/util.c](./geom/util.c)
  - Függvények logika elemek kiértékeléséhez és egyéb segédfüggvények

## Adatstruktúrák

Az elemek különböző al-típusai struct-okba ágyazott union-okként vannak megoldva.
Gondolkodtam, hogy megpróbáljam-e a három különbőző elemtípust egy struktúrában kezelni, de nem jutott eszembe szép megoldás.
Sajnos ennek következménye, hogy sok kódrészlet háromszor egymás után kisebb változtatással szerepel, ami "sormintának" számíthat egyesek számára.

Maguk az geometriai elemek (PointDef/LineDef/CircleDef) az esetek nagy részében dinamikusan foglalt területen vannak.
Az elemek közti kapcsolatok irányított körmentes gráfot alkotnak.
Az elemekre mutató pointereket láncolt listákban tároljuk, ezek a `GeometryState`-ben találhatóak.
A láncolt listák használata a beillesztés és törlés gyorsasága miatt előnyös, bár mivel nincs olyan gyakran új elem létrehozva,
nem annyira lett volna ez fontos, lehetett volna dinamikusan méretezett tömb is.

A gráf cache-elést alkalmaz, nem számolja ki minden pillanatban újra az elemek pozícióit.
Sajnos nem csináltam logikát, hogy mozgatáskor tényleg csak az számolódjon újra, ami megmozdult, vagy attól függ,
ilyenkor egszerűen minden újraszámolódik.

Elemek törlésénél vigyázni kell, hogy kaszkádosítva töröljünk, hiszen különben egyes elemek nem létező elemektől függenének.
Törléskor és hozzáadáskor is figyelni kell, hogy mind a gráf csúcsat és mind a láncolt lista elemét le kell foglalni vagy fel kell szabadítani.

Betöltéskor, mivel nem tudjuk, hogy hány sornyi adatunk lesz, láncolt listákba olvassuk be először a sorokat, elemtípustól függően.
A sorok max 256 karakter hosszúként vannak beolvasva, hiszen normális működés mellett sose lesz ilyen hosszú egy sor.
Mikor már tudjuk, hogy melyik típusú elemből mennyi lesz, lefoglaljuk a később a `GeometryState`-be bekerülő láncolt listákat.
De mivel mentett adatok azonosítokat használnak, és az azonosítókat gyorsan akarjuk majd keresni,
létrehozunk mind a beolvasott adatok láncolt listáihoz és mind az elemeknek legfoglalt láncolt listákhoz egy-egy segédtömböt,
amiket qsort-tal és bsearch segítségével gyorsan alkalmazhatunk.
Sajnos a sok ideiglenesen lefoglalt memóriát fel is kell szabadítani,
így több helyen is a `goto` cleanup patternt használtam a letisztultabb a logikához.

A módok egy nagy állapotgép által vannak implementálva. Minden módhoz tartozik egy function pointer "vtable",
amik kezelik az abban a módban történő eseményeket.
Sajnos a módok különböző adatokat tárolnak, így nem tudtam ténylegesen az adatstruktúrát elválasztani a konkrét módoktól.
