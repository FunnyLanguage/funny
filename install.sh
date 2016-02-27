#!/bin/sh
#
# Install FUNNY in Linux
#

mkdir /usr/local/funny
mkdir /usr/local/funny/templates
mkdir /usr/local/funny/bin

cd ./build
rm -rf *
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make
make install
