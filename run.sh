#!/bin/bash

# outdated

set -e

make clean
make
qemu-system-x86_64 -cdrom wee64-0.1.0.iso
