#!/bin/bash

function findAndCopyDLL() 
{
    for i in "${paths[@]}"
    do
        FILE="$i/$1"
        if [ -f $FILE ]; then
           echo -e "\033[1;34mFound DLL $FILE\033[0m"
           cp $FILE build/
           return 0
        fi
    done

    return 1
}

WINDOWS=1
RELEASE=0
TEST=0
OSX=1
VALIDATION=0
VK_SDK="include/vendored/VulkanSDK"

while [[ $# -gt 0 ]]; do
  case $1 in
    -w|--windows)
      WINDOWS=0
      shift # past argument
      ;;
    -v|--validation)
      VALIDATION=1
      shift
      ;;
    --vk)
      VK_SDK=$2
      shift
      shift
      ;;
    -o|--osx)
      OSX=0
      shift
      ;;
    -r|--release)
      RELEASE=1
      shift
      ;;
    -t|--test)
      TEST=1
      shift
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done

for file in build CMakeFiles cmake_install.cmake CMakeCache.txt Makefile Particles
do
  if [ -d $file ];
  then
    rm -rf $file
  fi
  if [ -f $file ];
  then
    rm $file
  fi
done

if [[ $WINDOWS -eq 0 ]];
then 
  cmake -E make_directory build
  export VULKAN_SDK=$VK_SDK/Windows
  export VULKAN_LIBRARY="$VK_SDK/Windows/Lib"
  export VULKAN_INCLUDE_DIR="$VK_SDK/Windows/Include" 
  cmake -E chdir build cmake .. -D WINDOWS=ON -D RELEASE=$RELEASE -D CMAKE_TOOLCHAIN_FILE=./windows.cmake && make -j 8 -C build
  # now copy dlls
  PREFIX="x86_64-w64-mingw32"

  paths=("/usr/local/mingw64/bin"
    "/usr/local/mingw64/bin/x64"
     "/usr/$PREFIX/bin"
    "/usr/$PREFIX/lib"
  )

  for p in /usr/lib/gcc/$PREFIX/*
  do 
    paths+=($p)
  done

  echo -e "\n###############\nChecking Paths: \n"
  for p in "${paths[@]}"
  do
    echo -e "$p\n"
  done 
  echo -e "###############\n"

  dll=("libgcc_s_seh-1.dll"
    "libstdc++-6.dll"
    "libwinpthread-1.dll"
  )

  for j in "${dll[@]}"
  do
    findAndCopyDLL $j || echo "Could not find $j"
  done
elif [[ $OSX -eq 0 ]];
then
  cmake -E make_directory build
  cmake -E chdir build cmake .. -D OSX=ON -D RELEASE=$RELEASE -D VALIDATION=$VALIDATION -D CMAKE_TOOLCHAIN_FILE=./osx.cmake && make -j 8 -C build
else
  cmake -E make_directory build
  cmake -E chdir build cmake -D RELEASE=$RELEASE -D VALIDATION=$VALIDATION .. && make -j 8 -C build
fi