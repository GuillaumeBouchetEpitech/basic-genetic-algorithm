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

echo ""
echo "#################################################"
echo "#                                               #"
echo "# IF THIS SCRIPT FAIL -> TRY THOSE TWO COMMANDS #"
echo "# -> 'chmod u+x ./sh_everything.sh'              #"
echo "# -> './sh_everything.sh'                       #"
echo "#                                               #"
echo "#################################################"
echo ""

#
#
#
#
#

if [ -z "${EMSDK}" ]; then
  echo "the env var 'EMSDK' is missing, the web-wasm builds will be skipped"
  echo " => check the readme if you want to install emscripten"
  echo " => it emscripten is laready installed, you may just need to run '. ./emsdk_env.sh' in this terminal"
  WEB_WASM_AVAILABLE=no
else
  echo "the env var 'EMSDK' was found, the web-wasm builds will be included"
  WEB_WASM_AVAILABLE=yes
fi

#
#
#
#
#

case $AS_THIRD_PARTY_LIBRARY in
yes)

  if [ -z "${DIR_LIB_GERONIMO}" ]; then
    echo "the env var 'DIR_LIB_GERONIMO' is missing (see the README.md to install/set it)"
    exit 1
  fi
  echo "the env var 'DIR_LIB_GERONIMO' was found"
  ;;
*)

  echo "ensuring the thirdparties are installed"

  chmod u+x ./sh_install_thirdparties.sh
  ./sh_install_thirdparties.sh not-interactive

  echo "building thirdparties libraries"
  echo "  native version"

  cd ./thirdparties/dependencies/geronimo

  ./sh_everything.sh

  export DIR_LIB_GERONIMO=$DIR_ROOT/thirdparties/dependencies/geronimo

  cd $DIR_ROOT

  ;;
esac

#
#
#
#
#

func_ensure_dependencies() {

  ALL_DEPENDENCY_NAMES=${*}

  for dependency_name in $ALL_DEPENDENCY_NAMES;
  do

    echo "check if local dependency is present: $dependency_name"

    if test -f "${dependency_name}"; then
      echo "${dependency_name} was found"
    else
      echo "${dependency_name} was not found, possibly not built"
      exit 1
    fi

  done
}

declare -a all_dependency_names=(
  "${DIR_LIB_GERONIMO}/lib/native/lib-geronimo-system.a"
)

func_ensure_dependencies ${all_dependency_names[*]}

case $WEB_WASM_AVAILABLE in
yes)

  declare -a all_dependency_names=(
    "${DIR_LIB_GERONIMO}/lib/web-wasm/lib-geronimo-system.bc"
  )

  func_ensure_dependencies ${all_dependency_names[*]}

  ;;
esac

#
#
#
#
#

echo "building projects applicaton"
echo "  native version"
make build_mode="release" build_platform="native" all -j6

case $WEB_WASM_AVAILABLE in
yes)
  echo "  web-wasm version"
  make build_mode="release" build_platform="web-wasm" all -j6
  ;;
esac
