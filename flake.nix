{
  description = "An abstraction layer on top of Geant4";

  inputs = {
    nixpkgs         .url = "github:NixOS/nixpkgs/nixos-25.11";
    flake-compat = { url = "github:NixOS/flake-compat"; flake = false; };
    nosys           .url = "github:divnix/nosys";
  };

  outputs = inputs @ {
    self,
    nosys,
    nixpkgs, # <---- This `nixpkgs` still has the `system` e.g. legacyPackages.${system}.zlib
    ...
  }:
    let
      giturl       = self.sourceInfo.url  or "nullurl";
      gitpath      = self.sourceInfo.path or "nullpath";
      giturlorpath = self.sourceInfo.url or self.sourceInfo.path or "notgit";
      gitrev       = self.rev or "dirty";
      outputs      = import ./flake/outputs.nix;
      system       = [ "x86_64-linux" "x86_64-darwin" "aarch64-linux" "aarch64-darwin" ];
    in  nosys (inputs // { inherit system; }) outputs //
        {giturl=giturl; gitpath=gitpath; giturlorpath=giturlorpath; gitrev=gitrev;};
}
