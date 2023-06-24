#!/bin/bash

DIR_ROOT=$PWD

#
#
#
#
#

FIRST_ARG=$1

case $FIRST_ARG in
as-third-party-library)
  AS_THIRD_PARTY_LIBRARY=yes
  ;;
*)
  AS_THIRD_PARTY_LIBRARY=no
  ;;
esac

#
#
#
#
#

#
#
#
#
#

DIR_THIRDPARTIES=$PWD/thirdparties
DIR_DEPENDENCIES=$DIR_THIRDPARTIES/dependencies

mkdir -p $DIR_DEPENDENCIES

#
#
#
#
#

echo ""
echo "###"
echo "###"
echo "### ensuring the cpp to wasm compiler (emsdk) is installed"
echo "###"
echo "###"
echo ""

EMSDK_VERSION=3.1.26

if [ -z "${EMSDK}" ]; then

  echo " -> not installed"
  echo "   -> installing"

  echo "the env var 'EMSDK' is missing, the web-wasm builds will be skipped"
  echo " => check the readme if you want to install emscripten"
  echo " => it emscripten is laready installed, you may just need to run '. ./emsdk_env.sh' in this terminal"


  sh sh_install_one_git_thirdparty.sh \
    $DIR_DEPENDENCIES \
    "EMSDK" \
    "emsdk" \
    "emscripten-core/emsdk" \
    $EMSDK_VERSION \
    "not-interactive"

  cd $DIR_DEPENDENCIES/emsdk

else

  echo " -> already installed"

  cd $EMSDK
fi

echo " -> ensuring the correct version is installed"

./emsdk install $EMSDK_VERSION

echo " -> activating the correct version"

./emsdk activate --embedded $EMSDK_VERSION

. ./emsdk_env.sh

# em++ --clear-cache

cd $DIR_ROOT

echo " -> success"

#
#
#
#
#

echo ""
echo "###"
echo "###"
echo "### ensuring the thirdparties are installed"
echo "###"
echo "###"
echo ""

case $AS_THIRD_PARTY_LIBRARY in
yes)

  if [ -z "${DIR_LIB_GERONIMO}" ]; then
    echo "the env var 'DIR_LIB_GERONIMO' is missing (see the README.md to install/set it)"
    exit 1
  fi
  echo "the env var 'DIR_LIB_GERONIMO' was found"
  ;;
*)

  echo ""
  echo "###"
  echo "###"
  echo "### building thirdparties libraries"
  echo "###"
  echo "###"
  echo ""

  sh sh_install_one_git_thirdparty.sh \
    $DIR_DEPENDENCIES \
    "GERONIMO" \
    "geronimo" \
    "GuillaumeBouchetEpitech/geronimo" \
    "v0.0.11" \
    "not-interactive"

  echo "building thirdparties libraries"

  cd ./thirdparties/dependencies/geronimo

  sh sh_everything.sh

  export DIR_LIB_GERONIMO=$DIR_ROOT/thirdparties/dependencies/geronimo

  cd $DIR_ROOT

  ;;
esac

#
#
#
#
#

func_ensure_dependency_exist() {

  dependency_name=$1

  echo "check if local dependency is present: $dependency_name"

  if test -f "${dependency_name}"; then
    echo "${dependency_name} was found"
  else
    echo "${dependency_name} was not found, possibly not built"
    exit 1
  fi
}

func_ensure_dependency_exist "${DIR_LIB_GERONIMO}/lib/native/lib-geronimo-system.a"
func_ensure_dependency_exist "${DIR_LIB_GERONIMO}/lib/web-wasm/lib-geronimo-system.bc"

#
#
#
#
#

echo ""
echo "###"
echo "###"
echo "### building main libraries"
echo "###"
echo "###"
echo ""

echo "#"
echo "# native version"
echo "#"

make build_mode="release" build_platform="native" all -j4

echo "#"
echo "# web-wasm version"
echo "#"

make build_mode="release" build_platform="web-wasm" all -j4
