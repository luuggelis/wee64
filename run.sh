#!/usr/bin/env bash

set -Eeuo pipefail

echo "[wee64] cleaning up old build..."
make clean

echo "[wee64] building wee64..."
make

echo "[wee64] build successful!"
echo "[wee64] launching qemu..."

make run