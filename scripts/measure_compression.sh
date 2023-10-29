#!/bin/bash
source utils.sh

COV_LIST=(32 64 128)
STR_LEN=(1 10 100 1000 10000)

HEADER="file|algorithm|peak_comp|stack_comp|compression_time|peak_decomp|stack_decomp|decompression_time|compressed_size|plain_size"
GCIS_EXECUTABLE="../../GCIS/build/src/./gc-is-codec"
#set -x
CURR_DATE="2023-10-28"
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
    #size_file=$(echo "scale=2; $size_file / (1024 * 1024)" | bc)
	echo "$size_file|$SIZE_PLAIN" >> $REPORT
}

compress_and_decompress_with_dcx() {
    echo -e "\n${GREEN}%%% REPORT: Compresses the files from pizza_chilli, decompresses them, and compares the result with the original version${RESET}."

    make clean -C ../compressor/
    make compile MACROS="REPORT=1" -C ../compressor/

    for file_path in $FILES; do
        IFS="/" read -ra file_name <<< "$file_path"
        file="${file_name[1]}"

        report="$REPORT_DIR/$CURR_DATE/$file-dcx-encoding.csv"
        echo $HEADER > $report; 

        plain_file_path="$PIZZA_DIR/$file"
        size_plain=$(stat $stat_options $plain_file_path)
        #size_plain=$(echo "scale=2; $size_plain / (1024 * 1024)" | bc)

        for cover in "${COV_LIST[@]}"; do
            echo -e "\n${BLUE}####### FILE: $file, COVERAGE: ${cover} ${RESET}"

            file_out="$COMP_DIR/$CURR_DATE/$file-dc$cover"

            #adding file name and coverage to the report
            echo -n "$file|DC$cover|" >> $report
            #perform compress and decompress
            ../compressor/./main $plain_file_path $file_out c $cover $report
            ../compressor/./main $file_out.dcx $file_out-plain d $cover $report

            #validate compressed file
            validate_compression_and_decompression "$plain_file_path" "$file_out-plain"

            #adding file size information to the report
            size_file=$(stat $stat_options $file_out.dcx)
            #size_file=$(echo "scale=2; $size_file / (1024 * 1024)" | bc)
            echo "$size_file|$size_plain" >> $report
        done

	    #compresses and decompresses the file using GCIS
        compress_and_decompress_with_gcis "ef" "$plain_file_path" "$report" "$file" "$size_plain"
        compress_and_decompress_with_gcis "s8b" "$plain_file_path" "$report" "$file" "$size_plain"
    done
    make clean -C ../compressor/
}

run_extract() {
    make clean -C ../compressor/
    make compile MACROS="REPORT=1 FILE_OUTPUT=1" -C ../compressor/

    echo -e "\n${BLUE}####### Extract validation ${RESET}"
    for file_path in $FILES; do
        IFS="/" read -ra file_name <<< "$file_path"
        file="${file_name[1]}"

        plain_file_path="$PIZZA_DIR/$file"
        extract_dir="$REPORT_DIR/$CURR_DATE/extract"
        compressed_file="$COMP_DIR/$CURR_DATE/$file"

        report="$REPORT_DIR/$CURR_DATE/$file-dcx-extract.csv"
        echo "file|algorithm|peak|stack|time|substring_size" > $report;

        #generates intervals
        python3 ../../GCIS/scripts/generate_extract_input.py "$plain_file_path" "$extract_dir/$file"
        for length in "${STR_LEN[@]}"; do
            query="$extract_dir/$file.${length}_query"

            #generates valid response based on plain text
            extract_answer="$extract_dir/${file}_extract_answer_len$length.txt"
            python3 ../utils/extract.py $plain_file_path $extract_answer $query

            #collect metrics from GCIS execution
            echo -e "\n\t${BLUE} GCIS - INTERVAL SIZE $length ${RESET}"
            echo -n "$file|GCIS-ef|" >> $report
            $GCIS_EXECUTABLE -e "$compressed_file-gcis-ef" $query -ef $report
            echo "$length" >> $report

            for cover in "${COV_LIST[@]}"; do
                #collect metrics from DCX execution
                echo -e "\n\t${BLUE} DCX - INTERVAL SIZE $length, INITIAL COVERAGE $cover ${RESET}"
                echo -n "$file|DC$cover|" >> $report
                extract_output="$extract_dir/${file}_result_extract_dc${cover}_len${length}.txt"
                ../compressor/./main "$compressed_file-dc$cover.dcx" $extract_output e $cover $query $report
                echo "$length" >> $report

                #checks equality
                if ! diff -s $extract_output $extract_answer; then
                    echo "** Extract ** Erro ao extrair strings de tamanho $length, para o arquivo $file com tamanho de regra para o DCX igual à $cover" >> "$GENERAL_REPORT/errors/errors.txt"
      		fi
                rm $extract_output
            done
            rm $extract_answer
        done
    done
}

generate_graphs() {
    echo -e "\n\n${GREEN}%%% Starting the generation of the graphs. ${RESET}"

    for file_path in $FILES; do
        IFS="/" read -ra file_name <<< "$file_path"
        file="${file_name[1]}"

        report_compress="$REPORT_DIR/$CURR_DATE/$file-dcx-encoding.csv"
        report_extract="$REPORT_DIR/$CURR_DATE/$file-dcx-extract.csv"

        echo -e "\n\tGenerate graphs for $file"

        python3 report.py "$report_compress" "$REPORT_DIR/$CURR_DATE/graphs" "compress"
        python3 report.py "$report_extract" "$REPORT_DIR/$CURR_DATE/graphs" "extract"
    done
    echo -e "\n\n${GREEN}%%% FINISHED. ${RESET}"
}

if [ "$0" = "$BASH_SOURCE" ]; then
    check_and_create_folder
#    download_files
    compress_and_decompress_with_dcx
    run_extract
#    generate_graphs
fi
