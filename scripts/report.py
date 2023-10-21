import sys
import os
import pandas as pd
import constants
import plotting as plt


def bytes_to_mb(bytes):
    return bytes / (1024 * 1024)

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

def compress(results_dcx, output_dir, file_names):
    results_dcx['ratio_percentage'] = compute_and_set_ratio_percentage(results_dcx)
    #convert bytes to MB
    results_dcx['peak_comp'] = results_dcx['peak_comp'].apply(lambda x: bytes_to_mb(x))
    results_dcx['stack_comp'] = results_dcx['stack_comp'].apply(lambda x: bytes_to_mb(x))
    results_dcx['peak_decomp'] = results_dcx['peak_decomp'].apply(lambda x: bytes_to_mb(x))
    results_dcx['stack_decomp'] = results_dcx['stack_decomp'].apply(lambda x: bytes_to_mb(x))

    results_dcx.drop(columns=['plain_size', 'compressed_size'], inplace=True)
    results_dcx.set_index('file', inplace=True)

    filter = results_dcx['algorithm'].str.contains('GCIS') 
    results_gcis = results_dcx[filter]
    results_dcx = results_dcx[~filter]

    print("## Creating charts to compression time comparison between DCX and GCIS")
    plt.generate_chart(file_names, results_dcx, results_gcis, constants.TIME[0], output_dir, "compress")
    print("## Creating charts to decompression time comparison between DCX and GCIS")
    plt.generate_chart(file_names, results_dcx, results_gcis, constants.TIME[1], output_dir, "decompress")
    print("## Creating charts to compressed size ratio comparison between DCX and GCIS")
    plt.generate_chart(file_names, results_dcx, results_gcis, constants.RATIO_INFO, output_dir, "ratio")
    print("## Creating charts to compare memory usage during compression with DCX and GCIS")
    plt.generate_memory_chart(file_names, results_dcx, results_gcis, constants.MEMORY_USAGE[0], output_dir)
    print("## Creating charts to compare memory usage during decompression with DCX and GCIS")
    plt.generate_memory_chart(file_names, results_dcx, results_gcis, constants.MEMORY_USAGE[1], output_dir)

def extract(results_dcx, output_dir, file_names):
    results_dcx.set_index('file', inplace=True)

    plt.generate_extract_chart(file_names, results_dcx, constants.TIME[2], output_dir)
    plt.generate_extract_chart(file_names, results_dcx, constants.MEMORY_USAGE[2], output_dir)
    plt.generate_extract_chart(file_names, results_dcx, constants.MEMORY_USAGE[3], output_dir)

def main(argv):
    file_names = get_file_names()
    results_dcx = pd.read_csv(argv[1], sep='|', decimal=".")
    output_dir = argv[2]
    operation = argv[3]

    if operation == "compress":
        compress(results_dcx, output_dir, file_names)
    elif operation == "extract":
        extract(results_dcx, output_dir, file_names)

if __name__ == '__main__':
    sys.exit(main(sys.argv))