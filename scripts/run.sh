#!/bin/bash

set -ex

(cd builddir && ninja)

echo "Running: $1"

builddir/webglruntime $1