# InfoC NHF Specifikáció

- A program neve még később lesz eldöntve, nem része a specifikációnak!

## Rövid program összefoglaló
- grafikus felületes síkgeometriai szerkesztőprogram
- geometriai elemek egymással függőségi kapcsolatban
- létrehozás után is változtathatóak az elemek pozíciói,
  - pl: ha elmozdul egy pont, akkor a tőle függő elemek (pl: rajta kereszülhaladó egyenesek) is vele változnak
- állapot fájlba mentése és fájlból betöltése

## Programleírás
- A megjelenített szövegek angol nyelvűek.
- A program grafikus megjelnítést használ.
- Indítás után megjelenik egy grafikus felület, amit nevezzünk vászonnak.
- A végtelen sík egy felhasználó által állítható része van megjelenítve meg a vásznon, nevezzül ezt a fókuszált területnek.
- A végtelen sík tartalmazza majd a geometriai elemeket.
- A végtelen sík eredetileg nem tartalmaz semmit.
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
      - ha olyan helyre akarnánk húzni, ami nem az egyenesen van,
        akkor az egyenes ahhoz leközelebb eső pontjához mozduljon a csúszka
    - ha félegyenestől vagy szakasztól függ, akkor a csúszka annak csak a végpontjáig tud csúszni
    - a csúszka állapota egy szabad `prog` paraméterben van tárolva
      - a pont pozíciója megkapható a `(1-prog)*S + prog*E` képlet által,
        ahol az S és E az egyenes-szerű elem segédpontjai (erről később bővebben)
        - prog [0;1] intervallumban, ha a csúszka szakaszon található
        - prog [0;inf), ha a csúszka félegyenesegyen található
        - prog (-inf;inf), ha a csúszka egyenesen található
  - "egyenesek metszéspontja"
    - két egyenes-szerű elemtől függ
    - az egyenes-szerű elemek valódi típusuktól függetlenül mindig teljes egyenesként értelmezendőek metszéspontszámításnál
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
    - az egyenes-szerű elemek valódi típusuktól függetlenül mindig teljes egyenesként értelmezendőek metszéspontszámításnál
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
    - két ponttól függ, de a pontok sorrendje számít
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
  - "középpont és külső pont"
    - két ponttól függ, a középponttól és a külső ponttól
    - a kör középpontja értelemszerűen a megadott középpont
    - a kör sugara akkora, hogy a kör keresztülmenjen a külső ponton
  - "középpont és szakaszhossz"
    - egy ponttól és egy szakasztól függ
    - a kör középpontja a pontnál található
    - a kör sugara pontosan a szakasz hosszával egyenlő

### Módok
- A programban különböző módok közül lehet választani
- A jelenlegi módot egy indikátor jelzi
  - TODO: eldönteni, hogy milyen indikátor legyen
- lehetséges módok:
  - "Move" - mozgatás
    - A szabadsági fokkal rendelkező pontok "literál" vagy "csúszka"
  - "Delete" - törlés
  - "Point" - pont
  - "Midpoint" - felezőpont
  - "Line" - egyenes
  - "Parallel/Perpendicular" - párhuzamos/merőleges
  - "Circle" - kör
  - "Circle by length" - kör, hossz alapján
