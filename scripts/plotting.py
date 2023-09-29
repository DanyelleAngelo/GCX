import matplotlib.pyplot as plt
import numpy as np
import constants as cons
import locale

locale.setlocale(locale.LC_NUMERIC, 'pt_BR.utf-8')  
colors=['purple', 'brown', 'gray', 'cyan', 'magenta', 'lime', 'olive', 'teal', 'navy', 'black', 'yellow']
width_bar = 0.2

def customize_chart(y_label, x_label, title):
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.title(title)
    plt.legend()

    plt.xticks(rotation=45)
    plt.tight_layout(pad=3.0)  
    plt.grid(linestyle=':', alpha=0.5)

def generate_chart(file_names, results_dcx, results_gcis, information, output_dir, operation):
    col = information['col']
    plt.figure(figsize=(10,8))

    i=0
    plt.bar(cons.COVERAGE, results_dcx[col], width=0.5, color=colors[i], edgecolor='black', label="DCX")

    j=0
    for index, row in results_gcis.iterrows():
        i = i+1
        codec = index.split("-")[2].upper()
        plt.axhline(y=row[col], linestyle=cons.LINE_STYLE[j], linewidth=2, color=colors[i], label=f"GCIS {codec}")
        if j > len(cons.LINE_STYLE)-1:
            j=0
        j=j+1

    customize_chart(information['y_label'], f"{information['title']} {results_dcx.index[0].upper()}", "Variações de cobertura para o DCX.")
    max_value = max(max(results_gcis[col]), max(results_dcx[col]))
    if(operation!="ratio"):
        max_value=locale.atof(max_value)
    #plt.ylim(0, max_value + 10)

    file = f"{output_dir}/{information['output_file']}-{results_dcx.index[0]}.png"
    plt.savefig(file)

def generate_memory_chart(file_names, results_dcx, results_gcis, information, output_dir):
    plt.figure(figsize=(10,8))

    indexes = np.arange(len(cons.COVERAGE))

    plt.bar(indexes - width_bar, results_dcx[information['peak']], label='DCX - Peak', width=width_bar, align='center')
    plt.bar(indexes, results_dcx[information['stack']], label='DCX - Stack', width=width_bar, align='center')

    i=0
    for index, row in results_gcis.iterrows():
        codec = index.split("-")[2].upper()
        plt.axhline(y=row[information['peak']], linestyle=cons.LINE_STYLE[i], color=colors[i], label=f"GCIS {codec} - peak")
        i+=1
        plt.axhline(y=row[information['stack']], linestyle=cons.LINE_STYLE[i], color=colors[i], label=f"GCIS {codec} - stack")
        i+=1


    customize_chart(information['y_label'], f"{information['title']} {results_dcx.index[0].upper()}", "Variações de cobertura para o DCX.")
    plt.xticks(indexes, cons.COVERAGE)

    file = f"{output_dir}/{information['output_file']}-{results_dcx.index[0]}.png"
    plt.savefig(file)

def generate_extract_chart(file_names, results_dcx, results_gcis, information, output_dir):
    plt.figure(figsize=(10,8))

    #agrupa as informações de tempo por tamanho de substring
    time_by_substring_size = {}
    for index, row in results_dcx.iterrows():
        key = str(row['substring_size'])
        if key in time_by_substring_size:
            time_by_substring_size[key].append(row['time'])
        else:
            time_by_substring_size[key] = [row['time']]

    substring_size = list(time_by_substring_size.keys())
    position = np.arange(len(substring_size))

    #monta o gráfico efetivamente (informações do dcx)
    for i, coverage in enumerate(cons.COVERAGE):
        time = [time_by_substring_size[size][i] for size in substring_size]
        plt.bar(position + i * width_bar, time, width_bar, label=coverage)
    #gráfico do GCIS
    for index, row in results_gcis.iterrows():
        plt.axhline(y=row['time'], linestyle=cons.LINE_STYLE[i], label=f"GCIS - Tamanho de substring {row['substring_size']}")

    customize_chart(information['y_label'], f"{information['title']} {results_dcx.index[0].upper()}", "Tamanho da Substring")
    plt.xticks(position + width_bar * (len(cons.COVERAGE) - 1) / 2, substring_size) 
    file = f"{output_dir}/{information['output_file']}-{results_dcx.index[0]}.png"
    plt.savefig(file)





