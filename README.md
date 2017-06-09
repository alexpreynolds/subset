# subset
Draw a subset of lines from an input text file, using either a start index and length or by specifying a file of 0-indexed line numbers.

When specifying a file containing line numbers, the indices are first sorted to allow fast filtering of the input file in one pass.

Output can be written to the standard output stream (`stdout`) or to a regular file via `--output=<fn>`

For debugging, you can use the `--prefix-with-indices` option to write lines with their associated line number.

See the `test/makefile` tests for a quick demonstration of options and usage.