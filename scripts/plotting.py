import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import constants as cons
import random
import pandas as pd

cmap= {
    'compression_time': cm.get_cmap('tab10'),
    'decompression_time': cm.get_cmap('tab20'),
    'compressed_size': cm.get_cmap('Dark2'),
    'memory': cm.get_cmap('Set1'),
}

width_bar = 0.2

def customize_chart(information, title, legend):
    plt.xlabel(information['x_label'])
    plt.ylabel(information['y_label'])
    plt.title(title)
    plt.legend(title=legend)

    plt.xticks(rotation=45)
    plt.tight_layout(pad=3.0)  
    plt.grid(linestyle=':', alpha=0.5)

def generate_chart(results_dcx, results_gcis, information, output_dir, max_value):
    col = information['col']
    plt.figure(figsize=(10,8))

    algorithm = results_dcx['algorithm'].unique().tolist()

    plt.bar(algorithm, results_dcx[col], width=0.5, color=cmap[col](0), edgecolor='black', label="DCX")

    j=0
    for index, row in results_gcis.iterrows():
        plt.axhline(y=row[col], color=cmap[col](j+1), linestyle=cons.LINE_STYLE[j], linewidth=2, label=index)
        j+=1

    customize_chart(information, f"{information['title']} {results_dcx.index[0].upper()}", "Algoritmo")
    plt.ylim(0, max_value+5)

    file = f"{output_dir}/{information['output_file']}-{results_dcx.index[0]}.png"
    plt.savefig(file)

def generate_memory_chart(results_dcx, results_gcis, information, output_dir, max_value):
    plt.figure(figsize=(10,8))

    algorithm = results_dcx['algorithm'].unique().tolist()

    indexes = np.arange(len(algorithm))
    operation='memory'

    plt.bar(indexes - width_bar, results_dcx[information['col']], label='DCX - Peak', width=width_bar, align='center', color=cmap[operation](0))
    plt.bar(indexes, results_dcx[information['stack']], label='DCX - Stack', width=width_bar, align='center', color=cmap[operation](1))

    i=0
    for index, row in results_gcis.iterrows():
        plt.axhline(y=row[information['col']], linestyle=cons.LINE_STYLE[i], color=cmap[operation](i+2), label=f"GCIS {index} - col")
        i+=1
        plt.axhline(y=row[information['stack']], linestyle=cons.LINE_STYLE[i], color=cmap[operation](i+2), label=f"GCIS {index} - stack")
        i+=1

    plt.yscale('log')
    customize_chart(information, f"{information['title']} {results_dcx.index[0].upper()}", "Algoritmo")
    plt.xticks(indexes, algorithm)

    file = f"{output_dir}/{information['output_file']}-{results_dcx.index[0]}.png"
    plt.savefig(file)

def generate_extract_chart(results, information, output_dir, max_value):
    fig = plt.figure(figsize=(15,10))
    ax = fig.add_subplot(1, 1, 1)

    #para preservar a ordem dos dados mesmos após o groupby
    results['algorithm'] = pd.Categorical(results['algorithm'], categories=results['algorithm'].unique(), ordered=True)
    results = results.sort_values(by='algorithm')
    group = results.groupby(['algorithm', 'substring_size'])[information['col']].mean().unstack()

    algorithm = group.index
    substring_list = group.columns
    time = group.values
    start = np.arange(len(algorithm)) 
    for i, substring_size in enumerate(substring_list):
        ax.bar(start + width_bar * i, time[:, i], width_bar, label=substring_size)

    customize_chart(information, f"{information['title']} - {results.index[0].upper()}", "Tamanho do intervalo extraído")
    plt.xticks(np.arange(len(algorithm)), algorithm)
    plt.ylim(0, max_value+5)

    file = f"{output_dir}/{information['output_file']}-{results.index[0]}.png"
    plt.savefig(file)
