# InfoC NHF - Geometria

- NHF3 megjegyzés
  - A [specfikáció](./SPECIFICATION.md) által meghatározott rengeteg feature közül már szinte mindegyik implementálva van,
    ezért inkább azt írom le, hogy mi hiányzik:
    - Felugró ablakok helyett a hibaüzenetek még csak a stdout-ra vannak kiírva.
    - Nincs megkérdezve betöltésnél, hogy biztosan meg akarod-e tenni.
    - A törlésnél nincsenek más színnel megjelenítve a törlendő elemek
  - További dolgok, amik még kell javítani:
    - Jelenleg csak statikus méretű tömbök vannak használva a betöltéséhez és az elemekere mutató pointerek számontartásához.
      - Talán láncolt listák lesznek használva, de még nem döntöttem
    - Ez nem volt benne a dokumentációban, de ha ráviszed a kurzorodat egy elemre, és a kattintás csinálna valamit,
      akkor logikus lenne, hogy más színnel rajzolja ki azt. Jelenleg csak az eredeti szín van használva.
    - A program kinézete még nem túl szép
    - Nincs *minden* hibakezelve
    - A dokumentáció erősen hiányos


- [Git repo](https://github.com/TomaSajt/infoc-nhf)
- [Specifikáció](./SPECIFICATION.md)
- [Projekt struktúra](./STRUCTURE.md)
- [Futtatás/Telepítés](./BUILD.md)
