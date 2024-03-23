import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import pandas as pd

LINE_STYLE = ["--", ":", "-.", "-","--"]

color_map= {
    'compression_time': {
        "default_color": "#007599", "highlighted_color": "#00ffff", "gcis": cm.get_cmap('PiYG')
    },
    'decompression_time': {
        "default_color": "#007599", "highlighted_color": "#00ffff", "gcis": cm.get_cmap('PiYG')
    },
    'compressed_size': {
        "default_color": "#bc5090", "highlighted_color": "#ffa600", "gcis": cm.get_cmap('coolwarm')
    },
    'memory': cm.get_cmap('Set1'),
    'default': cm.get_cmap('winter')
}



width_bar = 0.2

def customize_chart(information, title):
    plt.xlabel(information['x_label'])
    plt.ylabel(information['y_label'])
    plt.title(title)
    plt.legend(title=information["legend"])

    plt.xticks(rotation=45)
    plt.tight_layout(pad=3.0)  
    plt.grid(linestyle=':', alpha=0.5)

def generate_chart_bar(results_gcx, results_gcis, information, output_dir, max_value=None):
    plt.figure(figsize=(10,8))

    target_column = information['col']
    algorithms = results_gcx['algorithm'].unique().tolist()
    gcx_column = algorithms.index('GCX')

    #Sets chart colors
    default_color = color_map[target_column]["default_color"]
    colors = [default_color] * len(algorithms)
    colors[gcx_column] = color_map[target_column]["highlighted_color"]

    #GCX results
    plt.bar(algorithms, results_gcx[target_column], width=0.5, color=colors, edgecolor='black', label="GCX")

    j=0
    #GCIS results
    for index, row in results_gcis.iterrows():
        plt.axhline(y=row[target_column], color=color_map[target_column]["gcis"](j), linestyle=LINE_STYLE[j], linewidth=2, label=row['algorithm'])
        j+=1


    file=results_gcx.index[0].upper().split("-")[-1]
    customize_chart(information, f"{information['title']} {file}")

    if max_value != None:
        plt.yticks(np.arange(0, max_value, 3))
        plt.ylim(0, max_value)

    file = f"{output_dir}/{information['output_file']}-{results_gcx.index[0]}.png"
    plt.savefig(file)
    plt.close()

def generate_memory_chart(results_gcx, results_gcis, information, output_dir, max_value):
    plt.figure(figsize=(10,8))
    
    algorithm = results_gcx['algorithm'].unique().tolist()
    indexes = np.arange(len(algorithm))
    operation='memory'

    #GCX results
    plt.bar(indexes - width_bar, results_gcx[information['col']], label='GCX - Peak', width=width_bar, align='center', color=color_map[operation](0))
    plt.bar(indexes, results_gcx[information['stack']], label='GCX - Stack', width=width_bar, align='center', color=color_map[operation](1))

    i=0
    #GCIS results
    for index, row in results_gcis.iterrows():
        plt.axhline(y=row[information['col']], linestyle=LINE_STYLE[i], color=color_map[operation](i+2), label=f"GCIS {index} - peak")
        i+=1
        plt.axhline(y=row[information['stack']], linestyle=LINE_STYLE[i], color=color_map[operation](i+2), label=f"GCIS {index} - stack")
        i+=1

    file=results_gcx.index[0].upper().split("-")[-1]
    customize_chart(information, f"{information['title']} {file}")

    plt.xticks(indexes, algorithm)
    plt.ylim(0, max_value)

    file = f"{output_dir}/{information['output_file']}-{results_gcx.index[0]}.png"
    plt.savefig(file)
    plt.close()

def generate_extract_chart(results, information, output_dir, max_value, min_value):
    plt.figure(figsize=(10,8))

    for algorithm, group in results.groupby('algorithm'):
        plt.plot(group['substring_size'], group['time'], marker='o', linewidth=0.5, label=algorithm)

    #plt.ylim([0, max_value+5])
    plt.xscale('log')
    plt.yscale('log')

    file=results.index[0].upper().split("-")[-1]
    customize_chart(information, f"{information['title']} - {file}")

    file = f"{output_dir}/{information['output_file']}-{results.index[0]}.png"
    plt.savefig(file)
    plt.close()
