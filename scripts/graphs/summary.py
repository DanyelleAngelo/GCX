import json

compression_summary = {
    'peak_comp': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'REPAIR':0.0, 'GC-': 0.0},
    'peak_decomp': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'REPAIR':0.0, 'GC-': 0.0},
    'compression_time': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'REPAIR':0.0, 'GC-': 0.0},
    'decompression_time': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'REPAIR':0.0, 'GC-': 0.0},
    'compressed_size': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'REPAIR':0.0, 'GC-': 0.0}
}

extract_summary = {
    'time': {'GCX': 0.0, 'GCIS-ef': 0.0, 'GCIS-s8b': 0.0, 'REPAIR':0.0, 'GC-': 0.0}
}

def set_summary(values, df):
    for index, line in df.iterrows():
        for key in values.keys():
            if line['algorithm'] == 'GCX':
                values[key]['GCX'] += line[key] 
            elif line['algorithm'] == 'GCIS-ef':
                values[key]['GCIS-ef'] += line[key]
            elif line['algorithm'] == 'GCIS-s8b':
                values[key]['GCIS-s8b'] += line[key]
            elif line['algorithm'] == 'REPAIR':
                values[key]['REPAIR'] += line[key]
            elif 'PlainSlp' in line['algorithm']:
                values[key]['REPAIR'] += line[key]
            else:
                 values[key]['GC-'] += line[key]

def print_summary(df, summary):
    size=len(df)
    for keys in summary.keys():
        summary[keys]['GCX'] = summary[keys]['GCX'] / size
        summary[keys]['GCIS-ef'] = summary[keys]['GCIS-ef'] / size
        summary[keys]['GCIS-s8b'] = summary[keys]['GCIS-s8b'] / size
        summary[keys]['REPAIR'] = summary[keys]['REPAIR'] / size
        summary[keys]['GC-'] = summary[keys]['GC-'] / size/13
    print("\n\t------ Average Values  ------")
    print(json.dumps(summary, indent=4))