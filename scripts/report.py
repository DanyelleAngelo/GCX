import sys
import os
import pandas as pd

import constants
import plotting as plt

def get_file_names():
    file_names = []

    with open("../file_names.txt", "r") as file:
        for line in file:
            file_names.append(line.rstrip().split("/")[1])
    return file_names

def compute_and_set_ratio_percentage(results_dcx):
    ratio_percentage=[]
    for index, row in results_dcx.iterrows():
        ratio= (row['compressed_size']/row['plain_size'])*100
        ratio_percentage.append(ratio)
    return ratio_percentage

def main(argv):
    file_names = get_file_names()
    results_dcx = pd.read_csv(argv[1])
    output_dir = argv[2]
    results_dcx['ratio_percentage'] = compute_and_set_ratio_percentage(results_dcx)
    results_dcx.drop(columns=['plain_size', 'compressed_size'], inplace=True)
    results_dcx.set_index('file', inplace=True)

    results_gcis = results_dcx[results_dcx.index.str.contains('gcis')]
    results_dcx = results_dcx.drop(results_gcis.index)

    plt.generate_chart(file_names, results_dcx, results_gcis, constants.COMPRESSION_TIME, output_dir)
    plt.generate_chart(file_names, results_dcx, results_gcis, constants.DECOMPRESSION_TIME, output_dir)
    plt.generate_chart(file_names, results_dcx, results_gcis, constants.RATIO_INFO, output_dir)
    plt.generate_memory_chart(file_names, results_dcx, results_gcis, constants.MEMORY_USAGE[0], output_dir)

if __name__ == '__main__':
    sys.exit(main(sys.argv))