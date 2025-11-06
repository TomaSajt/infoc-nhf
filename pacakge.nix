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
      rev = "add4f8c517d7b0275a6dcd67638d073be051ea88";
      hash = "sha256-2oRBylxVFdMg0yaALaHDH2uWQtAVVMVH+re+AAwN49c=";
    }}")
  ];
}
