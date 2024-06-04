#!/bin/bash

find ./src/basic-genetic-algorithm -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i
find ./samples/car-demo/src -iname *.h -o -iname *.hpp -o -iname *.cpp | xargs clang-format -i
