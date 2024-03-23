
COMPRESS_AND_DECOMPRESS = {
    'cmp_time': {
        "col": "compression_time",
        "y_label": "Compression time (s).",
        "x_label": "Algorithm",
        "title": "Compression time - ",
        "output_file": "compression_time",
        "legend":  "Algorithm"
    },
    'dcmp_time': {
        "col": "decompression_time",
        "y_label": "Decompression time (s).",
        "x_label": "Algorithm",
        "title": "Decompression time - ",
        "output_file": "decompression_time",
        "legend":  "Algorithm"
    },
    'cmp_peak': {
        "col": "peak_comp",
        "stack": "stack_comp",
        "y_label": "Memory usage MB (log)",
        "x_label": "Algorithm",
        "title": "Memory usage - ",
        "output_file": "memory_usage_comp",
        "legend":  "Algorithm"
    },
    'dcmp_peak': {
        "col": "peak_decomp",
        "stack": "stack_decomp",
        "y_label": "Memory usage MB (log)",
        "x_label": "Algorithm",
        "title": "Memory usage - ",
        "output_file": "memory_usage_decomp",
        "legend":  "Algorithm"
    },
    'ratio': {
        "col": "compressed_size",
        "y_label": "Compression ratio (%).",
        "x_label": "Algorithm",
        "title": "Compression ratio - ",
        "output_file": "ratio",
        "legend":  "Algorithm"
    }
}

EXTRACT = {
    'time': {
        "col": "time",
        "y_label": "Extraction time for substrings of length 10, 100, 1000 and 10000",
        "x_label": "Algorithm",
        "title": "Extraction time (s)",
        "output_file": "extracting_time",
        "legend":  "Algorithm"
    },
    'peak': {
        "col": "peak",
        "y_label": "Pico de memória em MB (log)",
        "x_label": "Algorithm",
        "title": "Pico de memória durante a extração - ",
        "output_file": "peak_memory_usage_extract"
    },
    'stack': {
        "col": "stack",
        "y_label": "Uso de memória em MB (log)",
        "x_label": "Algorithm",
        "title": "Uso de memória durante a extração (stack) - ",
        "output_file": "stack_memory_usage_extract"
    }
}