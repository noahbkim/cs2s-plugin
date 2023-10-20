# CS2 Server Plugin Template

This repository facilitates convenient development of CS2 server plugins.
It uses CMake instead of AMBuild, but in exchange, the build is much more flexible.

## CS2S

This repository comes bundled with [`cs2s`](https://github.com/noahbkim/cs2s), a Docker Compose setup I created to develop plugins in a VM.
Currently, I can only guarantee plugins will build in the container.

```bash
# Build the compose setup. Provisions a persistent volume for the game files
# under `/cs2` and the directory where this repository is cloned as `/work`.
# Don't worry about having to repeat the CS2 installation process; the `/cs2`
# volume persists between `docker compose up` and `docker compose down` calls.
docker compose up -d  # --detach and run in the background

# Attach to the Docker container and open a shell. `bash` is always available.
docker compose cs2s exec zsh

# Install the CS2 client (there's no server build yet). You should only have to
# do this once unless you somehow wipe your live Docker volumes.
cs2s-install  # INSIDE THE DOCKER COMPOSE SHELL

# Proceed to the next section!

# You can always leave the container running in the background (it'll take up
# minimal resources because it won't be executing anything if you have no
# attached shell), but if you do want to close it (to rebuild the image, etc.):
docker compose down
```

Once you've run `docker compose cs2s exec zsh`, you can proceed to the setup.

## Setup

```bash
# Install the vendored packages (included as submodules). Most of them clone
# shallowly so they don't take up too much space.
git submodule update --init --recursive

# Generate a release (default) and debug Conan profile for dependencies. Note
# that you actually have to edit the second one to change it to debug! This is
# already done for you in the cs2s container.
conan profile detect               # Automatically names it `default`
conan profile detect --name debug  # Edit this such that `build_type=Debug`

# Install your dependencies. You don't have to install the dependencies of both
# profiles at once, it's just here for completeness. Note that if you don't 
# specify --profile, it automatically uses our default one, which is Release.
# The directory you install everything can be named anything/placed anywhere,
# it just has to be accessible from where you invoke CMake. I prefer to match
# the naming scheme we'll use for the build directories.
conan install . --build missing --output-folder conan-build-release
conan install . --build missing --output-folder conan-build-debug --profile debug

# Make a build directory. I tend to adhere to CLion's naming convention, but it
# doesn't matter what you choose.
mkdir cmake-build-debug
cd cmake-build-debug

# Run CMake. Note that we have to refer to the `conan_toolchain.cmake`
# generated in the corresponding `conan install` directory to the build type
# we want.
cmake .. -DCMAKE_TOOLCHAIN_FILE=../conan-build-debug/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=debug
```

## Development

This section is unfinished.

```bash
# Build metamod from the locally-vendored packages.
make -j cs2metamod-build

# TODO: explain overlays.

# Create a new overlay. You only have to do this once. You can name it anything
# you want; I've chosen `main` out of a lack of creativity.
cs2s-overlay add main

# TODO: make this part of overlay

# Copy the metamod files to the overlay.
cp -r metamod/package/addons/ ~steam/main/game/csgo/
chmod -R 775 ~steam/main/game/csgo/addons/
chown -R steam:steam ~steam/main/game/csgo/addons/

# TODO: make this part of overlay

# Modify the gameinfo.gi and add
#   Game    csgo/addons/metamod
#   GameBin csgo/addons/metamod/bin
# Under GameInfo/FileSystem/SearchPaths
# TODO: figure out if this is actually correct or if you can do less
nano ~steam/main/game/csgo/gameinfo.gi

# Build your plugin
make -j

# TODO: make this part of overlay
# TODO: do the actual VDF stuff
cp cs2s-plugin.so ~steam/main/game/csgo/addons/
chmod -R 775 ~steam/main/game/csgo/addons/cs2s-plugin.so
chown -R steam:steam ~steam/main/game/csgo/addons/cs2s-plugin.so
```

## TODO

- Fix funchook
- VDF stuff
- Extend overlay and docs

## References

To be categorized/explained.

- https://github.com/Soren90/cs2-docker/blob/main/Dockerfile
- https://github.com/steamcmd/docker
- https://developer.valvesoftware.com/wiki/Counter-Strike_2/Dedicated_Servers
- https://developer.valvesoftware.com/wiki/SteamCMD
- https://developer.valvesoftware.com/wiki/Metamod_(Source)
- https://github.com/truckersmp-cli/truckersmp-cli/issues/331  # Bug with 0x10C state
- https://github.com/alliedmodders/metamod-source
- https://wiki.alliedmods.net/Building_SourceMod
- https://github.com/Source2ZE/CS2Fixes/tree/main
- https://github.com/Poggicek/cs2-docs
- https://cs2.poggu.me/
- https://wiki.archlinux.org/title/Overlay_filesystem
- https://docs.google.com/document/d/1v8QtbUtGAf6OapE9iWn4Y4STA7Scn_hr1Yih1nf2_BA/edit
- https://github.com/kubo/funchook
- https://wiki.alliedmods.net/AMBuild_Tutorial
- https://cmake.org/cmake/help/book/mastering-cmake/chapter/Custom%20Commands.html
- https://stackoverflow.com/questions/9298278/cmake-print-out-all-accessible-variables-in-a-script
- https://stackoverflow.com/questions/1260748/how-do-i-remove-a-submodule
- https://www.reddit.com/r/cs2/comments/16zjaid/counterstrike_2_metamod_installation_guide/
