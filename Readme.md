# Grammar Compression modulo X (GCX)

## Introduction

GCX is grammar compressor based on the induced suffix ordering technique introduced in GCIS. Our method builds on the factorization of the DC3 algorithm to create the rules of a straight line grammar for the input string.

GCX supports very fast extraction operation on the encoded grammar without the need to decompress the complete string. 


In this version, the size of the **grammar rules varies at each level*, this value is calculated based on the average LCP of all suffixes (ordered) in the text. 

***You can use this version of the algorithm if you want to choose the first LCP calculation window.***

## Compilation
```shell
TBD
```

## Compression
To compress a text, execute:

```shell
./main <plain_file> <compressed_file> c <cover>
```

## Decompression
To decompress a text, execute:

```shell
./main <compressed_file> <decompressed_file> d <cover>
```

## Extraction
To extract substrings from the compressed text, execute:

```shell
./main <compressed_file> <output_file> e <cover> <queries_file>
```