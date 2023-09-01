#!/bin/bash
GREEN='\033[0;32m'
BLUE='\033[34m'
RESET='\033[0m'
CURRENT_DATE=$(date +"%Y-%m-%d")

#url to download the files
PIZZA_URL="http://pizzachili.dcc.uchile.cl/repcorpus"
#directories
REPORT_DIR="../report"
PIZZA_DIR="../dataset/pizza_chilli"
COMPRESSED_DIR="../dataset/compressed_files"

GENERAL_REPORT="$REPORT_DIR/$CURRENT_DATE-general-report.csv"
FILE_PATHS=$(cat ../file_names.txt)
HEADER="file,coverage,peak_comp,stack_comp,compression_time,peak_decomp,stack_decomp,decompression_time,compressed_size,plain_size" 

check_and_create_folder() {
    echo -e "\n\n${GREEN}%%% Creating directories for files in case don't exist ${RESET}."
    if [ ! -d "$PIZZA_DIR" ]; then
        mkdir -p "$PIZZA_DIR"
    fi
    if [ ! -d "$COMPRESSED_DIR" ]; then
        mkdir -p "$COMPRESSED_DIR"
    fi
    if [ ! -d "$COMPRESSED_DIR/$CURRENT_DATE" ]; then
        mkdir -p "$COMPRESSED_DIR/$CURRENT_DATE"
    fi
    if [ ! -d "$REPORT_DIR" ]; then
        mkdir -p "$REPORT_DIR"
    fi
    if [ ! -d "$REPORT_DIR/$CURRENT_DATE" ]; then
        mkdir -p "$REPORT_DIR/$CURRENT_DATE"
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

validate_compression_and_decompression() {
    if ! cmp -s "$1" "$2"; then
        echo "$1 and $2 are different." >> $GENERAL_REPORT
    fi 
}

dcx_generate_report() {
    echo -e "\n${GREEN}%%% REPORT: Compresses the files from pizza_chilli, decompresses them, and compares the result with the original version${RESET}."

    make clean -C ../compressor/
    make compile -C ../compressor/
    coverageList=(3 4 5 6 7 8 9 15 30 60) 

    for plain_file in $FILE_PATHS; do
        IFS="/" read -ra file_name <<< "$plain_file"
        in_plain="$PIZZA_DIR/${file_name[1]}"
        report="$REPORT_DIR/$CURRENT_DATE/${file_name[1]}-dcx-encoding.csv"
        
        #create a file header
        echo $HEADER > $report; 

        for coverage in "${coverageList[@]}"; do
            echo -e "\n${BLUE}####### FILE: ${file_name[1]}, COVERAGE: ${coverage} ${RESET}"
            out_compressed="$COMPRESSED_DIR/$CURRENT_DATE/${file_name[1]}-coverage$coverage"
            out_descompressed=$out_compressed-plain
            #adding file name and coverage to the report
            echo -n "${file_name[1]}," >> $report 
            echo -n "$coverage," >> $report
            #perform compress
            ../compressor/./main $in_plain $out_compressed c $coverage $report
            #perform decompress
            ../compressor/./main $out_compressed.dcx $out_descompressed  d $coverage $report
            validate_compression_and_decompression "$in_plain" "$out_descompressed"
            #adding file size information to the report
            echo -n $(stat -c %s  "$out_compressed.dcx")"," >> $report
            echo  $(stat -c %s  "$in_plain") >> $report
        done
	output_file="$COMPRESSED_DIR/$CURRENT_DATE/${file_name[1]}"
	#CODEC = elias-fano
	./../external/gc-is-codec -c $in_plain "$output_file-gcis-ef" -ef
	./../external/gc-is-codec -d "$output_file-gcis-ef" "$output_file-gcis-ef-plain" -ef
	echo -n "${file_name[1]}-gcis-ef, 0, 0, 0, 0, 0,0, 0," >> $report
	echo -n $(stat -c %s "$output_file-gcis-ef")"," >> $report
	echo $(stat -c %s "$in_plain") >> $report
	#CODEC = Simple8b	
	./../external/gc-is-codec -c $in_plain "$output_file-gcis-s8b" -s8b	
	./../external/gc-is-codec -d "$output_file-gcis-s8b" "$output_file-gcis-s8b-plain" -s8b
	echo -n "${file_name[1]}-gcis-s8b, 0, 0, 0, 0, 0,0, 0," >> $report
	echo -n $(stat -c %s "$output_file-gcis-s8b")"," >> $report
	echo $(stat -c %s "$in_plain") >> $report
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
    dcx_generate_report
#   python_setup_and_generate_graphs
fi
