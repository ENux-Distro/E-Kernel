#!/bin/bash

set -e

git clone --depth 1 https://github.com/ENux-Distro/E-Kernel $(pwd)/E-Kernel
cd E-Kernel
make all
echo "Done"