A program elkészítéséhez telepítsd a következőeket:
 - CMake
 - SDL3
 - SDL3_ttf

Ha telepítve van a `nix` csomagkezelő, akkor könnyen beléphetsz a `nix-shell` paranccsal egy olyan fejlesztői könyezetbe,
amelyben az buildeléshez szükséges programok és könyvtárak elérhetőek.

A program CMake-et használ build-system-ként.

Könnyen futtatható a program a `./br.sh` helper script által, ami re-buildeli és futtatja a kódot.
