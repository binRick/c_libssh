#!/bin/bash
set -eou pipefail
reset

BD="$(pwd)/.build"
[[ -d .build ]] && rm -rf .build
if [[ -d "$BD" ]]; then
	meson build -C $BD
	#meson --reconfigure --wipe $BD
else
	meson $BD
fi
cd $BD
ninja

clear

meson test -v

./test1
