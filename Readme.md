# Grammar Compression modulo X (GCX)

## Introduction

GCX is grammar compressor based on the induced suffix ordering technique introduced in GCIS. Our method builds on the factorization of the DC3 algorithm to create the rules of a straight line grammar for the input string.

GCX supports very fast extraction operation on the encoded grammar without the need to decompress the complete string.


This repository contains implementations of two algorithms for Grammar Compression modulo X:
- GC* (gc_/ folder): generates grammar rules with a fixed size X. This value must be provided as input.
- GCX (gcx/ folder): allows variable rule sizes X across grammar levels, based on the average Longest Common Prefix (LCP) of substrings in the text.

## 🛠️ Compilation

###  Generating Executables for Both Versions (GC* and GCX)
To generate the executables for both versions, simply run the following command:
```shell
./build.sh all
```
This command will:
- Compile the GC* project, creating the main_gc_ executable in the current directory.
- Compile the GCX project, creating the main_gcx executable in the current directory.

After successful execution, you will see success messages indicating that both executables have been generated.

###  Compiling the GC*
To compile only the GC* project, run the following command:

```shell
./build.sh gc_star
```
This command will create the main_gc_ executable in the current directory.

###  Compiling the GCX
To compile only the GCX project, run the following command:
```shell
./build.sh gcx
```
This command will create the main_gcx executable in the current directory



## 🚀 Usage Instructions

### Compression
- To compress a text using GC*, execute:
```shell
./gc_star -c <plain_file> <output_file> <coverage>
```
- To compress a text using GCX, execute:
```shell
./gcx  -c <plain_file> <output_file>
```

###   Decompression
- To decompress a text using GC*, execute:
```shell
./gc_star  -d <compressed_file> <output_file> <coverage>
```
- To decompress a text using GCX, execute:
```shell
./gcx -d <compressed_file> <output_file>
```

###   Extraction
- To extract substrings from the compressed text using GC*, execute:
```shell
./gc_star -e <compressed_file> <output_file> <coverage> <queries_file>
```
- To extract substrings from the compressed text using GCX, execute:
```shell
./gcx -e <compressed_file> <output_file> <queries_file>
```

<br>

> Note: The queries_file format is the same for both GC* and GCX extractions. The first line of the queries_file consists of two numbers. The first number represents the number of intervals, and the second number indicates the size of these intervals. The subsequent lines contain two numbers, which define the intervals, separated by a blank space.


## 🧪 Experiments

The results of our experiments are stored in a separate repository: [**gcx-experiments**](https://github.com/DanyelleAngelo/gcx-experiments), where you can find detailed analysis and output data. The repository helps track the performance and comparison of the algorithms under different conditions and datasets.

###  Dataset Links

In the `experiments/file_paths/` folder, we maintain links to some datasets used during the experiments. These datasets are used for testing the compression, decompression, and extraction processes.


---

###  Compression and Decompression Summary


The following table provides a general overview of the **average compression and decompression times**, as well as the **average compression ratio** for different algorithms tested in the experiments using **repetitive datasets**.



| Algorithm   | Compression Time | Decompression Time | Compression Ratio |
|-------------|------------------|--------------------|-------------------|
| **GCX**     | X secs           | Y secs             | Z:1               |
| **GCX\***   | A secs           | B secs             | C:1               |
| **GCIS-ef** | D secs           | E secs             | F:1               |
| **GCIS-s8b**| G secs           | H secs             | I:1               |
| **Repair**  | J secs           | K secs             | L:1               |
---

###  Extraction Summary

The following table provides an overview of the **average extraction times** for different algorithms used in the experiments with **repetitive datasets**.


| Algorithm   | Extraction Time  |
|-------------|------------------|
| **GCX**     | M secs           |
| **GCX\***   | N secs           |
| **GCIS-ef** | O secs           |
| **GCIS-s8b**| P secs           |
| **Repair (PlainSlp_32Fblc)**| Q secs           |
| **Repair (PlainSlp_FblcFblc)**| R secs           |

---

<br>

> **Note:** Detailed experimental results and analyses can be found in the [**Experiments Repository**](https://github.com/DanyelleAngelo/gcx-experiments).

---
