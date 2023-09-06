import matplotlib.pyplot as plt
import constants as cons

colors=['palegreen', 'orange', 'red', 'red', 'red']

def customize_chart(y_label, title):
    plt.xlabel("Variações de cobertura para o DCX.")
    plt.ylabel(y_label)
    plt.title(title)
    plt.legend()

    plt.xticks(rotation=45)
    plt.tight_layout(pad=3.0)  
    plt.grid(linestyle=':', alpha=0.5)

def generate_chart(file_names, results_dcx, results_gcis, information, output_dir):
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

    customize_chart(information['y_label'], f"{information['title']} {results_dcx.index[0].upper()}")
    max_value = max(max(results_gcis[col]), max(results_dcx[col]))
    plt.ylim(0, max_value + 10)

    file = f"{output_dir}/{information['output_file']}-{results_dcx.index[0]}.png"
    plt.savefig(file)