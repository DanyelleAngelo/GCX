
TIME = [
    {
        "col": "compression_time",
        "y_label": "Tempo gasto para compressão (s).",
        "x_label": "Variações de cobertura para o DCX.",
        "title": "Velocidade de compressão para ",
        "output_file": "compression_time"
    },
    {
        "col": "decompression_time",
        "y_label": "Tempo gasto para descompressão (s).",
        "x_label": "Variações de cobertura para o DCX.",
        "title": "Velocidade de descompressão para textos repetitivos",
        "output_file": "decompression_time"
    },
    {
        "col": "time",
        "y_label": "Tempo gasto para extração de 1000 substrings (em escala logarítmica).",
        "x_label": "Algoritmo usado para extração",
        "title": "Tempo de extração (s)",
        "output_file": "extracting_time"
    }
]

MEMORY_USAGE = [
    {
        "peak": "peak_comp",
        "stack": "stack_comp",
        "y_label": "Consumo de memória em MB (log)",
        "x_label": "Variações de cobertura para o DCX.",
        "title": "Uso de memória durante a compressão - ",
        "output_file": "memory_usage_comp"
    },
    {
        "peak": "peak_decomp",
        "stack": "stack_decomp",
        "y_label": "Consumo de memória em MB (log)",
        "x_label": "Variações de cobertura para o DCX.",
        "title": "Uso de memória durante a descompressão - ",
        "output_file": "memory_usage_decomp"
    },
    {
        "col": "peak",
        "y_label": "Pico de memória em MB (log)",
        "x_label": "Variações de cobertura para o DCX.",
        "title": "Pico de memória durante a extração - ",
        "output_file": "peak_memory_usage_extract"
    },
    {
        "col": "stack",
        "y_label": "Uso de memória em MB (log)",
        "x_label": "Variações de cobertura para o DCX.",
        "title": "Uso de memória durante a extração (stack) - ",
        "output_file": "stack_memory_usage_extract"
    }
]

RATIO_INFO = {
    "col": "ratio_percentage",
    "y_label": "Taxa de compressão (%).",
    "x_label": "Variações de cobertura para o DCX.",
    "title": "Taxa de compressão de arquivos usando GCIS e DCX - ",
    "output_file": "ratio"
}

COVERAGE = ["DC3","DC4","DC5","DC6","DC7","DC8","DC9","DC11","DC15","DC30","DC60"]

LINE_STYLE = ["--", ":", "-.", "-","--"]