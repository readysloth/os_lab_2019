gcc -c -fPIC revert_string.c -o revert_string.o
gcc -shared revert_string.o swap.o -o libRevertD.so
gcc main.c -L$(pwd) -lRevertD -o RevertStringPR_D
gcc tests.c -L$(pwd) -lRevertD -o revertTests -lcunit
./revertTests