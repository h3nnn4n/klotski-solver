#!/bin/bash
set -e

case "$1" in
	build) make build ;;
	test) make pcg && make test ;;
	lint-all)
		"$0" cpplint
		"$0" cppcheck
		"$0" clang-format
		echo "All linters passed!"
		;;
	cpplint)
		cpplint --filter=-build/include_subdir,-readability/nolint,-whitespace/line_length,-whitespace/comments,-readability/casting,-build/header_guard,-runtime/arrays src/*.c
		;;
	cppcheck)
		cppcheck --enable=all --suppressions-list=.cppcheck.suppressions --std=c11 --language=c --error-exitcode=1 src/*.c
		;;
	clang-format)
		./format.sh --check
		;;
	format)
		./format.sh
		;;
	*)
		echo "Usage: $0 {build|test|lint-all|cpplint|cppcheck|clang-format|format}"
		exit 1
		;;
esac
