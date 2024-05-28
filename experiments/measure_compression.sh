#!/bin/bash
source utils.sh

COV_LIST=(3 4 5 6 7 8 9 11 15 30 32 60 64)
STR_LEN=(1 10 100 1000 10000)

HEADER="file|algorithm|peak_comp|stack_comp|compression_time|peak_decomp|stack_decomp|decompression_time|compressed_size|plain_size"
GCIS_EXECUTABLE="../../GCIS/build/src/./gc-is-codec"
#set -x

compress_and_decompress_with_gcis() {
    CODEC=$1
    PLAIN=$2
    REPORT=$3
    FILE_NAME=$4
    SIZE_PLAIN=$5
    OUTPUT="$COMP_DIR/$CURR_DATE/$FILE_NAME"
	echo -n "$FILE_NAME|GCIS-${CODEC}|" >> $report
    "$GCIS_EXECUTABLE" -c "$PLAIN" "$OUTPUT-gcis-$CODEC" "-$CODEC" "$REPORT"
	"$GCIS_EXECUTABLE" -d "$OUTPUT-gcis-$CODEC" "$OUTPUT-gcis-$CODEC-plain" "-$CODEC" "$REPORT"
    size_file=$(stat "$stat_options" "$OUTPUT-gcis-$CODEC")
	echo "$size_file|$SIZE_PLAIN" >> $REPORT

    echo -e "\n\t\t ${YELLOW} Checking if the decoded file is the same as the original ${RESET}\n"
    checks_equality "$PLAIN" "$OUTPUT-gcis-$CODEC-plain" "gcis"
}

compress_and_decompress_with_gcx() {
    echo -e "\n${GREEN}%%% REPORT: Compresses the files, decompresses them, and compares the result with the original version${RESET}."

    make clean -C ../compressor/
    make compile MACROS="REPORT=1" -C ../compressor/

    for file in $files; do
        report="$REPORT_DIR/$CURR_DATE/$file-gcx-encoding.csv"
        echo $HEADER > $report; 

        plain_file_path="$RAW_FILES_DIR/$file"
        size_plain=$(stat $stat_options $plain_file_path)

        echo -e "\n ${YELLOW}Starting compression/decompression operations on the $file file. ${RESET}\n"
        echo -e "\n\t ${YELLOW}Starting compression/decompression using gcX ${RESET}\n"
        for cover in "${COV_LIST[@]}"; do
            echo -e "\n\t${BLUE}####### FILE: $file, COVERAGE: ${cover} ${RESET}"
            #adding file name and coverage to the report
            echo -n "$file|GC$cover|" >> $report

            file_out="$COMP_DIR/$CURR_DATE/$file-gc$cover"
            #perform compress and decompress
	    ../compressor/./main $plain_file_path $file_out c $cover $report
 	    ../compressor/./main $file_out.gcx $file_out-plain d $cover $report

            echo -e "\n\t\t ${YELLOW} Checking if the decoded file is the same as the original ${RESET}\n"
            checks_equality "$plain_file_path" "$file_out-plain" "gcx"

            #adding file size information to the report
            size_file=$(stat $stat_options $file_out.gcx)
            echo "$size_file|$size_plain" >> $report
        done

        echo -e "\n\t ${YELLOW}Starting compression/decompression using GCIS ${RESET}\n"
        compress_and_decompress_with_gcis "ef" "$plain_file_path" "$report" "$file" "$size_plain"
        compress_and_decompress_with_gcis "s8b" "$plain_file_path" "$report" "$file" "$size_plain"
        
        echo -e "\n\t ${YELLOW}Finishing compression/decompression operations on the $file file. ${RESET}\n" =
    done
    make clean -C ../compressor/
}

run_extract() {
    make clean -C ../compressor/
    make compile MACROS="REPORT=1 FILE_OUTPUT=1" -C ../compressor/

    if [ ${#compressed_success_files[@]} -eq 0 ]; then
        compressed_success_files="$files"
    fi

    echo -e "\n${BLUE}####### Extract validation ${RESET}"
    for file in $compressed_success_files; do
        echo -e "\n ${BLUE}Starting extract operation on the $file file. ${RESET}\n"
    
        plain_file_path="$RAW_FILES_DIR/$file"
        extract_dir="$REPORT_DIR/$CURR_DATE/extract"
        compressed_file="$COMP_DIR/$CURR_DATE/$file"

        report="$REPORT_DIR/$CURR_DATE/$file-gcx-extract.csv"
        echo "file|algorithm|peak|stack|time|substring_size" > $report;

        #generates intervals
        echo -e "\n\t${YELLOW} Generating search intervals... ${RESET}"
        python3 ../../GCIS/scripts/generate_extract_input.py "$plain_file_path" "$extract_dir/$file"
        for length in "${STR_LEN[@]}"; do
            query="$extract_dir/$file.${length}_query"

            echo -e "\n\t${YELLOW} Generating responses for searched interval...${RESET}"
            extract_answer="$extract_dir/${file}_extract_answer_len$length.txt"
            python3 ../scripts/extract.py $plain_file_path $extract_answer $query

            echo -e "\n\t ${YELLOW}Starting extract with GCIS - INTERVAL SIZE $length.${RESET}"
            echo -n "$file|GCIS-ef|" >> $report
            $GCIS_EXECUTABLE -e "$compressed_file-gcis-ef" $query -ef $report
            echo "$length" >> $report

            echo -e "\n\t ${YELLOW}Starting extract with gcX - INTERVAL SIZE $length.${RESET}"
            for cover in "${COV_LIST[@]}"; do
                echo -n "$file|GC$cover|" >> $report
                extract_output="$extract_dir/${file}_result_extract_gc${cover}_len${length}.txt"
                ../compressor/./main "$compressed_file-gc$cover.gcx" $extract_output e $cover $query $report
                echo "$length" >> $report

                echo -e "\n\t\t ${YELLOW} Checking if the extracting substrings from compressed text was successful. ${RESET}\n"
                checks_equality "$extract_output" "$extract_answer" "extract"
                rm $extract_output
            done
            rm $extract_answer
        done
    done
}

generate_graphs() {
    echo -e "\n\n${GREEN}%%% Starting the generation of the graphs. ${RESET}"
    python3 ../scripts/report_en.py "$REPORT_DIR/$CURR_DATE/*-gcx-encoding" "$REPORT_DIR/$CURR_DATE/graphs" "compress" "en"
    #python3 ../scripts/report.py "$REPORT_DIR/$CURR_DATE/*-gcx-extract" "$REPORT_DIR/$CURR_DATE/graphs" "extract" "pt"
    echo -e "\n\n${GREEN}%%% FINISHED. ${RESET}"
}

if [ "$0" = "$BASH_SOURCE" ]; then
    check_and_create_folder
    download_files
    compress_and_decompress_with_gcx
    # run_extract
    #generate_graphs
fi
