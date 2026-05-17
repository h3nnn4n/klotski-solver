#!/bin/bash

if command -v clang-format-20 &>/dev/null; then
	CLANG_FORMAT=clang-format-20
elif command -v clang-format &>/dev/null; then
	CLANG_FORMAT=clang-format
else
	echo "Error: clang-format not found"
	exit 1
fi

FILES="src/*.c src/*.h src/*.cpp src/*.hpp"

if [ "$1" = "--check" ]; then
	all_good=true
	for file in $FILES; do
		[ -f "$file" ] || continue
		formatted=$($CLANG_FORMAT "$file" --dry-run -Werror 2>&1)
		if [ $? -ne 0 ]; then
			echo "Not formatted: $file"
			all_good=false
		fi
	done
	if [ "$all_good" = true ]; then
		echo "All files formatted."
	else
		exit 1
	fi
else
	for file in $FILES; do
		[ -f "$file" ] && $CLANG_FORMAT -i "$file"
	done
	echo "Formatting complete."
fi
