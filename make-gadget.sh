#!/bin/sh

cd /home/james/gadget/gadget

echo "Previous executable was ..."
ls -al gadget

echo "Building Gadget ..."
make clean > /dev/null
make --quiet
strip gadget
make clean > /dev/null

echo "New executable is ..."
ls -al gadget

echo "OK, all done"
