#!/usr/bin/env bash

if [ "$1" = "clean" ]
then
    rm zia_server
    rm -rf build
    exit
fi

unameOut="$(uname -s)"

if [ "$unameOut" = "Linux" ]; then
    sudo apt install python3-pip
    sudo pip3 install conan


    conan remote add r-Type https://api.bintray.com/conan/bincrafters/public-conan

    mkdir build
    cd build

    conan install .. --build=missing -s compiler.libcxx=libstdc++11
    cmake .. -G "Unix Makefiles"
    cmake --build .
elif [ "$unameOut" = "Darwin" ]; then
    echo "MAC OS"
    brew install python3
    pip3 install conan
    conan remote add r-Type https://api.bintray.com/conan/bincrafters/public-conan

    mkdir build
    cd build
    conan install .. --build=missing
    cmake .. -G "Unix Makefiles"
    make
    cd bin
    cp *zia* ./../../
else
    echo "WINDOWS"

    conan remote add r-Type https://api.bintray.com/conan/bincrafters/public-conan
	
	mkdir build
    cd build

    conan install .. --build=missing
	cmake .. -G "Visual Studio 15 Win64"
    cmake --build . --config "Release"
fi
