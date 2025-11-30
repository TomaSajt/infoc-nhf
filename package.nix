{
  lib,
  stdenv,
  nix-gitignore,
  fetchFromGitHub,
  cmake,
  sdl3,
  sdl3-ttf,

}:

stdenv.mkDerivation {
  name = "infoc-nhf";

  src = nix-gitignore.gitignoreSource [ ] ./.;

  strictDeps = true;

  nativeBuildInputs = [ cmake ];

  buildInputs = [
    sdl3
    sdl3-ttf
  ];

  cmakeFlags = [
    (lib.cmakeFeature "FETCHCONTENT_SOURCE_DIR_SDL3_GFX" "${fetchFromGitHub {
      owner = "sabdul-khabir";
      repo = "SDL3_gfx";
      rev = "27b177ba86f13ea3c106c26fcd48169f482a10e1";
      hash = "sha256-NWxfKHNPWV1IBP+sAmoebvilgzI6+AcBr+ghQo5WCUY=";
    }}")
  ];
}
