Zuniq
======

My  submission to [CodeCup](https://www.codecup.nl/zuniq) 2021. Ranked [15º](https://www.codecup.nl/competition.php?comp=264) among 57 contestants.

![C++17](https://img.shields.io/badge/C++17-00599C?logo=C%2B%2B&logoColor=white&style=flat)
[![CMake](https://github.com/xboard/mcts_zuniq/workflows/CMake/badge.svg)](https://github.com/xboard/mcts_zuniq/actions?query=workflow%3ACMake)
[![License](https://img.shields.io/badge/license-GPL3.0-blue.svg)](https://opensource.org/licenses/GPL-3.0)



## Setting up the build

Dependencies:
* `g++ 7.5+` or `clang++ 3.8+`
* `CMake 3.16+`

The test suite uses [Catch](https://github.com/catchorg/Catch2) v2.13 but it is already included in the `lib` directory.

After cloning this repository (or uncompressing a [release](https://github.com/xboard/mcts_zuniq/release)), go to the source code diretory and write:
```shell
mkdir -p build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release  -DCMAKE_CXX_FLAGS="-DQUIET_MODE=1" ..
make
```

To suggest a specific compiler you can write:

```shell
CXX=clang++ cmake -DCMAKE_BUILD_TYPE=Release  -DCMAKE_CXX_FLAGS="-DQUIET_MODE=1" ..
```

To test your build execute:

```shell
ctest
```

in directory `build/`

## Playing

<p align="center"><img src="/img/zuniq_black.gif" alt="Playing as black"/></p>


Click [here](https://www.codecup.nl/zuniq/rules.php) to learn the rules of Zuniq.

## References

* [An Introduction to Monte Carlo Tree Search](https://github.com/italiancpp/cppday19/blob/master/An%20Introduction%20toMonte%20Carlo%20Tree%20Search%20-%20Manlio%20Morini.pdf)


## Zuniq API and simple app

Boris_de_Wilde's [API](https://dewildeit.nl/swagger-ui/index.html?configUrl=/v3/api-docs/swagger-config) and
[web app](https://dewildeit.nl/zuniq/) to play against this (or other) engine in a web interface.


