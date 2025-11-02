# InfoC NHF Specifikáció

- *Megjegyzés: ezen dokumentum írásakor a program jelentős része már működőképes.
  Saját véleményem szerint el fogom tudni készíteni a specifikáció szerint, még akkor is, ha ennyire összetett.*
- *(A program lényegében a https://www.desmos.com/geometry oldal butább verziója, de ugye ez nem elég specifikációnak.)*
- Rövid összefoglaló
  - A program egy grafikus felületű síkgeometriai szerkesztőprogram.
  - Támogatott geometriai elemek: pontok, egyenesek/félegyenesek/szakaszok, körök.
  - Az elemek egymással függőségi viszonyban állnak.
  - Létrehozás után is változtathatóak az elemek pozíciói.
    - pl: Ha elmozdul egy pont, akkor a tőle függő elemek (pl: rajta kereszülmenő egyenesek) is vele változnak.
  - A program képes az elemek definícióinak fájlba mentésére és fájlból betöltésére.

# Hosszú programleírás
- Megjegyzés:
  - A program neve még később lesz eldöntve, nem része a specifikációnak!
  - A programban az egérrel való kattintás csak a bal egérgombnál csinál bármit is,
    tehát a specifikáció szövegezésében az "egérgomb" = "bal egérgomb", "kattintás" = "bal klikkelés".
- A megjelenített szövegek angol nyelvűek.
- A program grafikus megjelnítést használ.
- Indítás után megjelenik egy grafikus felület, amit nevezzünk vászonnak.
- A végtelen sík egy felhasználó által állítható része van megjelenítve meg a vásznon, nevezzül ezt a fókuszált területnek.
- A végtelen sík tartalmazza majd a geometriai elemeket.
- A végtelen sík kezdő állapotában nem tartalmaz egy elemet sem.
  - Kivéve, ha van megadva argumentum, bővebben kifejtve a "Mentés/Betöltés" szekcióban.
- A fókuszált terület méretén változtatni fel/le görgetéssel lehet, nevezzük ezt "zoomolásnak":
  - görgetés fel -> kisebb terület látszódik
  - görgetés le -> nagyobb terület látszódik
  - A kurzor zoomolás közbeni pozíciója fixpontnak számít a síkon, azaz a kurzor közelében
    lévő objektumok a kurzor közelében is maradnak a zoomolás után.
- Az elemek pozícióját egér segítségével lehet beállítani, nincs lehetőség szövegdoboz/konzolablak segítségével pozíciókat bevinni.

## Geometriai elemek
- A program pontokkal, egyenes-szerű elemekkel és körökkel képes dolgozni.
  - Az egyenes-szerű elemek alatt értsük az egyenest, a félegyenest és a szakaszt .
- Az elemek egymással függőségi viszonyban vannak.
- A függőségi viszonyok egy irányított körmentes gráfot alkotnak (DAG).
- Az egyetlen elemtípus ami nem függ semmitől a pont "literál".
- Ha frissül egy elem elhelyezkedse, a többi, tőle függő elemnek is kell, hogy frissüljön.
- "Érvénytelen" állapotú elemek is jöhetnek létre.
  - Ez tipikusan akkor történik meg, ha egy metszéspont nem létezik.
  - De akkor is, ha egy elem függ egy olyan másik elemtől, ami "érvénytelen" állapotban van.
  - Az érvénytelen állapotban lévő elemek nincsenek megjelenítve.

### Elemtípusok
- Pont
  - "Literál"
    - Nem függ más elemektől.
    - Szabadon mozgatható.
  - "Felezőpont"
    - Két másik ponttól függ.
    - Félúton található a másik két pont között.
      - Lényegében a két pont közötti szakasz felezőpontja,
        bár nem szükséges, hogy tényleg legyen köztük szakasz.
  - "Csúszka egyenesen"
    - Egy egyenes-szerű elemtől függ.
    - Csak az egyenes mentén csúsztatható el.
      - Ha csúszkát olyan helyre akarnánk húzni, ami nem az egyenesen-szerű elemen van,
        akkor az csúszka pozíciója legyen egyenes-szerű elem a kurzorhoz leközelebb eső pontja.
    - A csúszka állapota egy szabad `prog` paraméterben van tárolva.
      - A pont pozíciója megkapható a `(1-prog)*S + prog*E` képlet által,
        ahol az `S` és `E` az egyenes-szerű elem segédpontjai. (Erről később bővebben.)
        - `prog` `[0;1]` intervallumban, ha a csúszka szakasztól függ.
        - `prog` `[0;inf)` intervallumban, ha a csúszka félegyenestől függ.
        - `prog` `(-inf;inf)` intervallumban, ha a csúszka egyenestől függ.
  - "Egyenesek metszéspontja"
    - Két egyenes-szerű elemtől függ.
    - Az egyenes-szerű elemek mindig teljes egyenesként értelmezendőek metszéspontszámításnál.
    - A pont pozíciója mindig megegyezik a két egyenes metszéspontjával.
    - Ha a két egyenes párhuzamos, akkor "érvénytelen" állapotú a pont.
  - "Csúszka körön"
    - Egy körtől függ.
    - Csak a kör mentén csúsztatható el.
      - Ha a csúszkát olyan helyre akarnánk húzni, ami nem a körön van,
        akkor a csúszka pozíciója legyen kör a kurzorhoz legközelebb eső pontja.
    - A csúszka állapota egy szabad `prog` paraméterben van tárolva.
      - A pont pozíciója megkapható a `C + r*(cos(2pi*prog),sin(2pi*prog))` képlet által,
        ahol `C` a kör középpontja és `r` a sugara.
        - `prog` `[0;1)` intervallumban
  - "Egyenes és kör metszéspontja"
    - Egy egyenes-szerű elemtől és egy körtől függ.
    - Az egyenes-szerű elem mindig teljes egyenesként értelmezendő metszéspontszámításnál.
    - A pont pozíciója mindig megegyezik az egyenes és a kör egyik metszéspontjával.
    - Egy extra paraméter tárolja, hogy melyik metszéspontról beszélünk a kettő közül.
    - Ha nincs metszéspont, akkor "érvénytelen" állapotú a pont.
  - "Körök metszéspontja"
    - Két körtől függ.
    - A pont pozíciója mindig megegyezik a két kör egyik metszéspontjával.
    - Egy extra paraméter tárolja, hogy melyik metszéspontról beszélünk a kettő közül.
    - Ha nincs metszéspont, akkor "érvénytelen" állapotú a pont.
- Egyenes-szerű elem
  - "Pontból pontba"
    - Két ponttól függ.
      - Az első pont számít az `S` "start" segédpontnak.
      - a másik pont számít az `E` "end" segédpontnak.
    - Választható, hogy szakaszként, félegyenesként vagy egyenesként legyen értelmezve az elem.
      - Ha szakaszként értelmezzük, akkor az `S` és `E` pontot összekötő szakaszról beszélünk.
      - Ha félegyenesként értelmezzük, akkor az `S` pontból induló, az `E` ponton keresztülmenő félegyenesről beszélünk.
      - Ha egyenesként értelmezzük, akkor az `S` és `E` pontokon keresztülmenő egyenesről beszélünk.
  - "Párhuzamos"
    - Egy egyenes-szerű elemtől és egy ponttól függ.
    - Maga az elem csak egyenes típusú lehet.
    - Az egyenes keresztülmegy a ponton és párhuzamos az egyenes-szerű elemmel.
  - "Merőleges"
    - Egy egyenes-szerű elemtől és egy ponttól függ.
    - Maga az elem csak egyenes típusú lehet.
    - Az egyenes keresztülmegy a ponton és merőleges az egyenes-szerű elemre.
- Kör
  - "Pont körül, ponton keresztül"
    - Két ponttól függ: a középponttól és egy külső ponttól.
    - A kör középpontja a megadott középpont.
    - A kör sugara akkora, hogy a kör keresztülmenjen a külső ponton.
  - "Szakaszhossz sugárral, pont körül"
    - Egy szakasztól és egy ponttól függ.
    - A kör sugara pontosan a szakasz hosszával egyenlő.
    - A kör középpontja a pontnál található.

## Kategóriák és módok
- A programban különböző módok közül lehet választani.
- A módok kategóriákba vannak sorolva.
- A kategóriák és a bennük található módok:
  - "Manage" (1)
    - "Move" (M)
    - "Delete" (D)
  - "Points" (2)
    - "Point" (P)
    - "Midpoint"
  - "Lines" (3)
    - "Segment" (S)
    - "Line" (L)
    - "Ray" (R)
    - "Parallel"
    - "Perpendicular"
  - "Circles" (4)
    - "Circle" (C)
    - "Circle by length"
- Egy kategóriát kiválasztani a fenti listában a kategória neve mellett zárójelbe írt gomb megnyomásával lehet
  - Ekkor a kategórián belül legutóbb használt módba fog lépni a program.
  - A program indításakor mindig az összes kategóriának az első módja számít utolsónak használtnak.
- Fontosabb módoknak van külön gyorsgombja, szintén zárójelbe írva.
- A kiválasztott kategória módjai között a Tab segítségével lehet váltani.
  - Tab megnyomásával jelenlegi mód alatti módba lépünk át.
  - Ha nincs alatta mód, akkor visszatérünk a legfelső módhoz.
  - Ha a Shift gomb nyomása közben nyomjuk meg a Tab-ot, akkor ez másik irányba történik meg.
    - Azaz felfele lépünk a módok között, és a legfelső után a legalsóhoz ugrunk.
- Módok:
  - "Move" (M)
    - Pont mozgatása.
    - A szabadsági fokkal rendelkező pontokat lehet mozgatni.
      - Ezalatt érsük a "literál" vagy "csúszka" pontokat.
    - Az egérgomb lenyomásával a kurzor alatti pontot "megfogjuk", egér mozgatásával arrébb helyezzük,
      majd a mozgatás befejezéséhez elengedjük az egérgombot.
  - "Delete" (D)
    - Elemek kaszkádosított törlése.
    - A kurzor alatti elem és az összes tőle függő elem törlésre kerül kattintásra.
    - Ha a kurzort az egyik elem fölé visszük, akkor az maga az elem és a tőle függő elemek
      más színnel (esetleg piros színnel) lesznek rajzolva, hogy lássuk, hogy mi fog törlésre kerülni.
  - "Point" (P)
    - Pont létrehozása.
    - Kattintásra az alábbiak közül a fentről lefele sorrendben a legkorábban bekövetkező kapjon prioritást:
      - Ha egy már létező pontra kattintunk, nem jön létre semmi.
      - Ha két egyenes-szerű elemre kattintunk rá egyszerre, azaz mindkét elem a kurzor alatt van,
        akkor "egyenesek metszéspontja" típusú pont jön létre.
      - Ha egy egyenes-szerű elemre és egy körre kattintunk rá egyszerre,
        akkor "egyenes és kör metészpontja" típusú pont jön létre.
      - Ha két körre kattintunk rá egyszerre, akkor "körök metszéspontja" típusú pont jön létre.
      - Ha egy egyenes-szerű elemre kattintunk, "csúszka egyenesen" típusú pont jön létre.
      - Ha egy körre kattintunk, "csúszka körön" típusú pont jön létre.
      - Ha üres területre kattintunk "literál" típusú pont pont jön létre.
    - A kurzor mozgatása során kattintás nélkül is előnézetben látjuk, hogy hova fog létrejönni az új pont.
  - TOVÁBBIAKBAN FONTOS:
    - A további módokban, amikor arról van szó, hogy egy pontra kattintunk,
      akkor valójában kattinthatunk bárhova, és ha nem egy létező pontra kattintunk,
      akkor létrejön a fenti "Point" logika szerint egy új pont a kattintás következtében.
    - Sőt, az tobábbi módokban még kattintás előtt is látszódik előnézetben,
      hogy kattintásra hova jönne létre új elem, beleértve a kurzor alatti potenciális pontot és magát az új "fő" elemet is.
  - "Midpoint"
    - Felezőpont létrehozása.
    - Kattintással kiválasztunk egy pontot, ez meg lesz jegyezve.
    - Ha már van megjegyzett pont, akkor kattintásra a megjegyzett pont és a kurzor alatti potenciális pont
      között "Felezőpont" típusú pont jön létre.
  - "Segment" (S), "Line" (L), "Ray" (R)
    - Egyenes-szerű elem létrehozása.
    - Mindhárom mód szinte ugyanúgy viselkedik, ezért nincs külön leírás mindegyiknek.
    - Kattintással kiválasztunk egy pontot, ez meg lesz jegyezve mint `S` segédpont.
    - Ha már van megjegyzett `S` pont, akkor kattintásra a megjegyzett `S` pont és a kurzor alatti potenciális `E` pont
      között létrejön az egyenes-szerű elem.
      - Értelemszerűen a módtól függ, hogy melyik típusú egyenes-szerű elem jön létre.
  - "Parallel/Perpendicular"
    - Ponton keresztülmenő párhuzamos/merőleges létrehozása.
    - Kattintással kiválasztunk egy egyenes-szerű elemet, ez meg lesz jegyezve.
    - Ha már van megjegyzett pont, akkor kattintásra a kurzor alatti potenciális ponton keresztülmenő,
      a megjegyzett egyenes-szerű elemmel párhuzamos/merőleges egyenes jön létre.
      - Értelemszerűen a módtól függ, hogy melyik párhuzamos vagy merőleges egyenes jön létre.
  - "Circle" (C)
    - "Pont körül, ponton keresztül" típusú kör létrehozása.
    - Kattintással kiválasztunk egy pontot, ez meg lesz jegyezve mint a kör középpontja.
    - Ha már van megjegyzett pont, akkor kattintásra a megjegyzett középpont körüli, a kurzor alatti potenciális ponton
      keresztülmenő "Pont körül, ponton keresztül" típusú kör jön létre.
  - "Circle by length"
    - "Szakaszhossz sugarral, pont körül" típusú kör létrehozása.
    - Kattintással kiválasztunk egy szakaszt, ez meg lesz jegyezve.
    - Ha már van megjegyzett szakasz, akkor kattintásra a megjegyzett szakaszhosszú sugárral
      a kurzor alatti potenciális pont középpontú kör jön létre.
- Ha ez fentebbről nem derült volna ki: amint létrejön egy új elem, ami függ egy megjegyzett elemtől, akkor a program elfelejti.
- Esc gomb megnyomásakor, ha van megjegyezve elem, akkor felejtse azt el.
  - Ha megjegyzett elem kattintás előtt nem létezett, akkor nem kell kitörölni
- Akkor is felejtse el a megjegyzett elemet, ha módváltás történik.
  - Akor is történjen meg ez, ha ugyanabba a módba történik a váltás amiben eddig is volt a program.

## Mentés / Betöltés
- A síkon található geometriai elemek jelenlegi állapotát el lehet menteni és vissza lehet tölteni.
- Az mentett elemek adatait a program egy fájlban fogja tárolni.

### Mentési fájlformátum
- A mentési fájl kiterjesztése `.geom` legyen.
  - A kiterjesztést a fájlnév végére tenni nem feltétlenül kötelező, de ajánlott.
- A formátum specifikációja:
  - Szöveges formátum.
  - Minden sor egy elem deklarációjával feleltethető meg.
  - S sorok szóközzel tagolva értelmezendőek.
  - A sorok `p`, `l` vagy `c` karakterrel kezdődnek, az elem típusát meghatározva.
  - Utána következik egy egész szám mint azonosító
    - Az azonosítónak csak a saját típusán belül kell egyedinek lennie.
  - Ha a sor `l` kezdetű, jön egy egész szám (enum), ami eldönti, hogy szakasz/egyenes/félegyenes.
  - Utána következik egy egész szám (enum), ami az elem al-típusát határozza meg.
  - Utána al-típustól függően tizedestört vagy egész számok sora következik.
    - Az egész számok tipikusan azonosítot vagy enum-ot jelölnek.
    - A tizedestört számok tipikusan szabad paramétert jelölnek.
    - A pontos al-típusonkénti leírás nagyon hosszú lenne, csak a lényeget írtam le.
  - Ha több adat van egy sorban, mint ami kell az elem definícióhoz, akkor a extra adatokkal nem foglalkozik a program.
- A fenti formátum potenciálisan még változhat a fejlesztés során.
  - A felhasználónak úgysem kell tudnia a program nélkül módosítania a file tartalmát.
    - Ezért szerintem nem is lenne nagy probléma, ha nem lenne specifikálva a formátum.
    - De azért a biztonság kedvéért leírtam a jelenlegi állapotot.

### Futási argumentumok
- Alapvetően, ha elindítjuk a programot, akkor egy üres síkkal fogunk kezdeni.
- Ha a programnak argumentumként megadunk egy fájlt, akkor miután elindul a program,
  megpróbálja azt argumentumként megadott fájlt megnyitni.
  - Lásd az "Open" funkció leírását lentebb.
- Ha valamiért több argumentumot adunk a programnak, akkor is csak az elsővel foglalkozik.

### Mentési állapot
- A program számon tartja, hogy mentett-e már a felhasználó, és ha igen milyen fájlba mentett utoljára.
  - Ezt az adat a vásznon valahol meg van jelenítve
  - Alapból a program indításakor úgy veszi a program, hogy még nem volt mentés.
  - A különféle mentési funkciók változtatnak ezen az állapoton, a következő részben erről olvashatunk.

### Funkciók
- "Save as" / Mentés másként (Ctrl+Shift+S)
  - Megnyílik egy fájl-párbeszédablak, ahol meg kell adni, hogy melyik fájlba történjen a mentés.
    - Ha sikeres a betöltés, akkor úgy veszi, hogy a betöltött fájlba mentett utoljára.
    - Ha hiba történik, akkor a hiba jelezve lesz egy felugró ablakban.
- "Save" / Mentés (Ctrl+S)
  - Ha még nem volt mentés, akkor a "Save as" logika fusson le.
  - Ha már történt mentés, akkor abba a fájlba ment, ahova a utoljára mentett.
    - Ha hiba történik, akkor a hiba jelezve lesz egy felugró ablakban.
- "Open" / Megnyitás (Ctrl+O)
  - Megnyílik egy fájl-párbeszédablak, ahol ki kell választani, hogy melyik fájlból törénjen a betöltés.
    - Ha nem lett fájl választva, nem történik semmi
    - Ha ki lett választva egy fájl, a program rákérdez, hogy felül akarod-e írni a jelenlegi állapotot.
      - Ha nem, akkor nem lesz semmi sem felülírva.
      - Ha igen, akkor megpróbálja betölteni a fájlból az adatokat.
        - Ha hiba történik, akkor a hiba jelezve lesz egy felugró ablakban.
- "New" / Új (Ctrl+N)
  - A program rákérdez, hogy felül akarod-e írni a jelenlegi állapotot.
    - Ha nem, akkor nem lesz semmi sem felülírva.
    - Ha igen, akkor az össszes elem törtlésre kerül és a program úgy veszi, hogy még nem történt mentés.
- "Quit" / Bezárás (Ctrl+W)
  - A program rákérdez, hogy biztosan be akarod-e zárni a programot.
    - Ha igen, akkor bezár a program.
    - Ha nem, akkor fut tovább a program.

### Bezárás
Ha a programot megpróbáljuk bezárni, akkor a fenti "Quit" funkció logikája fusson le.
