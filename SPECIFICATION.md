# InfoC NHF Specifikáció

## Rövid program összefoglaló
- a program egy grafikus felületű síkgeometriai szerkesztőprogram
- támogatott geometriai elemek: pontok, egyenesek/félegyenesek/szakaszok, körök
- az elemek egymással függőségi viszonyban állnak
- létrehozás után is változtathatóak az elemek pozíciói,
  - pl: ha elmozdul egy pont, akkor a tőle függő elemek (pl: rajta kereszülhaladó egyenesek) is vele változnak
- a program képes az elemek definícióinak fájlba mentésére és fájlból betöltésére

## Programleírás
- A program neve még később lesz eldöntve, nem része a specifikációnak!
- A megjelenített szövegek angol nyelvűek.
- A program grafikus megjelnítést használ.
- Indítás után megjelenik egy grafikus felület, amit nevezzünk vászonnak.
- A végtelen sík egy felhasználó által állítható része van megjelenítve meg a vásznon, nevezzül ezt a fókuszált területnek.
- A végtelen sík tartalmazza majd a geometriai elemeket.
- A végtelen sík kezdő állapotában nem tartalmaz egy elemet sem.
- A fókuszált terület méretén változtatni fel/le görgetéssel lehet, nevezzük ezt "zoomolásnak":
  - görgetés fel -> kisebb terület látszódik
  - görgetés le -> nagyobb terület látszódik
  - A kurzor zoomolás közbeni pozíciója fix pontnak számít a síkon,
    így a kurzor közelében lévő objektumok a kurzor közelében is maradnak a zoomolás után.
- Fontos megjegyzés
  - az elemek pozícióját egér segítségével lehet beállítani, nincs lehetőség szövegdoboz/konzolablak segítségével pozíciókat bevinni

### Geometriai elemek
- a program pontokkal, egyenes-szerű elemekkel és körökkel képes dolgozni
- az egyenes-szerű elemek alatt értsük az egyenest, a félegyenest és a szakaszt 
- az elemek egymással függőségi viszonyban vannak
- a függőségi viszonyok egy irányított körmentes gráfot alkotnak (DAG)
- az egyetlen elemtípus ami nem függ semmitől a pont "literál"
- ha frissül egy elem értéke, a többi elem ami tőle függ is kell, hogy frissüljön
- "érvénytelen" állapotban lévő elemek is jöhetnek létre
  - ez tipikusan akkor történik meg, ha egy metszéspont nem létezik
  - de akkor is, ha egy elem függ egy olyan másik elemtől, ami "érvénytelen" állapotban van
  - az érvénytelen állapotban lévő elemek nincsenek megjelenítve

#### Elem-típusok
- pont
  - "literál"
    - nem függ más elemektől
    - szabadon mozgatható
  - "felezőpont"
    - két másik ponttól függ
    - félúton található a másik két pont között
      - lényegében a két pont közötti szakasz felezőpontja,
        bár nem szükséges, hogy tényleg legyen köztük szakasz
  - "csúszka egyenesen"
    - egy egyenes-szerű elemtől függ
    - csak az egyenes mentén csúsztatható el
      - ha olyan helyre akarnánk húzni, ami nem az egyenesen-szerű elemen van,
        akkor az egyenes-szerű elem ahhoz leközelebb eső pontjához mozduljon a csúszka
    - a csúszka állapota egy szabad `prog` paraméterben van tárolva
      - a pont pozíciója megkapható a `(1-prog)*S + prog*E` képlet által,
        ahol az S és E az egyenes-szerű elem segédpontjai (erről később bővebben)
        - prog [0;1] intervallumban, ha a csúszka szakaszon található
        - prog [0;inf) intervallumban, ha a csúszka félegyenesegyen található
        - prog (-inf;inf) intervallumban, ha a csúszka egyenesen található
  - "egyenesek metszéspontja"
    - két egyenes-szerű elemtől függ
    - az egyenes-szerű elemek mindig teljes egyenesként értelmezendőek metszéspontszámításnál
    - a pont a két egyenes metszéspontjánál található
    - ha a két egyenes párhuzamos, akkor a pont "érvénytelen" állapotba kerül
  - "csúszka körön"
    - egy körtől függ
    - csak a kör mentén csúsztatható el
      - ha olyan helyre akarnánk húzni, ami nem a körön van,
        akkor az kör ahhoz legközelebb eső pontjához mozduljon a csúszka
    - a csúszka állapota egy szabad `prog` paraméterben van tárolva
      - a pont pozíciója megkapható a `C + R*(cos(2pi*prog),sin(2pi*prog))` képlet által,
        ahol C a kör középpontja és R a sugara
        - prog [0;1) intervallumban
  - "egyenes és kör metszéspontja"
    - egy egyenes-szerű elemtől és egy körtől függ
    - az egyenes-szerű elem mindig teljes egyenesként értelmezendő metszéspontszámításnál
    - a pont az egyenes és a kör egyik metszéspontjánál található
    - egy extra paraméter tárolja, hogy melyik metszéspontról beszélünk
    - ha nincs metszéspont, akkor "érvénytelen" állapotú a pont
  - "körök metszéspontja"
    - két körtől függ
    - a pont a két kör egyik metszéspontjánál található
    - egy extra paraméter tárolja, hogy melyik metszéspontról beszélünk
    - ha nincs metszéspont, akkor "érvénytelen" állapotú a pont
