import matplotlib.pyplot as plt

def customize_chart(y_label, title):
    plt.xlabel("Arquivos de entrada.")
    plt.ylabel(y_label)
    plt.title(title)
    plt.legend()

    plt.xticks(rotation=45)
    plt.tight_layout(pad=3.0)  
    plt.grid(linestyle=':', alpha=0.5)

def plot_text(file_names, results,va_pos, ha_pos):
    for i in range(len(file_names)):
        plt.text(file_names[i], results[i], str(results[i]), ha=ha_pos, va=va_pos, fontsize=8, weight='bold')

def generate_line_chart(file_names, results_gcis, results_dcx, information):
    col = information['col']
    plt.figure(figsize=(10,8))

    plt.plot(file_names, results_gcis[col], marker='o', markersize=4, linestyle='--', label="GCIS")
    plot_text(file_names, results_gcis[col], 'top', 'left')

    plt.plot(file_names, results_dcx[col], marker='s', markersize=4, linestyle=':', label="DCX")
    plot_text(file_names, results_dcx[col], 'bottom', 'left')

    customize_chart(information['y_label'], information['title'])
    plt.savefig(information['output_file'])

def generate_bar_chart(file_names, results_gcis, results_dcx, information):
    col = information['col']
    plt.figure(figsize=(10,8))
    bar_width = 0.5

    plt.bar(file_names, results_gcis[col], edgecolor='black', hatch="/", width=bar_width, label='GCIS')

    plt.bar(file_names, results_dcx[col], edgecolor='black', width=bar_width, bottom=results_gcis[col], label='DCX')

    customize_chart(information['y_label'], information['title'])
    plt.savefig(information['output_file'])