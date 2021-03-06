#!/bin/bash

# compiler being tested
CC1="src/luxdvr/luxdvr -q $1"
# reference compiler
CC2=gcc

TESTS_PATH=src/tests/execute

fail_counter=0
fail_files=""
pass_counter=0

echo "== Execution tests begin... =="

if [ "$LUX_QUIET" = "1" ] ; then
	echo "Running tests..."
fi

#for file in $TESTS_PATH/*.c ; do
for file in $(find $TESTS_PATH/ | grep '\.c') ; do
	# skip 'other' tests
	if echo $file | grep -q "$TESTS_PATH/other" ; then		
		continue;
	fi

	# avoid llvm benchmarks
	if echo $file | grep -q "llvm"; then
		continue
	fi

	if [ ! "$LUX_QUIET" = "1" ] ; then
		echo $file
	fi

	# out1
	$CC1 $CFLAGS $file -o $TESTS_PATH/test1 &>/dev/null &&
	$TESTS_PATH/test1 >"${file%.*}.output" 2>/dev/null
	rm -f $TESTS_PATH/test1

	# out2
	if [ ! "$LUX_DONT_RECALC" = "1" ] ; then
		$CC2 $file -o $TESTS_PATH/test2 2>/dev/null &&
		$TESTS_PATH/test2 >"${file%.*}.expect" 2>/dev/null
		rm -f $TESTS_PATH/test2
	fi

	# compare
	if ! cmp -s "${file%.*}.output" "${file%.*}.expect" ; then
		echo "failed: $file"
		let fail_counter=fail_counter+1
		fail_files="$fail_files $file"		
	else
		let pass_counter=pass_counter+1
	fi

	# clean
	rm -f "${file%.*}.output"
done

# 'other' tests
for file in $(find $TESTS_PATH/other | grep 'testme\.sh') ; do
	if ! /bin/bash $file $1 ; then
		let fail_counter=fail_counter+1
	else
		let pass_counter=pass_counter+1
	fi
done

echo "== Execution tests results: PASS: $pass_counter, FAIL: $fail_counter =="

if [ "$fail_counter" = "0" ] ; then
	exit 0
else
	exit 1
fi
