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
        compressed_file_size = os.path.getsize(row['compressed_file'])
        plain_file_size = os.path.getsize(row['file'])
        ratio= (compressed_file_size/plain_file_size)*100
        ratio_percentage.append(ratio)
    return ratio_percentage

def main(argv):
    file_names = get_file_names()
    results_dcx = pd.read_csv(argv[1]) 
    results_gcis = pd.read_csv(argv[2])
    results_gcis.set_index('file', inplace=True)
    results_dcx['ratio_percentage'] = compute_and_set_ratio_percentage(results_dcx)

    plt.generate_line_chart(file_names, results_gcis, results_dcx, constants.COMPRESSION_INFO)
    plt.generate_line_chart(file_names, results_gcis, results_dcx, constants.DECOMPRESSION_INFO)
    plt.generate_bar_chart(file_names, results_gcis, results_dcx, constants.RATIO_INFO)

if __name__ == '__main__':
    sys.exit(main(sys.argv))