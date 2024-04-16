
COMPRESS_AND_DECOMPRESS = {
    'cmp_time': {
        "col": "compression_time",
        "y_label": "Tempo de compressão (s).",
        "x_label": "Algoritmo",
        "title": "Tempo de compressão - ",
        "output_file": "compression_time",
        "legend":  "Algoritmo"
    },
    'dcmp_time': {
        "col": "decompression_time",
        "y_label": "Tempo de descompressão (s).",
        "x_label": "Algoritmo",
        "title": "Tempo de descompressão - ",
        "output_file": "decompression_time",
        "legend":  "Algoritmo"
    },
    'peak_comp': {
        "col": "peak_comp",
        "stack": "stack_comp",
        "y_label": "Consumo de memória em MB (pico)",
        "x_label": "Algoritmo",
        "title": "Consumo de memória durante a compressão - ",
        "output_file": "memory_usage_comp",
        "legend":  "Algoritmo"
    },
    'dcmp_peak': {
        "col": "peak_decomp",
        "stack": "stack_decomp",
        "y_label": "Consumo de memória em MB (pico)",
        "x_label": "Algoritmo",
        "title": "Consumo de memória durante a descompressão - ",
        "output_file": "memory_usage_decomp",
        "legend":  "Algoritmo"
    },
    'ratio': {
        "col": "compressed_size",
        "y_label": "Taxa de compressão (%).",
        "x_label": "Algoritmo",
        "title": "Taxa de compressão - ",
        "output_file": "ratio",
        "legend":  "Algoritmo"
    }
}

EXTRACT = {
    'time': {
        "col": "time",
        "y_label": "Tempo de extração para subcadeias de tamanho 1,10, 100, 1000 e 10000.",
        "x_label": "Tamanho do intervalo extraído",
        "title": "Tempo de extração (s)",
        "output_file": "extracting_time",
        "legend": "Algoritmo"
    },
    'peak': {
        "col": "peak",
        "y_label": "Pico de memória em MB (log)",
        "x_label": "Algoritmo",
        "title": "Pico de memória durante a extração - ",
        "output_file": "peak_memory_usage_extract"
    },
    'stack': {
        "col": "stack",
        "y_label": "Uso de memória em MB (log)",
        "x_label": "Algoritmo",
        "title": "Uso de memória durante a extração (stack) - ",
        "output_file": "stack_memory_usage_extract"
    }
}