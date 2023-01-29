
# Basic Genetic Algorithm

## Table of Contents
- [Basic Genetic Algorithm](#basic-genetic-algorithm)
  - [Table of Contents](#table-of-contents)
  - [Diagrams](#diagrams)
    - [Main Logic](#main-logic)
- [Dependencies](#dependencies)
  - [Dependency: Emscripten 3.1.26 (for web-wasm build)](#dependency-emscripten-3126-for-web-wasm-build)
  - [Dependency: Geronimo 0.0.4](#dependency-geronimo-004)
- [How to Build](#how-to-build)
  - [Build Everything (will skip web-wasm if emscripten is absent)](#build-everything-will-skip-web-wasm-if-emscripten-is-absent)
- [Thanks for watching!](#thanks-for-watching)

## Diagrams


### Main Logic

```mermaid

%%{
  init: {
    'theme': 'base',
    'themeVariables': {
      'primaryColor': '#242424',
      'primaryTextColor': '#DDD',
      'primaryBorderColor': '#000',
      'lineColor': '#A0A0A0',
      'secondaryColor': '#454545',
      'tertiaryColor': '#353535'
    }
  }
}%%

  flowchart TD

    subgraph simulation [Run The Simulation]

      run1[set the genomes]
      run2[try all the genomes]
      run3[rate all the genomes]

    end

    subgraph evolution [Evolve The Simulation]

      evo1[Natural Selection]
      evo2[Elitism -> 10%]
      evo3[Reproduction and Mutation -> 80%]
      evo4[Diversity -> anything left]

    end

    Start --> simulation
    simulation --> evolution
    evolution --> Stop

    run1 --> run2 --> run3

    evo1 -- Select the best genomes --> evo2
    evo2 -- cross breed best genomes\nmutate the newly bred genomes --> evo3
    evo3 -- fill the generation\nwith random genomes --> evo4


```

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

## Dependency: Geronimo 0.0.4

This dependency will be donwloaded and built with the `Build Everything` method below

# How to Build

## Build Everything (will skip web-wasm if emscripten is absent)

```bash
chmod u+x ./sh_everything.sh
./sh_everything.sh
# will tell if a dependency is missing
# will skip the web-wasm build if emscripten is not detected
```

# Thanks for watching!
