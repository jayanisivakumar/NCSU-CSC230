#!/bin/bash
# This is a shell script to check your program(s) on test cases.
# It usese the same language you normally use to type commands in
# a terminal window.  You can write whole programs in shell.

# Assume we've succeeded until we see otherwise.
FAIL=0

# Print an error message and set the fail flag.
fail() {
    echo "**** $1"
    FAIL=1
}

# Check the exit status.  The actual exit status (ASTATUS) should match
# the expected status (ESTATUS)
checkStatus() {
  ESTATUS="$1"
  ASTATUS="$2"

  # Make sure the program exited with the right exit status.
  if [ "$ASTATUS" -ne "$ESTATUS" ]; then
      fail "FAILED - incorrect program exit status. (expected $ESTATUS,  Got: $ASTATUS)"
      return 1
  fi

  return 0
}

# Check the contents of an a file.  If the expected file (EFILE)
# exists, then the actual file (AFILE) should exist and it should match.
checkFile() {
  NAME="$1"
  EFILE="$2"
  AFILE="$3"

  # Make sure we're really expecting this file.
  if [ ! -f "$EFILE" ]; then
      return 0
  fi

  # Make sure the output matches the expected output.
  echo "   diff $EFILE $AFILE"
  diff -q "$EFILE" "$AFILE" >/dev/null 2>&1
  if [ $? -ne 0 ]; then
      fail "FAILED - $NAME ($AFILE) doesn't match expected ($EFILE)"
      return 1
  fi

  return 0
}

# Same as checkFile, but if the expected file (EFILE) doesn't exist, the
# actual file (AFILE) should be empty.
checkFileOrEmpty() {
  NAME="$1"
  EFILE="$2"
  AFILE="$3"
  
  # if the expected output file doesn't exist, the actual file should be
  # empty.
  if [ ! -f "$EFILE" ]; then
      if [ -s "$AFILE" ]; then
	  fail "FAILED - $NAME ($AFILE) should be empty."
	  return 1
      fi
      return 0
  fi

  # Make sure the output matches the expected output.
  echo "   diff $EFILE $AFILE"
  diff -q "$EFILE" "$AFILE" >/dev/null 2>&1
  if [ $? -ne 0 ]; then
      fail "FAILED - $NAME ($AFILE) doesn't match expected ($EFILE)"
      return 1
  fi

  return 0
}

# Same as checkFile, but if the expected file (EFILE) doesn't exist, the
# actual file (AFILE) should not exist.
checkFileOrDNE() {
  NAME="$1"
  EFILE="$2"
  AFILE="$3"
  
  # if the expected output file doesn't exist, the actual file should not exist
  if [ ! -f "$EFILE" ]; then
      if [ -f "$AFILE" ]; then
	  fail "FAILED - $NAME ($AFILE) should not exist."
	  return 1
      fi
      return 0
  fi

  # Make sure the output matches the expected output.
  echo "   diff $EFILE $AFILE"
  diff -q "$EFILE" "$AFILE" >/dev/null 2>&1
  if [ $? -ne 0 ]; then
      fail "FAILED - $NAME ($AFILE) doesn't match expected ($EFILE)"
      return 1
  fi

  return 0
}

# The given file should exist but should be empty.
checkEmpty() {
  NAME="$1"
  AFILE="$2"
  
  if [ -s "$AFILE" ]; then
      fail "FAILED - $NAME ($AFILE) should be empty."
      return 1
  fi

  return 0
}

# Run a test case on the program.
runTest() {
    TESTNO="$1"
    EOUTPUT="$2"
    ESTATUS="$3"

    rm -f output.bin
    
    echo "Test $TESTNO"
    echo "   ./tcrypt ${args[@]} > stdout.txt 2> stderr.txt"
    ./tcrypt ${args[@]} > stdout.txt 2> stderr.txt
    ASTATUS=$?

    if ! checkStatus "$ESTATUS" "$ASTATUS" ||
	    ! checkEmpty "Terminal output" "stdout.txt" ||
	    ! checkFileOrDNE "Output file" "$EOUTPUT" "output.bin" ||
	    ! checkFileOrEmpty "Stderr output" "error-$TESTNO.txt" "stderr.txt"
    then
	FAIL=1
	return 1
    fi

    echo "Test $TESTNO PASS"
    return 0
}

# Try the unit tests
make clean
make TDEStest

if [ -x TDEStest ]
then
    if ./TDEStest
    then
       echo "Unit tests executed successfully"
    else
	fail "Unit tests didn't execute successfully";
    fi
else
    fail "Unit tests didn't build successfully";
fi

# make a fresh copy of the target program
make

if [ $? -ne 0 ]; then
  fail "Make exited unsuccessfully"
fi

# Run test cases
if [ -x tcrypt ]; then
    # Encrypt tests
    args=(key-a.txt plain-a.txt output.bin)
    runTest 01 cipher-a.bin 0
    
    args=(key-b.txt plain-b.txt output.bin)
    runTest 02 cipher-b.bin 0

    args=(key-c.txt plain-c.txt output.bin)
    runTest 03 cipher-c.bin 0

    args=(key-d.txt plain-d.txt output.bin)
    runTest 04 cipher-d.bin 0

    args=(key-e.bin plain-e.bin output.bin)
    runTest 05 cipher-e.bin 0

    args=(key-f.bin plain-f.bin output.bin)
    runTest 06 cipher-f.bin 0

    args=(key-g.bin plain-g.bin output.bin)
    runTest 07 cipher-g.bin 0

    args=(key-h.bin plain-h.bin output.bin)
    runTest 08 no-expected-output-file 1

    args=(key-i.bin missing-file.bin output.bin)
    runTest 09 no-expected-output-file 1

    args=(-x key-j.bin plain-j.bin output.bin)
    runTest 10 no-expected-output-file 1

    # Decrypt tests
    args=(-d key-a.txt cipher-a.bin output.bin)
    runTest 11 plain-a.txt 0

    args=(-d key-b.txt cipher-b.bin output.bin)
    runTest 12 plain-b.txt 0
    
    args=(-d key-c.txt cipher-c.bin output.bin)
    runTest 13 plain-c.txt 0
    
    args=(-d key-d.txt cipher-d.bin output.bin)
    runTest 14 plain-d.txt 0
    
    args=(-d key-e.bin cipher-e.bin output.bin)
    runTest 15 plain-e.bin 0
    
    args=(-d key-f.bin cipher-f.bin output.bin)
    runTest 16 plain-f.bin 0
    
    args=(-d key-g.bin cipher-g.bin output.bin)
    runTest 17 plain-g.bin 0
    
    args=(-d key-k.bin cipher-k.bin output.bin)
    runTest 18 no-expected-output-file 1
else
    fail "Since your program didn't compile, we couldn't test it"
fi

if [ $FAIL -ne 0 ]; then
  echo "FAILING TESTS!"
  exit 13
else 
  echo "TESTS SUCCESSFUL"
  exit 0
fi
