import sys
import os
import glob
import pandas as pd
import constants
import plotting as graphs

metrics_information = {
    'compression_time': constants.TIME[0],
    'decompression_time': constants.TIME[1],
    'extract_time': constants.TIME[2],
    'compressed_size': constants.RATIO_INFO,
    'memory_compress': constants.MEMORY_USAGE[0],
    'memory_decompress': constants.MEMORY_USAGE[1],
}

def bytes_to_mb(bytes):
    return bytes / (1024 * 1024)

def compute_and_set_ratio_percentage(results_dcx):
    ratio_percentage=[]
    for index, row in results_dcx.iterrows():
        ratio= (row['compressed_size']/row['plain_size'])*100
        ratio_percentage.append(ratio)
    return ratio_percentage

def compress_and_decompress(results_dcx, information, output_dir, function, max_value):
    filter = results_dcx['algorithm'].str.contains('GCIS') 
    results_gcis = results_dcx[filter]
    results_dcx = results_dcx[~filter]

    print(f"## Creating charts {information['col']} comparison between DCX and GCIS")
    getattr(graphs, function)(results_dcx, results_gcis, information, output_dir, max_value)

def extract(results_dcx, output_dir, max_value):
    print(f"## Creating charts extract comparison between DCX and GCIS")
    graphs.generate_extract_chart(results_dcx, constants.TIME[2], output_dir, max_value)
    graphs.generate_extract_chart(results_dcx, constants.MEMORY_USAGE[2], output_dir, max_value)
    graphs.generate_extract_chart(results_dcx, constants.MEMORY_USAGE[3], output_dir, max_value)

def get_data_frame(path, information):
    files = glob.glob(f"{path}*.csv")
    print()
    df_list = []
    max_info = 0
    for file in files:
        df = pd.read_csv(file, sep='|', decimal=".")
        df.set_index('file', inplace=True)

        max_file_info = df[information['col']].max()
        df_list.append(df)
        if max_file_info > max_info:
            max_info = max_file_info

    return df_list, max_info

def generate_chart(df_list, information, output_dir, max_value):
    for df in df_list:
        if information['col'] == 'compressed_size':
            df['compressed_size'] = compute_and_set_ratio_percentage(df)
            compress_and_decompress(df, information, output_dir, "generate_chart", max_value)
        elif 'peak' in information['col']:
            #convert bytes to MB
            df[information['col']] = df[information['col']].apply(lambda x: bytes_to_mb(x))
            df[information['stack']] = df[information['stack']].apply(lambda x: bytes_to_mb(x))
            compress_and_decompress(df, information, output_dir, "generate_memory_chart", max_value)
        elif information['col'] == 'time':
            extract(df, output_dir, max_value)
        else:
            compress_and_decompress(df, information, output_dir, "generate_chart", max_value)

def main(argv):
    path = argv[1]
    output_dir = argv[2]
    metric = argv[3]
    info = metrics_information[metric]

    df_list, max_value = get_data_frame(path, info)
    generate_chart(df_list, info, output_dir, max_value)

if __name__ == '__main__':
    sys.exit(main(sys.argv))