- egyenes-szerű elem
  - "pontból pontba"
    - két ponttól függ
      - az első pont számít az S "start" segédpontnak
      - a másik pont számít az E "end" segédpontnak
    - választható, hogy szakaszként, félegyenesként vagy egyenesként legyen értelmezve az elem
      - ha szakasz, akkor az S és E pont közötti szakasz
      - ha félegyenes, akkor az az S pontjából az E felé a végtelenbe tartó félegyenes
      - ha egyenes, akkor az S és E pontokon keresztül haladó egyenes
  - "párhuzamos"
    - egy egyenes-szerű elemtől és egy ponttól függ
    - csak egyenes típusú lehet
    - az egyenes keresztülhalad a ponton és párhuzamos az egyenes-szerű elemmel
  - "merőleges"
    - egy egyenes-szerű elemtől és egy ponttól függ
    - csak egyenes típusú lehet
    - az egyenes keresztülhalad a ponton és merőleges az egyenes-szerű elemre
- kör
  - "pont körül, ponton keresztül"
    - két ponttól függ, a középponttól és egy külső ponttól
    - a kör középpontja a megadott középpont
    - a kör sugara akkora, hogy a kör keresztülmenjen a külső ponton
  - "pont körül, szakaszhossz sugárral"
    - egy ponttól és egy szakasztól függ
    - a kör középpontja a pontnál található
    - a kör sugara pontosan a szakasz hosszával egyenlő

### Módok
- A programban különböző módok közül lehet választani
- A módok kategóriákba vannak sorolva
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
    - "Ray"
    - "Parallel"
    - "Perpendicular"
  - "Circles" (4)
    - "Circle" (C)
    - "Circle by length"
- A kategória első módjába lépni a zárójelbe írt gombok egyikének megnyomásával lehet
- Egyes módoknak van külön gyorsgombja, szintén zárójelbe írva
- A kategória módjai között a Tab gomb megnyomásával lehet váltani
  - Ha még nem értük el a kategória utolsó módját, akkor az alatta következő módba lépünk be
  - Ha már elértük az utolsót, akkor visszalépünk az kategóra első módjába

- módok:
  - "Move"
    - Pont mozgatása
    - Belépő billentyűk: 1, M, Escape
    - A szabadsági fokkal rendelkező pontokat lehet mozgatni ("literál" vagy "csúszka" pontokat)
    - Bal klikk lenyomásával a kurzor alatti pontot "megfogjuk", egérr mozgatásával arrébb helyezzük,
      majd a mozgatás befejezéséhez elengedjük a bal klikket.
  - "Delete"
    - Elemek kaszkádosított törlése
    - Belépő billentyűk: 2, D
    - A kurzor alatti elem és az összes tőle függő elem törlésre kerül bal klikkelésre
    - Ha a kurzort az egyik elem fölé visszük,
      akkor az elem és a tőle függő elemek más színnel (esetleg piros színnel)
      lesznek rajzolva, hogy lássuk, hogy mi fog törlésre kerülni
  - "Point"
    - Pont létrehozása
    - Belépő billentyűk: 3, P
    - Bal klikkelésre az alábbiak közül a fentről lefele sorrendben a legkorábban bekövetkező kapjon prioritást:
      - Ha egy már létező pontra kattintunk, nem jön létre semmi
      - Ha két egyenes-szerű elemre kattintunk rá egyszerre, azaz mindkét elem a kurzor alatt van,
        akkor "egyenesek metszéspontja" jön létre
      - Ha egy egyenes-szerű elemre és egy körre kattintunk rá egyszerre,
        akkor "egyenes és kör metészpontja" jön létre
      - Ha két körre kattintunk rá egyszerre,
        akkor "körök metszéspontja" jön létre
      - Ha egy egyenes-szerű elemre kattintunk, "csúszka egyenesen" jön létre
      - Ha egy körre kattintunk, "csúszka körön" jön létre
      - Ha üres területre kattintunk "literál" pont jön létre
    - A kurzor mozgatása során kattintás nélkül is előnézetben látjuk, hogy hova fog létrejönni az új pont
    - TOVÁBBIAKBAN FONTOS:
      - A további módokban, amikor arról van szó, hogy egy pontra kattintunk,
        akkor valójában kattinthatunk bárhova, és ha nem egy pontra kattintunk,
        akkor létrejön a fenti logika szerint egy új pont a kattintás következtében
      - Tehát a többi módban is, ha éppen nem egy létező ponton van a kurzor,
        akkor már előnézetben látjuk a pontot, ami kattintásra létrejönne
  - "Midpoint"
    - Felezőpont létrehozása
    - Bal klikkeléssel kiválasztunk egy pontot, ez meg lesz jegyezve
    - Ekkor a megjegyzett pont és a kurzor alatti, (nem feltétlenül létező) pont
      között félúton megjelenik előnézetben a felezőpont
    - Bal klikkelésre az előnézetben látható pont tényleg létrejön
      az elsőnek kiválasztott pont és a kurzor alatti pont között
  - "Segment", "Line", "Ray"
    - Egyenes-szerű elem létrehozása
    - 
  - "Parallel/Perpendicular" - párhuzamos/merőleges
  - "Circle" - kör
  - "Circle by length" - kör, hossz alapján
