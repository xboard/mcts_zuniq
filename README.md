Zuniq
======

My submission to [CodeCup](https://www.codecup.nl/zuniq) 2021.

[![CMake](https://github.com/xboard/zuniq/workflows/CMake/badge.svg)](https://github.com/xboard/zuniq/actions?query=workflow%3ACMake)
[![Build Status](https://travis-ci.com/xboard/zuniq.svg?token=kzFi7uxzUNyuZFTEpPEx&branch=master)](https://travis-ci.com/xboard/zuniq)
[![License](https://img.shields.io/badge/license-GPL3.0-blue.svg)](https://opensource.org/licenses/GPL-3.0)
## Setting up the build

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

## References

* [An Introduction to Monte Carlo Tree Search](https://github.com/italiancpp/cppday19/blob/master/An%20Introduction%20toMonte%20Carlo%20Tree%20Search%20-%20Manlio%20Morini.pdf)

* [Pocket MCTS @ Github](https://github.com/morinim/pocket_mcts)

* [MCTS Lynx3](https://github.com/lwevers/lynx/blob/master/Lynx3.java)

* [MCTS for Poly-Y](https://research.utwente.nl/files/5338881/polyy.pdf)

* [All-Moves-As-First Heuristics in Monte Carlo Go](https://users.soe.ucsc.edu/~dph/mypubs/AMAFpaperWithRef.pdf)

* [MCTS in Hex](http://webdocs.cs.ualberta.ca/~hayward/papers/mcts-hex.pdf)

* [Bitboard Design for Connect 4](https://github.com/denkspuren/BitboardC4/blob/master/BitboardDesign.md)

## Zuniq API and simple app

Boris_de_Wilde's [API](https://dewildeit.nl/swagger-ui/index.html?configUrl=/v3/api-docs/swagger-config) and
[web app](https://dewildeit.nl/zuniq/) to play against your own bot.


