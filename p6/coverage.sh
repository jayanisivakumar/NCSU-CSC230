# Get a fresh build to measure coverage
make clean

# Set coverage-flags to be used inside make.
export CFLAGS="-ftest-coverage -fprofile-arcs"
export LDLIBS="-lgcov"

# Make the driver, using these extra flags.
make driver

# Remove any of the gcda files (should be done by make clean).
rm -f *.gcda

echo "Running test inputs given with the starter"
for i in d01 d02 d03 d04 d05 d06 d07 d08
do
    args=()

    # Extra command-line arguments for some test cases.
    if [ "$i" == "d06" ] || [ "$i" == "d07" ] ; then
	args=(-term)
    elif [ "$i" == "d08" ]; then
	args=(-bad -arguments)
    fi
    
    echo "./driver ${args[@]} < input-$i.txtt > output.txt 2> stderr.txt"
    ./driver ${args[@]} < input-$i.txt > output.txt 2> stderr.txt
done

# To report if we find any student inputs.
STUDENT_INPUT_COUNT=0

# Run the student-generated inputs that are redirected from a file.
list=$(echo my-file-input-*.txt)

if [ "$list" != 'my-file-input-*.txt' ]; then
    echo
    echo "Running with student-created file test inputs"
    for input in my-file-input-*.txt
    do
	echo "./driver < $input > output.txt 2> stderr.txt"
	./driver < $input > output.txt 2> stderr.txt
    done

    STUDENT_INPUT_COUNT=$((STUDENT_INPUT_COUNT + 1))
fi

# Run the student-generated inputs pretend to be typed in from a terminal
# (although they are really from a file).
list=$(echo my-term-input-*.txt)

if [ "$list" != 'my-term-input-*.txt' ]; then
    echo
    echo "Running with student-created terminal test inputs"
    for input in my-term-input-*.txt
    do
	echo "./driver -term < $input > output.txt 2> stderr.txt"
	./driver -term < $input > output.txt 2> stderr.txt
    done

    STUDENT_INPUT_COUNT=$((STUDENT_INPUT_COUNT + 1))
fi

if [ "$STUDENT_INPUT_COUNT" -eq 0 ]; then
    echo "**** No student-created test inputs"
fi

gcov driver segTree input
