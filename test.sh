rm test/test.ir test/test.or test/test.s
./bin/lucc  test/test.c  test/test.ir
./bin/luop  test/test.ir test/test.or
./bin/lugen test/test.or test/test.s
gcc -o test/test test/test.s
./test/test
