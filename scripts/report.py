import sys
import os
import glob
import pandas as pd
import constants
import plotting as plt

compress_max_values = {
    'peak_comp': 0.0,
    'peak_decomp': 0.0,
    'compression_time': 0.0,
    'decompression_time': 0.0,
    'compressed_size': 0.0
}

extract_values = {
    'peak': 0.0,
    'time': 0.0,
    'min_time':sys.float_info.max,
}

def bytes_to_mb(bytes):
    return bytes / (1024 * 1024)

def compute_ratio_percentage(compressed_size, plain_size):
    return (compressed_size/plain_size)*100

def generate_chart_line(df_list, output_dir):
    for df in df_list:
        print(f"\n## FILE: {df.index[0]}")
        print(f"\t- Creating charts extract comparison between DCX and GCIS")
        plt.generate_chart_line(df, constants.EXTRACT["time"], output_dir, extract_values["time"], extract_values["min_time"])
        #plt.generate_chart_line(df, constants.EXTRACT["peak"], output_dir, extract_values["peak"])

def generate_chart(results_dcx, results_gcis, function, information, output_dir, metric, max_value):
    print(f"\t- Creating charts to {metric} comparison between DCX and GCIS")
    getattr(plt, function)(results_dcx, results_gcis, information, output_dir, max_value)

def generate_compress_chart(df_list, output_dir):
    max_time = max(compress_max_values["compression_time"], compress_max_values["decompression_time"])
    max_memory = max(compress_max_values["peak_comp"], compress_max_values["peak_decomp"])

    for df in df_list:
        filter = df['algorithm'].str.contains('GCIS') 
        gcis = df[filter]
        dcx = df[~filter]
        
        print(f"\n## FILE: {df.index[0]}")
        generate_chart(dcx, gcis, "generate_chart_bar", constants.COMPRESS_AND_DECOMPRESS['cmp_time'], output_dir, "compression time", max_time+5)
        generate_chart(dcx, gcis, "generate_chart_bar", constants.COMPRESS_AND_DECOMPRESS['dcmp_time'], output_dir, "decompression time", max_time+5)
        generate_chart(dcx, gcis, "generate_chart_bar", constants.COMPRESS_AND_DECOMPRESS['ratio'], output_dir, "compressed size ratio", 100)
        generate_chart(dcx, gcis, "generate_memory_chart", constants.COMPRESS_AND_DECOMPRESS['cmp_peak'], output_dir, "memory usage", max_memory)
        generate_chart(dcx, gcis, "generate_memory_chart", constants.COMPRESS_AND_DECOMPRESS['dcmp_peak'], output_dir, "memory usage", max_memory)

def set_max_values(max_values, df):
    for key in max_values.keys():
        if key in df:
            column_max = df[key].max()
            if column_max > max_values[key]:
                max_values[key] = column_max
        elif key == "min_time":
            min_value = df['time'].min()
            max_values[key] = min_value if min_value < max_values[key] else max_values[key]

def prepare_dataset(df):
    plain_size = df['plain_size'][0]
    #calculate compression rate
    df['compressed_size'] = df['compressed_size'].apply(lambda x: compute_ratio_percentage(x, plain_size))
    #convert bytes to MB
    df['peak_comp'] = df['peak_comp'].apply(lambda x: bytes_to_mb(x))
    df['peak_decomp'] = df['peak_comp'].apply(lambda x: bytes_to_mb(x))
    df['stack_comp'] = df['stack_comp'].apply(lambda x: bytes_to_mb(x))
    df['stack_decomp'] = df['stack_comp'].apply(lambda x: bytes_to_mb(x))

def get_data_frame(path, operation):
    files = glob.glob(f"{path}*.csv")
    df_list = []

    for file in files:
        df = pd.read_csv(file, sep='|', decimal=".")
        df.set_index('file', inplace=True)
        if operation == "compress":
            prepare_dataset(df)
            set_max_values(compress_max_values, df)
        elif operation == "extract":
            set_max_values(extract_values, df)
        df_list.append(df)

    return df_list

def main(argv):
    path = argv[1]
    output_dir = argv[2]
    operation = argv[3]

    df_list = get_data_frame(path, operation)
    if operation == "compress":
        print("\n\t------ Compress and Decompress ------")
        generate_compress_chart(df_list, output_dir)
    elif operation == "extract":
        print("\n\t------ Extract ------")
        generate_chart_line(df_list, output_dir)

if __name__ == '__main__':
    sys.exit(main(sys.argv))