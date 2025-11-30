# Projekt struktúra

## Segédfájlok
- [CMakeLists.txt](./CMakeLists.txt)
  - A projekt leírása a CMake build-system számára
  - A `debugmalloc.h` ezen a fileon belül force-`#include`-olva van minden forráskód fileba automatikusan
- [fonts/](./fonts/)
  - A program által használt betűtípusok
## Forráskód felbontása:
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

