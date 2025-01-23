# Grammar Compression modulo X (GCX)

## Introduction

GCX is grammar compressor based on the induced suffix ordering technique introduced in GCIS. Our method builds on the factorization of the DC3 algorithm to create the rules of a straight line grammar for the input string.

GCX supports very fast extraction operation on the encoded grammar without the need to decompress the complete string.

In this version, the size of the \*_grammar rules varies at each level_, this value is calculated based on the average LCP of all suffixes (ordered) in the text.

## Compilation

```shell
TBD
```

## Compression

To compress a text, execute:

```shell
./main -c <plain_file> <compressed_file>
```

## Decompression

To decompress a text, execute:

```shell
./main -d <compressed_file> <decompressed_file>
```

## Extraction

To extract substrings from the compressed text, execute:

```shell
./main -e <compressed_file> <output_file> <queries_file>
```

The first line of the `queries_file` consists of two numbers. The first number represents the number of intervals, and the second number indicates the size of these intervals. The subsequent lines contain two numbers, which define the intervals, separated by a blank space.

## Experiments

The files_paths folder, located in the experiments folder, contains some sample files used in this project for testing.
