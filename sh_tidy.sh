
clang-tidy \
  ./src/basic-genetic-algorithm/*.cpp \
  -extra-arg=-std=c++17 \
  -- \
  -I$DIR_LIB_GERONIMO/src \
  -I$DIR_LIB_GERONIMO/thirdparties \
  -I/usr/include/SDL2/ \
  -DMY_DEFINES \
  ...
