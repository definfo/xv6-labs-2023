{
  description = "A Nix-flake-based C/C++ development environment";

  inputs.nixpkgs.url = "https://flakehub.com/f/NixOS/nixpkgs/0.1.*.tar.gz";

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [ "x86_64-linux" "aarch64-linux" "x86_64-darwin" "aarch64-darwin" ];
      forEachSupportedSystem = f: nixpkgs.lib.genAttrs supportedSystems (system: f {
        pkgs = import nixpkgs { inherit system; };
        pkgsCross = import nixpkgs { inherit system; crossSystem.config = "riscv64-unknown-linux-gnu"; };
      });
    in
    {
      devShells = forEachSupportedSystem ({ pkgs, pkgsCross }: {
        default = pkgsCross.mkShell.override
          {
            # Override stdenv in order to change compiler:
            # stdenv = pkgsCross.riscv64.stdenv;
          }
          {
            packages = with pkgs; [ gcc gdb ];
          };
      });
    };
}

