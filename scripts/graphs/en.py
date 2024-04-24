
COMPRESS_AND_DECOMPRESS = {
    'cmp_time': {
        "col": "compression_time",
        "y_label": "Compression time (s).",
        "x_label": "Algorithms",
        "title": "Compression time - ",
        "output_file": "compression_time",
        "legend":  "Algorithm"
    },
    'dcmp_time': {
        "col": "decompression_time",
        "y_label": "Decompression time (s).",
        "x_label": "Algorithms",
        "title": "Decompression time - ",
        "output_file": "decompression_time",
        "legend":  "Algorithms"
    },
    'cmp_peak': {
        "col": "peak_comp",
        "stack": "stack_comp",
        "y_label": "Memory usage MB (peak)",
        "x_label": "Algorithms",
        "title": "Memory usage - ",
        "output_file": "memory_usage_comp",
        "legend":  "Algorithms"
    },
    'dcmp_peak': {
        "col": "peak_decomp",
        "stack": "stack_decomp",
        "y_label": "Memory usage MB (peak)",
        "x_label": "Algorithms",
        "title": "Memory usage - ",
        "output_file": "memory_usage_decomp",
        "legend":  "Algorithms"
    },
    'ratio': {
        "col": "compressed_size",
        "y_label": "Compression ratio (%).",
        "x_label": "Algorithms",
        "title": "Compression ratio - ",
        "output_file": "ratio",
        "legend":  "Algorithms"
    }
}

EXTRACT = {
    'time': {
        "col": "time",
        "y_label": "Extraction time (\u03BCs)",
        "x_label": "Substring length",
        "title": "Extraction time (\u03BCs)",
        "output_file": "extracting_time",
        "legend":  "Algorithms"
    },
    'peak': {
        "col": "peak",
        "y_label": "Pico de memória em MB (log)",
        "x_label": "Algorithms",
        "title": "Pico de memória durante a extração - ",
        "output_file": "peak_memory_usage_extract"
    },
    'stack': {
        "col": "stack",
        "y_label": "Uso de memória em MB (log)",
        "x_label": "Algorithms",
        "title": "Uso de memória durante a extração (stack) - ",
        "output_file": "stack_memory_usage_extract"
    }
}