# Felhasználói dokumentáció

- Zoomolás fel/le görgetéssel
  - A kurzor zoomolás közbeni pozíciója fixpontnak számít a síkon

## Kategóriák és módok
- A programban különböző módok közül lehet választani.
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
- A jelenlegi mód és kategória neve kiemelten jelenik meg a bal felső sarokban.
- Egy kategóriát kiválasztani a fenti listában a kategória neve mellett zárójelbe írt billentyű megnyomásával lehet.
  - Ekkor a kategórián belül legutóbb használt módba fog lépni a program.
- Fontosabb módoknak van külön gyorsbillentyűje, szintén zárójelbe írva.
- A Tab (vagy Shift+Tab) megnyomásával a kiválasztott kategória módjai között lehet lefele (vagy felfele) váltani.
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
    - Ha a kurzort az egyik elem fölé visszük, akkor maga az elem és a tőle függő elemek
      piros színnel lesznek megjelölve, hogy kattintásra mi fog törlésre kerülni.
  - "Point" (P)
    - Pont létrehozása.
    - Elemek metszéspontjára kattintással a metszéspontnál jön létre pont.
    - Egy körre vagy egyenes-szerű elemre kattintáskor csúszka jön létre.
    - Üres terültre kattintáskor szabadon mozgatható pont jön létre.
    - A kurzor mozgatása során kattintás nélkül is előnézetben látjuk, hogy hova fog létrejönni az új pont.
    - A többi módban is, amikor nem egy létező pontra kattintunk, akkor is létrejön egy pont a kurzor alatt a fenti szabályok szerint
  - "Midpoint"
    - Felezőpont létrehozása.
    - Kattintással kiválasztunk egy pontot, ez meg lesz jegyezve.
    - Ha már van megjegyzett pont, akkor kattintásra a megjegyzett pont és a kurzor alatti potenciális pont
      között "Felezőpont" típusú pont jön létre.
  - "Segment" (S), "Line" (L), "Ray" (R)
    - Egyenes-szerű elem létrehozása.
    - Kattintással kiválasztunk egy pontot, ez meg lesz jegyezve mint az egyik segédpont
    - Ha már van megjegyzett segédpont pont, akkor kattintásra a megjegyzett pont és a kurzor alatti potenciális pont
      között létrejön az egyenes-szerű elem.
  - "Parallel", "Perpendicular"
    - Ponton keresztülmenő párhuzamos/merőleges létrehozása.
    - Kattintással kiválasztunk egy egyenes-szerű elemet, ez meg lesz jegyezve.
    - Ha már van megjegyzett egyenes-szerű elem, akkor következő kattintásra a kurzor alatti potenciális ponton keresztülmenő,
      a megjegyzett egyenes-szerű elemmel párhuzamos/merőleges egyenes jön létre.
  - "Circle" (C)
    - "Pont körül, ponton keresztül" típusú kör létrehozása.
    - Kattintással kiválasztunk egy pontot, ez meg lesz jegyezve mint a kör középpontja.
    - Ha már van megjegyzett pont, akkor kattintásra a megjegyzett középpont körüli, a kurzor alatti potenciális ponton
      keresztülmenő kör jön létre.
  - "Circle by length"
    - "Szakaszhossz sugárral, pont körül" típusú kör létrehozása.
    - Kattintással kiválasztunk egy szakaszt, ez meg lesz jegyezve.
    - Ha már van megjegyzett szakasz, akkor kattintásra a megjegyzett szakaszhosszú sugárral
      a kurzor alatti potenciális pont középpontú kör jön létre.
- Amikor sikeresen létrejön egy új elem, akkor a program elfelejti a megjegyzett elemet.
- Esc billentyű megnyomásakor vagy módváltáskor el lesz felejtve a megjegyzett elem.

## Mentés / Betöltés
- A program a saját `.geom` fájlkiterjesztését használja mentésre.
- A képernyő bal alsó sarkában látszódik a jelenleg betöltött file neve.
- A programnak futtatáskor meg lehet adni egy fájlnevet, hogy azt induláskor azonnal betöltse.
- Billentyűkombinációk
  - "Open" / Megnyitás (Ctrl+O)
  - "Save as" / Mentés másként (Ctrl+Shift+S)
  - "Save" / Mentés (Ctrl+S)
  - "New" / Új (Ctrl+N)
  - "Quit" / Bezárás (Ctrl+W)

