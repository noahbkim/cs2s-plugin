# CS2 Server Plugin Template

This repository serves as a template for developing CS2 server plugins.
It uses CMake instead of AMBuild, but in exchange, is much more flexible regarding build steps and dependencies.

## CS2S

This repository targets [`cs2s`](https://github.com/noahbkim/cs2s), a Docker Compose setup I created to develop plugins in a virtual machine.
Currently, I can only guarantee plugins will build in the container.

**Before you do anything, install the dependencies vendored as submodules**.
You will notice each command is prefixed with a full prompt; this is to distinguish between commands run on the `host` and on the Docker `container`.

```shell
# Install the vendored packages (included as submodules). Most of them clone
# shallowly so they don't take up too much space.
user@host cs2s-plugin $ git submodule update --init --recursive
```

Next, optionally, build the Docker environment.
This requires having Docker installed; I've only tested with Docker Desktop.
If you would rather develop directly on your machine, the only thing you need to do for this section is install CS2--ideally with SteamCMD.
**Do not use the Steam installation you play on**, as having third-party software installed on it could get you banned.

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

# You can skip to the next step; this last command is included for reference.

# You can leave the container running in the background without a shell
# attached (and it'll take up minimal resources because it won't be executing
# anything). If you do want to close it (to rebuild the image, save resources,
# etc.), run the following back from your host (not inside the container):
user@host cs2s-plugin $ docker compose down
```

Once you've started the container, you'll want to install CS2.
I've included a script that does this for you via SteamCMD:

```shell
# For the sake of your security, please read through this bash script before
# you run it (it requires your Steam credentials).
user@container /work $ cs2s-install

# If you would prefer to run the commands yourself, I've outlined it below.
# Note the use of /cs2, which is a mounted Docker volume that will persist
# between container runs.
user@container /work $ su steam -c steamcmd  # Must run a steam user
force_install_dir /cs2
login <username>
app_update 730 validate
quit
```

## Setup

Next, you'll need to get your C++ build environment set up.
As prerequisites, install the following:

1. A recent [Python 3](https://www.python.org/downloads/)
2. [AMBuild](https://github.com/alliedmodders/ambuild) from AlliedModders.

I've elected to use CMake rather than AMBuild because I wanted to use [Conan](https://conan.io/), a C++ package manager.
In addition to slimming down the build, Conan makes it super easy to add custom dependencies to your plugin.
The included [`conanfile.txt`](./conanfile.txt) installs `protobuf` and `abseil`.

Non-Docker users will have to create Conan profiles if they haven't already.
**This is already done for you in the Docker image**:

```shell
# Generate a release (default) Conan profile. The profile you choose when
# installing your dependencies controls how they're built.
user@host cs2s-plugin $ conan profile detect              

# For development, you'll likely want to use a debug build. To create a debug
# profile, you'll have to manually edit the detected profile to set 
# `build_type=Debug`. I've included a one-liner, but you may have to do it
# manually depending on your platform.
user@host cs2s-plugin $ conan profile detect --name debug
user@host cs2s-plugin $ sed -i 's/^build_type=Release/build_type=Debug/' $(conan profile path debug) 
```

In the Docker container, this repository directory, `./`, is mounted as `/work`.
Going forward, I will tailor shell commands to Docker users.
Users developing locally (or setting up CMake for their IDE) should interpolate `cs2s-plugin` for `/work`.

```shell
# The best I could come up with for vendoring protobuf 3.21.8 with modified
# compilation options was to include it as a Conan recipe that must be locally
# installed. This is the only way I could get the `protobuf_generate` command
# to be available in our CMake configuration.
user@container /work $ conan create vendor/cs2s-protobuf

# You'll typically only need one profile at a time; debug for development and
# release for the final build. Commands for both are interleaved so you can
# easily compare the two. Note that if you don't specify a --profile, Conan
# automatically uses release. Also note that the Conan output folder can be
# anywhere, though the current directory is convenient because it's mounted to
# your host so you can look through it when debugging.
user@container /work $ conan install . --build missing --output-folder conan-debug --profile debug
user@container /work $ # conan install . --build missing --output-folder conan-release

# Make a build directory. I adhere to CLion's naming convention, but you could
# choose something like `build` if you don't wanna type so much. Just make sure
# it's in the .gitignore.
user@container /work $ mkdir build-debug && cd build-debug
user@container /work $ # mkdir build-release && cd build-release

# Run CMake, generating a Makefile and build config. Note the reference to the
# Conan output and build profile.
user@container /work/cmake-build-debug $ cmake .. -DCMAKE_TOOLCHAIN_FILE=../conan-debug/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=debug
user@container /work/cmake-build-release $ # cmake .. -DCMAKE_TOOLCHAIN_FILE=../conan-release/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=release
```

This configuration has only been tested end-to-end in the Docker container.
It doesn't raise any fatal errors for me on Windows, but it definitely won't build correctly either.
It is, however, sufficient to appease CLion and get (fairly accurate) intellisense on my host/development machine.

## Development

**The development process is still being ironed out**.
Expect this section to change.
For now, users who are not using the Docker Compose setup are expected to extrapolate.

The `cs2s` Docker setup allows us to leverage OverlayFS to create lightweight clones of our (extremely heavy) game installation.
We do this by compositing two layers: the lower, read-only `/cs2` volume which contains our real game installation, and the upper, read-write `tmpfs` mount where our development changes are stored.
The result looks like a complete copy of `/cs2`, but it only needs to store files that have been changed, added, or deleted.
This has all been wrapped up into a convenient script, `cs2s-overlay`:

```shell
# Create a new overlay. This script is installed in /usr/local/bin, so you can
# run it from anywhere. Overlays are installed in /home/steam, so they will be
# erased if you `docker compose down` or otherwise delete the container.
# Fortunately, they're extremely easy to recreate. Note the use of --addons,
# which creates an addons directory and adds a `Game csgo/addons/metamod` to
# the gameinfo.gi.
root@container /work/cmake-build-debug $ cs2s-overlay add main --addons
```

Because `SteamCMD` doesn't like to be run as root, the game installation and overlays are owned by `steam:steam`.
**Make sure files you change are readable/executable by `steam`**--it's best to just `su` whenever you're manually fiddling with them.
The packaged `cs2s-` scripts take care of this already.

```shell
# Build your plugin--this is also how you do it if you're developing locally.
root@container /work/cmake-build-debug $ make -j

# The build assembles all of the artifacts--both from `metamod` and from your
# plugin--into a single `package` directory. You'll want to copy its contents
# (`addons/*` and anything else that ends up in there) into your overlay's game
# directory, e.g. `~steam/main/game/csgo/` as well as update the permissions
# and owners. I've included a script that automates this as well:
root@container /work/cmake-build-debug $ cs2s-overlay install main package

# You can the run the server either manually (you must `su steam` for it to
# work property) or use the overlay script (which `exec`s as `steam`):
root@container /work/cmake-build-debug $ cs2s-overlay start main

# TODO: make this better and also figure out how the `.vdf` should work.
# From there, the usual metamod steps apply. Note that currently, it builds a
# verbose but safe plugin structure, so you have to write (in the console).
# You can create a symlink in `csgo` to type less :)
meta load addons/cs2s-plugin/plugin
```

Note that I haven't actually spent time figuring out how to use the generated
VDF file correctly (but it is present among the `package` artifacts).

## TODO

- Decide on a structure for `lib/`.
    It currently contains plugin runtime utils and SDK polyfills.
    Both (or at least the latter) could be formalized into separate projects.
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
