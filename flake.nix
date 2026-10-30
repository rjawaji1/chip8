{
  description = "";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = inputs @ { flake-parts, ... }:
  flake-parts.lib.mkFlake { inherit inputs; } {
    systems = ["x86_64-linux" "aarch64-linux" "aarch64-darwin"];

    perSystem = { pkgs, ... }: {
      devShells.default = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
        inputsFrom = [ pkgs.sdl3 ];

        packages = with pkgs; [
          clang-tools
          cmake
          ninja
          pkg-config
        ];
      };
    };
  };
}
