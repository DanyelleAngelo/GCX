# Grammar Compression modulo X (GCX)

## Introduction

GCX is grammar compressor based on the induced suffix ordering technique introduced in GCIS. Our method builds on the factorization of the DC3 algorithm to create the rules of a straight line grammar for the input string.

GCX supports very fast extraction operation on the encoded grammar without the need to decompress the complete string. 

In this version, the size of the **grammar rules varies at each level*, this value is calculated based on the average LCP of all suffixes (ordered) in the text.

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

## Experiments results

### Compression ratio

|**file**| **compression ratio (%)** | **original file size (M)** |
|----|---------------------|-----------------------|
|fib41|0.000019|256|
|rs.13|0.000094|207|
|tm29|0.000044|256|
|dickens|48.665531|9.7|
|etext99|38.924605|100|
|howto|38.571094|38|
|nci|8.471983|32|
|rctail96|16.746573|109|
|code-sources|27.746838|201|
|sprot34.dat|31.327472|105|
|webster|34.258619|40|
|dna|39.608912|385|
|dna.001.1|2.428843|100|
|dblp.xml.0001.1|0.457076|100|
|dblp.xml.0001.2|0.393795|100|
|dblp.xml.00001.1|0.187582|100|
|dblp.xml.00001.2|0.177124|100|
|english.001.2|2.409162|100|
|proteins.001.1|2.643889|100|
|sources.001.2|2.338541|100|
|cere|1.859898|440|
|coreutils|2.536955|196|
|archive|25.938373|26|
|einstein.de.txt|0.158867|88|
|einstein.en.txt|0.097552|446|
|Escherichia_Coli|9.601072|107|
|influenza|3.26442|148|
|kernel|1.097978|246|
|para|2.737906|409|
|world_leaders|1.786707|0.828|
|xml-dblp.xml|16.387752|100|