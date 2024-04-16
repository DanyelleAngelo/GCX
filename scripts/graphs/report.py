import sys
import os
import plotting as plt
import glob
import pandas as pd
import json
import utils as ut

compress_max_values = {
    'peak_comp': 0.0,
    'peak_decomp': 0.0,
    'compression_time': 0.0,
    'decompression_time': 0.0,
    'compressed_size': 0.0
}

mean_values = {
    'peak_comp': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0},
    'peak_decomp': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0},
    'compression_time': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0},
    'decompression_time': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0},
    'compressed_size': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0}
}

extract_values = {
    'time': 0.0,
    "min_time": 0.0
}

def generate_extract_chart(df_list, output_dir, language):
    for df in df_list:
        print(f"\n## FILE: {df.index[0]}")
        print(f"\t- Creating charts extract comparison between GCX and GCIS")
        plt.generate_chart_line(df, language.EXTRACT["time"], output_dir, extract_values["time"], extract_values["min_time"])

def generate_compress_chart(df_list, output_dir, language):
    for df in df_list:
        gcis_filter = df['algorithm'].str.contains('GCIS') 
        repair_filter = df['algorithm'].str.contains('REPAIR') 
        combined_filter = gcis_filter | repair_filter

        others = df[combined_filter]
        dcx = df[~combined_filter]
       
        print(f"\n## FILE: {df.index[0]}")
        # plt.generate_chart_bar(dcx, others, language.COMPRESS_AND_DECOMPRESS['cmp_time'], output_dir)
        # plt.generate_chart_bar(dcx, others, language.COMPRESS_AND_DECOMPRESS['dcmp_time'], output_dir)

        # plt.generate_chart_bar(dcx, others, language.COMPRESS_AND_DECOMPRESS['ratio'], output_dir, 100)

        plt.generate_chart_bar(dcx, others, language.COMPRESS_AND_DECOMPRESS['cmp_peak'], output_dir, compress_max_values["peak_comp"])
        plt.generate_chart_bar(dcx, others, language.COMPRESS_AND_DECOMPRESS['dcmp_peak'], output_dir, compress_max_values["peak_decomp"])

def print_report_summary(df):
    size=len(df)
    for keys in mean_values.keys():
        mean_values[keys]['GCX'] = mean_values[keys]['GCX'] / size
        mean_values[keys]['GCIS-ef'] = mean_values[keys]['GCIS-ef'] / size
        mean_values[keys]['GCIS-s8b'] = mean_values[keys]['GCIS-s8b'] / size
    print("\n\t------ Average Values  ------")
    print(json.dumps(mean_values, indent=4))

def set_max_values(values, df):
    for key in values.keys():
        if key in df:
            column_max = df[key].max()
            if column_max > values[key]:
                values[key] = column_max

def set_mean_values(values, df):
    for index, line in df.iterrows():
        for key in values.keys():
            if line['algorithm'] == 'GCX':
                values[key]['GCX'] += line[key] 
            elif line['algorithm'] == 'GCIS-ef':
                values[key]['GCIS-ef'] += line[key]
            elif line['algorithm'] == 'GCIS-s8b':
                values[key]['GCIS-s8b'] += line[key]

def prepare_dataset(df):
    plain_size = df['plain_size'][0]
    #calculate compression rate
    df['compressed_size'] = df['compressed_size'].apply(lambda x: ut.compute_ratio_percentage(x, plain_size))
    
    #convert bytes to MB
    df['peak_comp'] = df['peak_comp'].apply(lambda x: ut.bytes_to_mb(x))
    df['peak_decomp'] = df['peak_comp'].apply(lambda x:ut.bytes_to_mb(x))
    df['stack_comp'] = df['stack_comp'].apply(lambda x:ut.bytes_to_mb(x))
    df['stack_decomp'] = df['stack_comp'].apply(lambda x:ut.bytes_to_mb(x))

def get_data_frame(path, operation, report):
    files = glob.glob(f"{path}*.csv")
    df_list = []

    for file in files:
        df = pd.read_csv(file, sep='|', decimal=".")
        df.set_index('file', inplace=True)

        if operation == "compress":
            prepare_dataset(df)
            set_max_values(compress_max_values, df)
            set_mean_values(mean_values, df)
        elif operation == "extract":
            set_max_values(extract_values, df)
        df_list.append(df)

    if operation == "compress" and report:
        print_report_summary(df)

    return df_list

def main(argv):
    path = argv[1]
    operation = argv[3]
    locale = argv[4]
    report =  argv[5] if len(sys.argv) > 5 else False
    output_dir = f"{argv[2]}/{locale}"

    language = ut.set_locale(locale)
    df_list = get_data_frame(path, operation, report)
    
    if operation == "compress":
        print("\n\t------ Compress and Decompress ------")
        generate_compress_chart(df_list, output_dir, language)
    elif operation == "extract":
        print("\n\t------ Extract ------")
        generate_extract_chart(df_list, output_dir, language)

if __name__ == '__main__':
    sys.exit(main(sys.argv))