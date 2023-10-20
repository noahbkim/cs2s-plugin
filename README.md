# CS2 Server Plugin Template

This repository facilitates convenient development of CS2 server plugins.
It uses CMake instead of AMBuild, but in exchange, the build is much more flexible.

## CS2S

This repository targets [`cs2s`](https://github.com/noahbkim/cs2s), a Docker Compose setup I created to develop plugins in a virtual machine.
Currently, I can only guarantee plugins will build in the container.

**Before you do anything, install the dependencies vendored as submodules**.
You will notice each command is prefixed with a full prompt; this is to distinguish between commands run on the `host` and on the Docker `container`.

```console
# Install the vendored packages (included as submodules). Most of them clone
# shallowly so they don't take up too much space.
user@host cs2s-plugin $ git submodule update --init --recursive
```

Next, optionally, build the Docker environment.
This requires having Docker installed; I've only tested with Docker Desktop.
If you would rather develop directly on your machine, the only thing you need to do for this section is install CS2.

```shell
# Build the compose setup. Provisions a persistent volume for the game files
# under `/cs2` and the directory where this repository is cloned as `/work`.
# Don't worry about having to repeat the CS2 installation process; the `/cs2`
# volume persists between `docker compose up` and `docker compose down` calls.
user@host cs2s-plugin $ docker compose up -d  # --detach and run in the background

# Attach to the Docker container and open a shell. `bash` is always available.
# You can exit this like any other terminal session, e.g. ^D or `logout`.
user@host cs2s-plugin $ docker compose cs2s exec zsh

# Running this command should open a terminal in your virtual container. Your
# prompt will hopefully look something like the below where instead of
# `container`, you'll see a hex string (the container's Docker ID). If you
# like the `zsh` config, you can either fuck with it yourself or just use
# `docker compose cs2s exec bash`.
user@container /work $

# You can leave the container running in the background without a shell
# attached (and it'll take up minimal resources because it won't be executing
# anything). If you do want to close it (to rebuild the image, save resources,
# etc.), run the following back from your host (not inside the container):
user@host cs2s-plugin $ docker compose down
```

Once the installation is complete, you can proceed to the setup.

## Setup

Next, you'll need to get your C++ build environment set up.
I've elected to use CMake rather than AMBuild because I wanted to use [Conan](https://conan.io/), a C++ package manager.
As you can see in [`conanfile.txt`](./conanfile.txt), we're installing `protobuf` and `abseil`.

In the Docker container, this repository directory is mounted as `/work`, which I'll use in the console prompt.
If you're developing locally, replace `/work` with your `cs2s-plugin` directory.

```shell
# THIS STEP IS ALREADY DONE FOR YOU IN THE CS2S DOCKER CONTAINER.
# Generate a release (default) and debug Conan profile for dependencies. Note
# that you actually have to edit the second one to change it to debug!
/work $ conan profile detect               # Automatically names it `default`
/work $ conan profile detect --name debug  # Edit this such that `build_type=Debug`

# Install the C++ dependencies. You don't have to install the dependencies of
# both profiles at once, it's just here for completeness. Note that if you don't 
# specify --profile, it automatically uses our default one, which is Release.
# The directory you install everything can be named anything/placed anywhere,
# it just has to be accessible from where you invoke CMake. I prefer to match
# the naming scheme we'll use for the build directories.
/work $ conan install . --build missing --output-folder conan-build-release
/work $ conan install . --build missing --output-folder conan-build-debug --profile debug

# Make a build directory. I tend to adhere to CLion's naming convention, but
# you could just choose `build` if you don't wanna type so much.
/work $ mkdir cmake-build-debug && cd cmake-build-debug
# mkdir cmake-build-release && cd cmake-build-release

# Run CMake. Note that we have to refer to the `conan_toolchain.cmake`
# generated in the corresponding `conan install` directory to the build type
# we want.
/work/cmake-build-debug $ cmake .. -DCMAKE_TOOLCHAIN_FILE=../conan-build-debug/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=debug
# /work/cmake-build-debug $ cmake .. -DCMAKE_TOOLCHAIN_FILE=../conan-build-release/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=release
```

This configuration has only been tested end-to-end in the Docker container.
It doesn't raise any fatal errors for me on Windows, but it definitely won't build correctly either.
It is, however, sufficient to appease CLion and get (fairly accurate) intellisense on my host/development machine.

## Development

The development process is still being ironed out.
Expect this section to change
For now, users who are not using the Docker Compose setup are expected to extrapolate.

The `cs2s` Docker setup allows us to leverage OverlayFS to create lightweight clones of our (extremely heavy) game installation.
We do this by compositing two layers: the lower, read-only `/cs2` volume which contains our real game installation, and the upper, read-write `tmpfs` mount where our development changes are stored.
The result looks like a complete copy of `/cs2`, but it only needs to store files that have been changed, added, or deleted.

This has all been wrapped up into a convenient script, `cs2s-overlay`.

**IMPORTANT**: because `SteamCMD` doesn't like to be run as root, the game installation and overlays are owned by `steam:steam`.
Make sure files you change are readable/executable by `steam`--it's best to just `su` whenever you're manually fiddling with them.
The packaged `cs2s-` scripts take care of this already.

```console
# Create a new overlay. You only have to do this once. You can name it anything
# you want; I've chosen `main` out of a lack of creativity. Note that when your
# container is deleted (e.g. by `docker compose down`), your overlays will be
# erased. Only `/work` (a mount from your host machine) and `/cs2` (a virtual
# mount configured in the `cs2s/compose.yaml` are persistent.
root@container /work/cmake-build-debug $ cs2s-overlay add main

# Next, you'll need to modify the gameinfo.gi so it properly loads Metamod.
# TODO: there's gotta be a cleaner way to do this.
#   Game    csgo/addons/metamod
# Under GameInfo/FileSystem/SearchPaths
root@container /work/cmake-build-debug $ nano ~steam/main/game/csgo/gameinfo.gi

# Build your plugin--this is also how you do it if you're developing locally.
root@container /work/cmake-build-debug $ make -j

# The build assembles all of the artifacts--both from `metamod` and from your
# plugin--into a single `package` directory. You'll want to copy its contents
# (`addons/*` and anything else that ends up in there) into your overlay's game
# directory, e.g. `~steam/main/game/csgo/` as well as update the permissions
# and owners. I've included a script that automates this as well:
root@container /work/cmake-build-debug $ cs2s-copy main

# You can the run the server either manually (you must `su steam` for it to
# work property) or use the overlay script (which `exec`s as `steam`):
root@container /work/cmake-build-debug $ cs2s-overlay start main

# TODO: make this better and also figure out how the `.vdf` should work.
# From there, the usual metamod steps apply. Note that currently, it builds a
# verbose but safe plugin structure, so you have to write (in the console).
# You can create a symlink in `csgo` to type less :)
meta load addons/cs2s-plugin/cs2s-plugin
```

Note that I haven't actually spent time figuring out how to use the generated
VDF file correctly (but it is present among the `package` artifacts).

## TODO

- Decide on a structure for `lib/`.
    It currently contains plugin runtime utils and SDK polyfills.
    Both (or at least the latter) could be formalized into separate projects.
- Ensure the CMake configuration is resiliant to a name change (or make a clearer way to rename your plugin).
- Clean up `CMakeLists.txt`.
- Windows compatibility (not gonna bother with this in the short term).
- Categorize links.

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
