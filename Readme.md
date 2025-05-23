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
- Compile the GC* project, creating the gc_star executable in the current directory.
- Compile the GCX project, creating the gcx executable in the current directory.

After successful execution, you will see success messages indicating that both executables have been generated.

###  Compiling the GC*
To compile only the GC* project, run the following command:

```shell
./build.sh gc_star
```
This command will create the gc_star executable in the current directory.

###  Compiling the GCX
To compile only the GCX project, run the following command:
```shell
./build.sh gcx
```
This command will create the gcx executable in the current directory



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

> Note: The queries_file format is the same for both GC* and GCX extractions. The each lines contain two numbers, which define the intervals, separated by a blank space.


## 🧪 Experiments

The results of our experiments are stored in a separate repository: [**gcx-experiments**](https://github.com/DanyelleAngelo/gcx-experiments), where you can find detailed analysis and output data. The repository helps track the performance and comparison of the algorithms under different conditions and datasets.

###  Dataset Links

In the `experiments/file_paths/` folder, we maintain links to some datasets used during the experiments. These datasets are used for testing the compression, decompression, and extraction processes.


---

###  Compression and Decompression Summary


The following table provides a general overview of the **compression and decompression times**, as well as the **average compression ratio** for different algorithms tested in the experiments using **repetitive datasets**.



#### Compression Time (s) 
| File | Input Size (MB) | bzip2 | RePair (32Fblc) | RePair (FblcFblc) | GCIS-ef | GCIS-s8b | GC4 | GC8 | GC16 | GC32 | GCX-y4 | GCX-y8 | GCX-y16 | GCX-y32 |
|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Escherichia_Coli | 107.47 | 4.74 | 19.39 | 19.39 | 5.39 | 5.13 | 2.85 | 2.64 | 2.81 | 2.57 | 5.23 | 4.98 | 5.69 | 5.55 |
| artificial-fib41 | 255.50 | 33.77 | 13.81 | 13.81 | 3.84 | 3.87 | 2.60 | 3.73 | 4.37 | 4.57 | 5.16 | 7.46 | 8.65 | 8.85 |
| artificial-rs.13 | 206.71 | 27.80 | 9.89 | 9.89 | 2.86 | 2.90 | 2.95 | 2.98 | 3.49 | 3.82 | 5.33 | 6.28 | 7.17 | 7.24 |
| artificial-tm29 | 256.00 | 49.87 | 12.95 | 12.95 | 3.42 | 3.46 | 1.74 | 1.69 | 2.04 | 4.27 | 3.98 | 3.54 | 4.10 | 8.48 |
| cere | 439.92 | 18.13 | 70.30 | 70.30 | 21.59 | 21.17 | 11.52 | 10.70 | 11.31 | 10.99 | 25.41 | 22.23 | 22.12 | 22.44 |
| coreutils | 195.77 | 8.30 | 45.60 | 45.60 | 8.83 | 8.69 | 5.80 | 5.09 | 4.96 | 4.78 | 13.15 | 10.19 | 9.82 | 9.53 |
| dblp.xml.00001.1 | 100.00 | 4.93 | 20.25 | 20.25 | 4.02 | 3.83 | 2.55 | 2.35 | 2.19 | 2.15 | 5.97 | 4.46 | 4.56 | 4.40 |
| dblp.xml.00001.2 | 100.00 | 4.92 | 18.82 | 18.82 | 4.09 | 3.90 | 2.50 | 2.23 | 2.28 | 2.24 | 5.89 | 4.61 | 4.59 | 4.49 |
| dblp.xml.0001.1 | 100.00 | 4.88 | 19.52 | 19.52 | 4.05 | 3.97 | 2.52 | 2.18 | 2.22 | 2.22 | 5.74 | 4.58 | 4.59 | 4.57 |
| dblp.xml.0001.2 | 100.00 | 4.83 | 18.31 | 18.31 | 4.12 | 3.96 | 2.52 | 2.21 | 2.24 | 2.18 | 5.89 | 4.55 | 4.57 | 4.57 |
| dna.001.1 | 100.00 | 4.31 | 28.77 | 28.77 | 4.33 | 4.21 | 2.14 | 2.26 | 2.32 | 2.38 | 4.99 | 4.66 | 4.81 | 4.81 |
| einstein.de.txt | 88.46 | 4.35 | 17.27 | 17.27 | 3.56 | 3.50 | 2.31 | 2.13 | 2.02 | 2.00 | 5.15 | 4.18 | 4.03 | 3.97 |
| einstein.en.txt | 445.96 | 22.73 | 94.05 | 94.05 | 22.78 | 22.49 | 12.50 | 11.35 | 10.95 | 10.76 | 28.90 | 22.52 | 21.31 | 20.71 |
| english.001.2 | 100.00 | 4.24 | 22.09 | 22.09 | 4.86 | 4.76 | 2.62 | 2.31 | 2.48 | 2.35 | 6.11 | 5.04 | 5.03 | 5.03 |
| influenza | 147.64 | 8.54 | 23.23 | 23.23 | 5.85 | 5.80 | 3.34 | 3.48 | 3.53 | 3.46 | 7.70 | 6.92 | 6.94 | 6.92 |
| kernel | 246.01 | 9.64 | 51.02 | 51.02 | 12.09 | 11.92 | 7.32 | 6.57 | 6.38 | 6.17 | 17.02 | 13.23 | 12.52 | 12.30 |
| para | 409.38 | 17.11 | 69.96 | 69.96 | 20.70 | 20.14 | 11.32 | 10.31 | 10.86 | 10.46 | 24.52 | 21.55 | 21.38 | 21.34 |
| proteins.001.1 | 100.00 | 4.62 | 24.56 | 24.56 | 5.05 | 4.81 | 2.65 | 2.26 | 2.47 | 2.31 | 6.13 | 4.72 | 4.87 | 4.53 |
| sources.001.2 | 100.00 | 3.99 | 20.63 | 20.63 | 4.33 | 4.11 | 2.50 | 2.28 | 2.24 | 2.39 | 5.95 | 4.92 | 4.88 | 4.92 |
| world_leaders | 44.79 | 1.18 | 6.14 | 6.14 | 1.21 | 1.18 | 0.78 | 0.74 | 0.81 | 0.88 | 1.67 | 1.43 | 1.53 | 1.72 |


<br>
<br>


#### Decompression Time (s) 
| File | Input Size (MB) | bzip2 | RePair (32Fblc) | RePair (FblcFblc) | GCIS-ef | GCIS-s8b | GC4 | GC8 | GC16 | GC32 | GCX-y4 | GCX-y8 | GCX-y16 | GCX-y32 |
|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Escherichia_Coli | 107.47 | 2.19 | 0.86 | 0.86 | 0.82 | 0.82 | 0.39 | 0.21 | 0.44 | 0.38 | 0.43 | 0.21 | 0.46 | 0.44 |
| artificial-fib41 | 255.50 | 3.30 | 1.01 | 1.01 | 0.84 | 0.92 | 0.56 | 0.31 | 0.23 | 0.18 | 0.96 | 0.36 | 0.24 | 0.18 |
| artificial-rs.13 | 206.71 | 2.61 | 0.82 | 0.82 | 0.66 | 0.72 | 0.43 | 0.26 | 0.19 | 0.15 | 0.76 | 0.29 | 0.19 | 0.15 |
| artificial-tm29 | 256.00 | 3.29 | 1.04 | 1.04 | 0.78 | 0.85 | 0.55 | 0.32 | 0.23 | 0.18 | 0.52 | 0.32 | 0.23 | 0.18 |
| cere | 439.92 | 8.53 | 3.42 | 3.42 | 2.44 | 3.00 | 2.33 | 1.21 | 1.93 | 1.70 | 2.55 | 1.65 | 2.12 | 1.74 |
| coreutils | 195.77 | 2.61 | 1.41 | 1.41 | 1.07 | 1.38 | 0.80 | 0.56 | 0.56 | 0.56 | 1.13 | 0.58 | 0.58 | 0.54 |
| dblp.xml.00001.1 | 100.00 | 1.33 | 0.59 | 0.59 | 0.37 | 0.55 | 0.22 | 0.14 | 0.10 | 0.08 | 0.43 | 0.18 | 0.15 | 0.12 |
| dblp.xml.00001.2 | 100.00 | 1.35 | 0.58 | 0.58 | 0.38 | 0.54 | 0.22 | 0.14 | 0.10 | 0.08 | 0.44 | 0.18 | 0.15 | 0.11 |
| dblp.xml.0001.1 | 100.00 | 1.33 | 0.60 | 0.60 | 0.39 | 0.52 | 0.26 | 0.14 | 0.10 | 0.09 | 0.43 | 0.18 | 0.15 | 0.13 |
| dblp.xml.0001.2 | 100.00 | 1.44 | 0.61 | 0.61 | 0.40 | 0.53 | 0.25 | 0.14 | 0.10 | 0.08 | 0.43 | 0.18 | 0.15 | 0.14 |
| dna.001.1 | 100.00 | 2.05 | 0.79 | 0.79 | 0.54 | 0.65 | 0.25 | 0.14 | 0.12 | 0.10 | 0.43 | 0.19 | 0.16 | 0.23 |
| einstein.de.txt | 88.46 | 1.14 | 0.57 | 0.57 | 0.30 | 0.56 | 0.23 | 0.14 | 0.10 | 0.09 | 0.41 | 0.15 | 0.11 | 0.10 |
| einstein.en.txt | 445.96 | 5.71 | 3.10 | 3.10 | 1.44 | 2.71 | 1.20 | 0.72 | 0.53 | 0.47 | 1.99 | 0.78 | 0.54 | 0.45 |
| english.001.2 | 100.00 | 2.26 | 0.90 | 0.90 | 0.60 | 0.78 | 0.27 | 0.16 | 0.12 | 0.10 | 0.50 | 0.26 | 0.25 | 0.29 |
| influenza | 147.64 | 1.95 | 0.86 | 0.86 | 0.64 | 0.87 | 0.40 | 0.27 | 0.21 | 0.25 | 0.69 | 0.26 | 0.22 | 0.27 |
| kernel | 246.01 | 3.66 | 1.85 | 1.85 | 1.31 | 1.74 | 1.15 | 0.68 | 0.89 | 0.88 | 1.42 | 0.84 | 0.88 | 0.84 |
| para | 409.38 | 8.19 | 3.23 | 3.23 | 2.43 | 2.90 | 2.27 | 1.13 | 1.86 | 1.63 | 2.53 | 1.67 | 1.89 | 1.73 |
| proteins.001.1 | 100.00 | 2.15 | 0.77 | 0.77 | 0.55 | 0.80 | 0.26 | 0.16 | 0.12 | 0.10 | 0.48 | 0.19 | 0.14 | 0.16 |
| sources.001.2 | 100.00 | 1.81 | 0.84 | 0.84 | 0.57 | 0.71 | 0.27 | 0.16 | 0.11 | 0.10 | 0.50 | 0.26 | 0.19 | 0.29 |
| world_leaders | 44.79 | 0.39 | 0.27 | 0.27 | 0.17 | 0.18 | 0.12 | 0.07 | 0.05 | 0.06 | 0.19 | 0.09 | 0.06 | 0.06 |

<br>
<br>


#### Compression ratio (\%)
| File | Input Size (MB) | bzip2 | RePair (32Fblc) | RePair (FblcFblc) | GCIS-ef | GCIS-s8b | GC4 | GC8 | GC16 | GC32 | GCX-y4 | GCX-y8 | GCX-y16 | GCX-y32 |
|:---|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| Escherichia_Coli | 107.47 | 26.98 | 56.44 | 48.43 | 16.68 | 13.16 | 50.06 | 31.96 | 91.03 | 95.15 | 39.59 | 32.49 | 78.94 | 85.64 |
| artificial-fib41 | 255.50 | 0.01 | 0.00 | 0.00 | 0.00 | 0.00 | 0.06 | 0.07 | 0.08 | 0.16 | 0.05 | 0.05 | 0.08 | 0.06 |
| artificial-rs.13 | 206.71 | 0.01 | 0.00 | 0.00 | 0.00 | 0.00 | 0.09 | 0.10 | 0.13 | 0.20 | 0.08 | 0.09 | 0.11 | 0.09 |
| artificial-tm29 | 256.00 | 0.01 | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 | 0.00 |
| cere | 439.92 | 25.22 | 13.12 | 11.37 | 4.44 | 3.40 | 33.67 | 26.33 | 57.56 | 68.88 | 37.43 | 31.56 | 54.27 | 60.36 |
| coreutils | 195.77 | 16.02 | 18.46 | 15.82 | 5.93 | 4.71 | 43.38 | 43.15 | 53.87 | 64.66 | 44.20 | 42.79 | 50.21 | 57.28 |
| dblp.xml.00001.1 | 100.00 | 10.97 | 1.64 | 1.32 | 0.47 | 0.34 | 0.59 | 0.66 | 0.75 | 0.94 | 1.66 | 17.42 | 12.31 | 26.68 |
| dblp.xml.00001.2 | 100.00 | 11.24 | 1.68 | 1.35 | 0.47 | 0.34 | 0.59 | 0.66 | 0.75 | 0.94 | 1.76 | 17.72 | 12.60 | 27.35 |
| dblp.xml.0001.1 | 100.00 | 11.03 | 2.81 | 2.25 | 0.90 | 0.66 | 1.01 | 1.16 | 1.44 | 1.94 | 2.08 | 17.58 | 12.53 | 26.96 |
| dblp.xml.0001.2 | 100.00 | 13.15 | 3.19 | 2.60 | 0.90 | 0.66 | 1.01 | 1.16 | 1.44 | 1.94 | 2.85 | 20.53 | 15.36 | 33.37 |
| dna.001.1 | 100.00 | 26.12 | 12.23 | 10.04 | 4.24 | 3.17 | 3.49 | 4.20 | 6.87 | 9.06 | 5.21 | 19.26 | 34.94 | 42.74 |
| einstein.de.txt | 88.46 | 4.32 | 1.25 | 0.98 | 0.34 | 0.25 | 7.69 | 9.41 | 13.11 | 14.41 | 7.96 | 8.59 | 11.22 | 13.23 |
| einstein.en.txt | 445.96 | 5.17 | 0.63 | 0.51 | 0.22 | 0.17 | 3.74 | 4.69 | 6.67 | 8.74 | 3.82 | 4.31 | 5.95 | 7.14 |
| english.001.2 | 100.00 | 37.32 | 16.37 | 13.73 | 4.87 | 3.66 | 4.48 | 5.52 | 6.92 | 9.12 | 12.12 | 27.00 | 50.79 | 82.67 |
| influenza | 147.64 | 6.59 | 16.99 | 14.27 | 5.66 | 4.31 | 16.49 | 18.80 | 24.61 | 33.73 | 16.84 | 17.36 | 23.15 | 31.35 |
| kernel | 246.01 | 21.74 | 8.41 | 7.21 | 2.69 | 2.12 | 38.90 | 39.51 | 48.41 | 66.19 | 40.91 | 36.55 | 44.34 | 53.91 |
| para | 409.38 | 26.15 | 18.32 | 16.10 | 5.73 | 4.52 | 37.50 | 28.42 | 65.02 | 77.31 | 42.01 | 35.72 | 55.95 | 67.27 |
| proteins.001.1 | 100.00 | 40.59 | 13.63 | 11.20 | 4.87 | 3.67 | 4.54 | 5.69 | 6.88 | 9.09 | 7.00 | 15.32 | 27.37 | 30.80 |
| sources.001.2 | 100.00 | 33.02 | 15.78 | 13.23 | 4.72 | 3.59 | 4.54 | 5.37 | 6.82 | 9.09 | 11.87 | 25.26 | 45.64 | 79.33 |
| world_leaders | 44.79 | 6.94 | 11.53 | 9.45 | 3.85 | 2.85 | 21.75 | 25.72 | 30.56 | 37.63 | 22.80 | 24.09 | 26.89 | 33.45 |

<br>
<br>


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
