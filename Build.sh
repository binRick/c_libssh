#!/usr/bin/env bash
set -eou pipefail
cd $(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
#./clean.sh

MAKE="./build.sh"
cmd="$MAKE"
if ! eval "$cmd"; then
	exec ./err.sh
	batgrep "error:" .make
	exit 23
	clear
	set +e
	ansi --red "BUILD FAILED"
	[[ -f .err ]] && unlink .err || true
	eval $MAKE
	ec=$?
	cat .err.log
	ansi --red --bg-black --italic "ERROR"
	ansi --red --bold "$(grep -A 5 -B 5 "error: " .err.log)"
	#sleep 666
	exit $ec
fi

cd .build
ninja test

cd ../
./Test.sh
