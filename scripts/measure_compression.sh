#!/bin/bash
REPORT_DIR="../report"

GREEN='\033[0;32m'
BLUE='\033[34m'
RESET='\033[0m'

PIZZA_DIR="../dataset/pizza_chilli"
COMPRESSED_DIR="../dataset/compressed_files"
FILE_PATHS=$(cat ../file_names.txt)
PIZZA_URL="http://pizzachili.dcc.uchile.cl/repcorpus"
EXECUTABLE="../compressor/./main"

formatted_date=$(date +"%Y-%m-%d_%H-%M-%S")

check_and_create_folder() {
    echo -e "\n\n${GREEN}%%% Creating directories for files in case don't exist ${RESET}."
    if [ ! -d "$PIZZA_DIR" ]; then
        mkdir -p "$PIZZA_DIR"
    fi
    if [ ! -d "$COMPRESSED_DIR" ]; then
        mkdir -p "$COMPRESSED_DIR"
    fi
    if [ ! -d "$REPORT_DIR" ]; then
        mkdir -p "$REPORT_DIR"
    fi
}

download_files() {
    echo -e "\n${GREEN}%%% Download files from a list, then descompress the files and remove the compressed files.${RESET}."

    for file in $FILE_PATHS; do
        IFS="/" read -ra file_name <<< "$file"
        compressed_files="$PIZZA_DIR/${file_name[1]}.gz"
        descompressed_file="$PIZZA_DIR/${file_name[1]}"

        if [ ! -e "$compressed_files" ] && [ ! -e "$descompressed_file" ]; then
            echo -e "\n${BLUE}  % $PIZZA_URL/$file.gz ${RESET}"
            curl --output "$compressed_files" "$PIZZA_URL/$file.gz"
            gzip -d "$compressed_files"
        elif [ ! -e "$descompressed_file" ]; then
            echo -e "\n${BLUE}  % $PIZZA_URL/$file.gz ${RESET}"
            gzip -d "$compressed_files"
        fi
    done
}

compress_decompress_and_generate_report() {
    echo -e "\n${GREEN}%%% REPORT: Compresses the files from pizza_chilli, decompresses them, and compares the result with the original downloaded version${RESET}."

    make clean -C ../compressor/
    make compile -C ../compressor/
    general_report="$REPORT_DIR/$formatted_date-general-report.csv"
    coverageList=(3 4 5 6 7 8 9 15 30 60) 

    for plain_file in $FILE_PATHS; do
        IFS="/" read -ra file_name <<< "$plain_file"
        in_plain="$PIZZA_DIR/${file_name[1]}"
        report="$REPORT_DIR/$formatted_date-${file_name[1]}-dcx-encoding.csv"

        echo "file,coverage,compression_time,decompression_time,memory_usage,compressed_size,plain_size" >> $report; 

        for coverage in "${coverageList[@]}"; do
            out_compressed="$COMPRESSED_DIR/${file_name[1]}-coverage$coverage"
            out_descompressed=$out_compressed-plain
            echo -e "\n${BLUE}####### FILE: ${file_name[1]} coverage SIZE: $coverage ${RESET}"
            #compress
            ../compressor/./main $in_plain $out_compressed c $coverage > output.txt
            compression_time="$(tail -n 1 output.txt)"
            #decompress
            ../compressor/./main $out_compressed.dcx $out_descompressed  d $coverage > output.txt
            decompression_time="$(tail -n 1 output.txt)"
            #validate
            if ! cmp -s "$in_plain" "$out_descompressed"; then
                echo "$in_plain and $out_descompressed are different." >> $general_report
            fi 
            #report
            echo -n "${file_name[1]}," >> $report 
            echo -n "$coverage," >> $report
            echo -n "$compression_time," >> $report
            echo -n "$decompression_time," >> $report
            if [[ "$OSTYPE" == "darwin"* ]]; then
                echo -n "1," >> $report
            else
                echo -n "0," >> $report
            fi
            echo -n $(stat -f%z  "$out_compressed.dcx")"," >> $report
            echo $(stat -f%z  "$in_plain") >> $report
            rm output.txt
        done
    done
    make clean -C ../compressor/
}

python_setup_and_generate_graphs() {
    libraries="pandas matplotlib"
    for lib in $libraries; do
        if ! python3 -c "import $lib" &> /dev/null; then
            echo -e "\n${GREEN}%%%Installing $lib library${RESET}."
            pip3 install $lib
        fi
    done

    echo -e "\n${GREEN}%%% Generates comparison charts between GCIS and DCX.${RESET}\n"
    python3 report.py ../$report ../gcis_result.csv
}


if [ "$0" = "$BASH_SOURCE" ]; then
    check_and_create_folder
    download_files
    compress_decompress_and_generate_report
#   python_setup_and_generate_graphs
fi