#!/bin/bash
REPORT_DIR="../report"

source validate_compress_decompress.sh

if [ ! -d "$REPORT_DIR" ]; then
    mkdir -p "$REPORT_DIR"
fi

compress_decompress_and_generate_report() {
    echo -e "\n${GREEN}%%% REPORT: Compresses the files from pizza_chilli, decompresses them, and compares the result with the original downloaded version${RESET}."

    make clean -C ../compressor/
    make compile -C ../compressor/

    coverageList=(3 4 5 6 7 8 9 15 30 60) 

    for coverage in "${coverageList[@]}"; do
        report="$REPORT_DIR/coverage$coverage-dcx-encoding.csv"
        if [ -e "$report" ]; then 
            rm -f $report; 
            echo "file,compressed_file,compression_time,decompression_time" >> $report; 
        fi
§w§s
        for plain_file in $FILE_PATHS; do
                IFS="/" read -ra file_name <<< "$plain_file"
                in_plain="$PIZZA_DIR/${file_name[1]}"
                out_compressed="$COMPRESSED_DIR/${file_name[1]}-coverage$coverage"
                out_descompressed=$out_compressed-plain

                echo -e "\n${BLUE}####### FILE: ${file_name[1]} coverage SIZE: $coverage ${RESET}"
                #compress
                ../compressor/./main $in_plain $out_compressed c $coverage > output.txt
                echo -n "$in_plain," >> $report 
                echo -n "$out_compressed," >> $report 
                echo -n "$(tail -n 1 output.txt)," >> $report
                #decompress
                ../compressor/./main $out_compressed.dcx $out_descompressed  d $coverage > output.txt
                echo "$(tail -n 1 output.txt)" >> $report
                rm output.txt
        done
        make clean -C ../compressor/
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
#python_setup_and_generate_graphs
fi