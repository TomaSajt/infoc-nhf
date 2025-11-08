let
  pkgs = import <nixpkgs> { };
in
pkgs.mkShell {
  inputsFrom = [ (pkgs.callPackage ./package.nix { }) ];
}
