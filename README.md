
# Basic Genetic Algorithm

## Table of Contents
- [Basic Genetic Algorithm](#basic-genetic-algorithm)
  - [Table of Contents](#table-of-contents)
- [Dependencies](#dependencies)
  - [Dependency: Emscripten 3.1.26 (for web-wasm build)](#dependency-emscripten-3126-for-web-wasm-build)
  - [Dependency: Geronimo 0.0.1](#dependency-geronimo-001)
- [How to Build](#how-to-build)
  - [Build Everything (will skip web-wasm if emscripten is absent)](#build-everything-will-skip-web-wasm-if-emscripten-is-absent)
- [Thanks for watching!](#thanks-for-watching)

# Dependencies

## Dependency: Emscripten 3.1.26 (for web-wasm build)
```bash
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk

./emsdk install 3.1.26
./emsdk activate --embedded 3.1.26

. ./emsdk_env.sh

em++ --clear-cache
```

## Dependency: Geronimo 0.0.1

```bash
cd <some-other-folder>
git clone --depth 1 --branch 0.0.1 https://github.com/GuillaumeBouchetEpitech/geronimo.git
# then just follow the README.md explanations of the repository
```

**`/!\ important /!\`**

Then, to make it available
```bash
export DIR_LIB_GERONIMO=<some-other-folder>
```
**`/!\ important /!\`**


# How to Build

## Build Everything (will skip web-wasm if emscripten is absent)

```bash
chmod u+x ./sh_everything.sh
./sh_everything.sh
# will tell if a dependency is missing
# will skip the web-wasm build if emscripten is not detected
```

# Thanks for watching!
