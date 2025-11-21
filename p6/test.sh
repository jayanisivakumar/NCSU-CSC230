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
# actual file (AFILE) should not exit either.
checkFileOrMissing() {
  NAME="$1"
  EFILE="$2"
  AFILE="$3"
  
  # if the expected output file doesn't exist, the actual file should't either.
  if [ ! -f "$EFILE" ]; then
      if [ -f "$AFILE" ]; then
	  fail "FAILED - $NAME ($AFILE) should not be created."
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

# The given file, AFILE, should be empty.
checkEmpty() {
  NAME="$1"
  AFILE="$2"
  
  if [ -s "$AFILE" ]; then
      fail "FAILED - $NAME ($AFILE) should be empty."
      return 1
  fi

  return 0
}

# Run a test of the sort program.
testSort() {
  TESTNO=$1
  ESTATUS=$2

  echo "Sort Test $TESTNO"
  rm -f output.txt stderr.txt

  echo "   ./sort ${args[@]} > output.txt 2> stderr.txt"
  ./sort ${args[@]} > output.txt 2> stderr.txt
  ASTATUS=$?

  if ! checkStatus "$ESTATUS" "$ASTATUS" ||
     ! checkFile "Program output" "expected-$TESTNO.txt" "output.txt" ||
     ! checkFileOrEmpty "Stderr output" "error-$TESTNO.txt" "stderr.txt"
  then
      FAIL=1
      return 1
  fi

  echo "Test $TESTNO PASS"
  return 0
}

# Run a test of the driver program.
testDriver() {
  TESTNO=$1
  ESTATUS=$2

  echo "Driver Test $TESTNO"
  rm -f output.txt stderr.txt

  echo "   ./driver ${args[@]} < input-$TESTNO.txt > output.txt 2> stderr.txt"
  ./driver ${args[@]} < input-$TESTNO.txt > output.txt 2> stderr.txt
  ASTATUS=$?

  if ! checkStatus "$ESTATUS" "$ASTATUS" ||
     ! checkFile "Program output" "expected-$TESTNO.txt" "output.txt" ||
     ! checkFileOrEmpty "Stderr output" "error-$TESTNO.txt" "stderr.txt"
  then
      FAIL=1
      return 1
  fi

  echo "Test $TESTNO PASS"
  return 0
}

# get a fresh copy of the target programs
make clean

# Try the unit test program.
make segTreeTest

if [ -x segTreeTest ]
then
    if ./segTreeTest
    then
       echo "Unit tests executed successfully"
    else
	fail "Unit tests didn't execute successfully";
    fi
else
    fail "Unit tests didn't build successfully";
fi


# Run tests for the sort program.
echo "Building sort program"

make sort
if [ $? -ne 0 ]; then
  fail "Make exited unsuccessfully"
fi

# Run tests for the sort program.
if [ -x sort ]; then
    args=(input-s1.txt)
    testSort s1 0

    args=(input-s2.txt)
    testSort s2 0

    args=(input-s3.txt)
    testSort s3 0

    args=(input-s4.txt)
    testSort s4 0

    args=(input-s5.txt)
    testSort s5 0

    args=(input-s6.txt)
    testSort s6 1
    
    args=(missing-file.txt)
    testSort s7 1
    
    args=(-bad-argument input-s8.txt)
    testSort s8 1
else
    fail "Your sort program didn't compile, so it couldn't be tested."
fi

# Run tests for the sort program.
echo "Building driver program"

make driver
if [ $? -ne 0 ]; then
  fail "Make exited unsuccessfully"
fi

# Run tests for the sort program.
if [ -x driver ]; then
    args=()
    testDriver d01 0

    args=()
    testDriver d02 0

    args=()
    testDriver d03 0

    args=()
    testDriver d04 0

    args=()
    testDriver d05 0

    args=(-term)
    testDriver d06 0

    args=(-term)
    testDriver d07 0

    args=(-bad -arguments)
    testDriver d08 1
else
    fail "Your driver program didn't compile, so it couldn't be tested."
fi


if [ $FAIL -ne 0 ]; then
  echo "FAILING TESTS!"
  exit 13
else 
  echo "TESTS SUCCESSFUL"
  exit 0
fi
