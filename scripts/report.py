import sys
import os
import plotting as plt
import glob
import pandas as pd
import json
import utils as ut

ratio_analysis = {
    'min_gcis_ef':0.0,
    'min_gcis_s8b':0.0,
    'min_dcx':0.0,
    'max_gcis_ef':0.0,
    'max_gcis_s8b':0.0,
    'max_dcx':0.0
}

compress_max_values = {
    'peak_comp': 0.0,
    'peak_decomp': 0.0,
    'compression_time': 0.0,
    'decompression_time': 0.0,
    'compressed_size': 0.0
}

mean_values = {
    'peak_comp': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'GC*': 0.0 },
    'peak_decomp': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'GC*': 0.0},
    'compression_time': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'GC*': 0.0},
    'decompression_time': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'GC*': 0.0},
    'compressed_size': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'GC*': 0.0}
}

extract_values = {
    'peak': 0.0,
    'time': 0.0,
    'min_time':sys.float_info.max,
}

def generate_chart(results_dcx, results_gcis, function, information, output_dir, metric, max_value, plt):
    print(f"\t- Creating charts to {metric} comparison between DCX and GCIS")
    getattr(plt, function)(results_dcx, results_gcis, information, output_dir, max_value)

def generate_chart_line(df_list, output_dir, constants):
    for df in df_list:
        print(f"\n## FILE: {df.index[0]}")
        print(f"\t- Creating charts extract comparison between GCX and GCIS")
        plt.generate_chart_line(df, constants.EXTRACT["time"], output_dir, extract_values["time"], extract_values["min_time"])

def generate_compress_chart(df_list, output_dir, constants):
    max_memory = max(compress_max_values["peak_comp"], compress_max_values["peak_decomp"])
    for df in df_list:
        filter = df['algorithm'].str.contains('GCIS') 
        gcis = df[filter]
        dcx = df[~filter]
        
        print(f"\n## FILE: {df.index[0]}")
        generate_chart(dcx, gcis, "generate_chart_bar", constants.COMPRESS_AND_DECOMPRESS['cmp_time'], output_dir, "compression time", compress_max_values["compression_time"]+5, plt)
        generate_chart(dcx, gcis, "generate_chart_bar", constants.COMPRESS_AND_DECOMPRESS['dcmp_time'], output_dir, "decompression time", compress_max_values["decompression_time"]+5, plt)
        
        generate_chart(dcx, gcis, "generate_chart_bar", constants.COMPRESS_AND_DECOMPRESS['ratio'], output_dir, "compressed size ratio", 100, plt)
        
        generate_chart(dcx, gcis, "generate_memory_chart", constants.COMPRESS_AND_DECOMPRESS['cmp_peak'], output_dir, "memory usage", compress_max_values["peak_comp"], plt)
        generate_chart(dcx, gcis, "generate_memory_chart", constants.COMPRESS_AND_DECOMPRESS['dcmp_peak'], output_dir, "memory usage", compress_max_values["peak_decomp"], plt)

def set_max_values(max_values, df):
    for key in max_values.keys():
        if key in df:
            column_max = df[key].max()
            if column_max > max_values[key]:
                max_values[key] = column_max
        elif key == "min_time":
            min_value = df['time'].min()
            max_values[key] = min_value if min_value < max_values[key] else max_values[key]

def set_mean_values(mean_values, df):
    for index, line in df.iterrows():
        for key in mean_values.keys():
            if line['algorithm'] == 'GCX':
                mean_values[key]['GCX'] += line[key] 
            elif line['algorithm'] == 'GCIS-ef':
                mean_values[key]['GCIS-ef'] += line[key]
            elif line['algorithm'] == 'GCIS-s8b':
                mean_values[key]['GCIS-s8b'] += line[key]
            else:
                mean_values[key]['GC*'] += line[key]
                

def prepare_dataset(df):
    plain_size = df['plain_size'][0]
    #calculate compression rate
    df['compressed_size'] = df['compressed_size'].apply(lambda x: ut.compute_ratio_percentage(x, plain_size))
    #convert bytes to MB
    df['peak_comp'] = df['peak_comp'].apply(lambda x: ut.bytes_to_mb(x))
    df['peak_decomp'] = df['peak_comp'].apply(lambda x:ut.bytes_to_mb(x))
    df['stack_comp'] = df['stack_comp'].apply(lambda x:ut.bytes_to_mb(x))
    df['stack_decomp'] = df['stack_comp'].apply(lambda x:ut.bytes_to_mb(x))

def get_data_frame(path, operation):
    files = glob.glob(f"{path}*.csv")
    df_list = []

    for file in files:
        df = pd.read_csv(file, sep='|', decimal=".")
        df.set_index('file', inplace=True)
        if operation == "compress":
            prepare_dataset(df)
            df['algorithm'] = df['algorithm'].str.replace('^DC','GC', regex=True)
            set_max_values(compress_max_values, df)
            set_mean_values(mean_values, df)
        elif operation == "extract":
            valid_algorithms = ['GCIS-ef', 'GCIS-s8b', 'DC32']
            df = df.query('algorithm in @valid_algorithms')
            df['algorithm'].replace({'DC32':'GCX'}, inplace=True)
            set_max_values(extract_values, df)
        df_list.append(df)

    if operation == "compress":
        size=len(df_list)
        for keys in mean_values.keys():
            mean_values[keys]['GCX'] = mean_values[keys]['GCX'] / size
            mean_values[keys]['GCIS-ef'] = mean_values[keys]['GCIS-ef'] / size
            mean_values[keys]['GCIS-s8b'] = mean_values[keys]['GCIS-s8b'] / size
            mean_values[keys]['GC*'] = mean_values[keys]['GC*'] / size
        print("\n\t------ Mean Values ------")
        print(json.dumps(mean_values, indent=4))

    return df_list

def main(argv):
    path = argv[1]
    operation = argv[3]
    language = argv[4]
    output_dir = f"{argv[2]}/{language}"

    constants = ut.set_locale(language)
    df_list = get_data_frame(path, operation)
    if operation == "compress":
        print("\n\t------ Compress and Decompress ------")
        generate_compress_chart(df_list, output_dir, constants)
    elif operation == "extract":
        print("\n\t------ Extract ------")
        generate_chart_line(df_list, output_dir, constants)

if __name__ == '__main__':
    sys.exit(main(sys.argv))