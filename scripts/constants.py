
TIME = [
    {
        "col": "compression_time",
        "y_label": "Tempo gasto para compressão (s).",
        "title": "Velocidade de compressão para ",
        "output_file": "compression_time"
    },
    {
        "col": "decompression_time",
        "y_label": "Tempo gasto para descompressão (s).",
        "title": "Velocidade de descompressão para textos repetitivos",
        "output_file": "decompression_time"
    },
    {
        "col": "time",
        "y_label": "Tempo gasto para extração de 1000 substrings.",
        "title": "Tempo de extração",
        "output_file": "extracting_time"
    }
]

MEMORY_USAGE = [
    {
        "peak": "peak_comp",
        "stack": "stack_comp",
        "y_label": "Consumo de memória em bytes",
        "title": "Uso de memória durante a compressão - ",
        "output_file": "memory_usage_comp"
    },
    {
        "peak": "peak_decomp",
        "stack": "stack_decomp",
        "y_label": "Consumo de memória em bytes",
        "title": "Uso de memória durante a descompressão - ",
        "output_file": "memory_usage_decomp"
    }
]

RATIO_INFO = {
    "col": "ratio_percentage",
    "y_label": "Taxa de compressão (%).",
    "title": "Taxa de compressão de arquivos usando GCIS e DCX - ",
    "output_file": "ratio"
}

COVERAGE = ["DC3","DC4"]#,"DC5","DC6","DC7","DC8","DC9","DC15","DC30","DC60"]

LINE_STYLE = ["--", ":", "-.", "-","--"]