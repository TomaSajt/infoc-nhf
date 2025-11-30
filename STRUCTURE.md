- [main.c](./main.c)
  - Meghajtó kód, és random kódrészek, amik később még átkerülnek más fájlokba
- [draw.c](./draw.c)
  - Grafikus segédfüggvények
  - Használja a `SDL_gfx` könyvtárat
- [hover.c](./hover.c)
  - Kód, ami eldönti, hogy mi van jelenleg a kurzor alatt, vagy mi jöjjön létre, ha pontot akarunk létrehozni a kurzor alatt
- [save/data.c](./save/data.c)
  - A `.geom` fileformátumot feldolgozó és író logika
- [save/dialog.c](./save/dialog.c)
  - A mentés és betöltés logikát meghívó függvények és párbeszédablakok logikája.
- [mode/defs.c](./mode/defs.c)
  - Nagy statikus vtable a módokhoz (így nem kell mindenhol switch-et használni)
- [mode/*.c](./mode/)
  - A módok viselkedéseinek leírása
- [geom/defs.h](./geom/defs.h)
  - A geometriai elemek definíciói
- [geom/defs.c](./geom/defs.c)
  - A geometriai elemek létrehozását elősegítő függvények definíciói
- [geom/util.c](./geom/util.c)
  - Függvények logika elemek kiértékeléséhez és egyéb segédfüggvények
- [geom/state.c](./geom/state.c)
  - A vászon elemeit kezelő függvények
- [fonts/](./fonts/)
  - A program által használt betűtípusok

---

Megjegyzés: A `debugmalloc.h` nincs manuálisan `#include`-olva mindehova,
hanem a `CMakeLists.txt`-ben compiler argumentum által force-include-olva van minden fileba automatikusan
