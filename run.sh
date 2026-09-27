#!/bin/bash

set -e

make clean
make
qemu-system-x86_64 -cdrom wee64.iso
