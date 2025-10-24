# InfoC NHF - Geom

## Rövid leírás

A program fő célja síkgeometriai műveletek elvégzése és megjelenítése grafikus felületen.

Egyes geometriai elemek definiálhatóak más elemek segítségével:
például két pontból definiálható a rajtuk keresztülhaladó egyenes, vagy a két pont közti szakasz felezőpontja.

A felület interakítv: egyes pontokat (amik megengedik), a felhasználó képes mozgatni.
Egy pont pozíciójának változtatásával a többi olyan geometriai elem is változik, ami bármilyen módon is függgött a pontnak a pozíciójától.

## Elemek deklarálása
### Pontok deklarálása
- **literál**:
  - függés: nem függ más geometriai elemtől
  - létrehozzás: "pont mód" -> katt a sík üres területére
  - szabadság: szabadon mozgatható bárhova
- **szakaszfelező pont**:
  - függés: p1 pont, p2 pont
  - definícó: p1 és p2 között félúton lévő pont
    - `(p1+p2)/2`
  - létrehozás: "felező mód" -> katt egyik pontra -> katt másik pontra
  - szabadság: nem mozgatható
- **csúszka egyenesen/félegyenesen/szakaszon**:
  - függés: l egy/félegy/szak, t csúszkaparaméter
  - definíció: egyenesen elhelyezkedő pont
    - az elhelyezkedés függ a t paramétertől
      - lineáris interpoláció az egy/félegy/szak segédpontjai között
        - `lerp(l.s, l.e, t)`
    - t [0;1], ha l szakasz
    - t [0;inf), ha l félegyenes
    - t (-inf;inf), ha l egyenes
  - létrehozás: "pont mód" -> katt l-re
  - szabadság: a t paraméter szabadon változtatható az engedélyezett intervallumában
    - a felhasználó ezt a pont csúsztatásával éri el
    - ha a felhasználó kurzora letér az egyenesről, akkor annak legközelebbi pontjához csúszik a pont
- **egyenes-egyenes metszéspont**:
  - függés: l1 egy/félegy/szak, l2 egy/félegy/szak
  - definíció: l1 és l2 egyenesekkét legyenek értelmezve, ezek metszéspontja
    - ha nincs metszéspont, vagy végtelen metszéspont van, akkor a pont legyen érvénytelen
  - létrehozás: "pont mód" -> katt két egy/félegy/szak metszéspontjára
  - megjegyzés: ha például egy szakasz és egy egyenes metszéspontját hozzuk létre, akkor azt csak akkor tehetjük meg, ha tényleg metszik egymást,
                de ha később a szakasz olyan pozícióba kerül, hogy csak a szakasz meghosszabbításával lenne metszéspont, attól még érvényesnek számít az a pont
- **csúszka körön**:
  - függés: c kör, t csúszkaparaméter
  - definíció: körön elhelyezkedő pont
    - az elhelyezkedés függ a t paramétertől
      - `c.center + c.radius * (cos(tau*t), sin(tau*t))`
    - t periódusa 1, tehát csak a törtrésze érdekel minket
  - létrehozás: "pont mód" -> katt c-re
  - szabadság: a t paraméter szabadon változtatható
    - a felhasználó ezt a pont csúsztatásával éri el
    - ha a felhasználó kurzora letér a körről, akkor annak legközelebbi pontjához csúszik a pont

