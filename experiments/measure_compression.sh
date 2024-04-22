#!/bin/bash
source utils.sh

STR_LEN=(1 10 100 1000 10000)

COMPRESSION_HEADER="file|algorithm|peak_comp|stack_comp|compression_time|peak_decomp|stack_decomp|decompression_time|compressed_size|plain_size"
EXTRACTION_HEADER="file|algorithm|peak|stack|time|substring_size"
GCIS_EXECUTABLE="../../GCIS/build/src/./gc-is-codec"
REPAIR_EXECUTABLE="../../GCIS/external/repair/build/src"
EXTRACT_ENCODING=("PlainSlp_32Fblc"  "PlainSlp_FblcFblc") #"PlainSlp_IblcFblc" "PoSlp_Iblc" "PoSlp_Sd")
#set -x

compress_and_decompress_with_gcis() {
	CODEC=$1
	PLAIN=$2
	REPORT=$3
	FILE_NAME=$4
	OUTPUT="$COMP_DIR/$CURR_DATE/$FILE_NAME"
	echo -n "$FILE_NAME|GCIS-${CODEC}|" >> $report
	"$GCIS_EXECUTABLE" -c "$PLAIN" "$OUTPUT-gcis-$CODEC" "-$CODEC" "$REPORT"
	"$GCIS_EXECUTABLE" -d "$OUTPUT-gcis-$CODEC" "$OUTPUT-gcis-$CODEC-plain" "-$CODEC" "$REPORT"
	echo "$(stat $stat_options $OUTPUT-gcis-$CODEC)|$5" >> $REPORT

	checks_equality "$PLAIN" "$OUTPUT-gcis-$CODEC-plain" "gcis"
}

compress_and_decompress_with_repair() {
	FILE=$1
	REPORT=$2
	FILE_NAME=$3
	OUTPUT="$COMP_DIR/$CURR_DATE/$FILE_NAME"
	echo -n "$FILE_NAME|REPAIR|" >> $report
	"${REPAIR_EXECUTABLE}/./repair" -i "$FILE" "$REPORT"
	"${REPAIR_EXECUTABLE}/./despair" -i "${FILE}" "$REPORT"
	echo "$(stat $stat_options $FILE.prel)|$4" >> $REPORT

	checks_equality "$FILE" "$FILE.u" "gcis"
}

compress_and_decompress_with_gcx() {
	echo -e "\n${GREEN}%%% REPORT: Compresses the files, decompresses them, and compares the result with the original version${RESET}."

	make clean -C ../compressor/
	make compile MACROS="REPORT=1" -C ../compressor/

	for file in $files; do
		report="$REPORT_DIR/$CURR_DATE/$file-gcx-encoding.csv"
		echo $COMPRESSION_HEADER > $report; 
		plain_file_path="$RAW_FILES_DIR/$file"
		size_plain=$(stat $stat_options $plain_file_path)

		echo -e "\n\t${BLUE}####### FILE: $file ${RESET}"

	#perform compress and decompress with GCX
	echo -e "\n\t\t ${YELLOW}Starting compression/decompression using GCX ${RESET}\n"
	echo -n "$file|GCX|" >> $report
	file_out="$COMP_DIR/$CURR_DATE/$file"
	../compressor/./main -c $plain_file_path $file_out $report
	../compressor/./main -d $file_out.gcx $file_out-plain $report
	checks_equality "$plain_file_path" "$file_out-plain" "gcx"
	echo "$(stat $stat_options $file_out.gcx)|$size_plain" >> $report

	#perform compress and decompress with GCIS
	echo -e "\n\t\t ${YELLOW}Starting compression/decompression using GCIS ${RESET}\n"
	compress_and_decompress_with_gcis "ef" "$plain_file_path" "$report" "$file" "$size_plain"
	compress_and_decompress_with_gcis "s8b" "$plain_file_path" "$report" "$file" "$size_plain"

	#perform compress and decompress with REPAIR
	echo -e "\n\t\t ${YELLOW}Starting compression/decompression using REPAIR ${RESET}\n"
	compress_and_decompress_with_repair "$plain_file_path" "$report" "$file" "$size_plain"
	echo -e "\n\t ${YELLOW}Finishing compression/decompression operations on the $file file. ${RESET}\n"
done
make clean -C ../compressor/
}

run_extract() {
	#make clean -C ../compressor/
	#make compile MACROS="REPORT=1 FILE_OUTPUT=1" -C ../compressor/

	echo -e "\n${BLUE}####### Extract validation ${RESET}"
	for file in $files; do
		echo -e "\n\t${BLUE}Starting extract operation on the $file file. ${RESET}\n"

		plain_file_path="$RAW_FILES_DIR/$file"
		extract_dir="$REPORT_DIR/$CURR_DATE/extract"
		compressed_file="$COMP_DIR/$CURR_DATE/$file"

		report="$REPORT_DIR/$CURR_DATE/$file-gcx-extract.csv"
		echo $EXTRACTION_HEADER > $report;

		echo -e "\n${YELLOW} Starting encode with repair-navarro - $file .${RESET}"
		if [ ! -f "$plain_file_path.C" ]; then
			"../../GCIS/external/repair-navarro/./repair" "$plain_file_path"
		fi

		echo -e "\n${YELLOW} Generating encodes with SLP...${RESET}"
		for encoding in "${EXTRACT_ENCODING[@]}"; do
			if [ ! -f "$plain_file_path-$encoding" ]; then
				"../../ShapedSlp/build/./SlpEncBuild" -i $plain_file_path -o "$plain_file_path-$encoding" -e $encoding -f NavarroRepair
			fi
		done
		#generates intervals
		echo -e "\n${YELLOW} Generating search intervals... ${RESET}"
		python3 generate_extract_input.py "$plain_file_path" "$extract_dir/$file"
		#perform extracting
		for length in "${STR_LEN[@]}"; do
			query="$extract_dir/${file}.${length}_extract"
			if [ -e $query ]; then
				echo -e "\n${YELLOW} Generating expected responses for searched interval...${RESET}"
				#extract_answer="$extract_dir/${file}_${length}_substrings_expected_response.txt"
				#python3 ../scripts/extract.py $plain_file_path $extract_answer $query

				echo -e "\n\t ${YELLOW}Starting extract with GCX - $file - INTERVAL SIZE $length.${RESET}"
				echo -n "$file|GCX|" >> $report
				extract_output="$extract_dir/${file}_${length}_substrings_results.txt"
				../compressor/./main -e "$compressed_file.gcx" $extract_output $query $report
				echo "$length" >> $report
				#checks_equality "$extract_output" "$extract_answer" "extract"
				#rm $extract_output
				#rm $extract_answer

				echo -e "\n${YELLOW}Starting extract with GCIS - $file - INTERVAL SIZE $length.${RESET}"
				echo -n "$file|GCIS-ef|" >> $report
				$GCIS_EXECUTABLE -e "$compressed_file-gcis-ef" $query -ef $report
				echo "$length" >> $report

				echo -e "\n${YELLOW} Starting extract with ShapedSlp - $file - INTERVAL SIZE $length.${RESET}"
				for encoding in "${EXTRACT_ENCODING[@]}"; do
					echo -n "$file|$encoding|" >> $report
					"../../ShapedSlp/build/./ExtractBenchmark" --input="$plain_file_path-$encoding" --encoding=$encoding --query_file=$query --file_report_gcx=$report
					echo "$length" >> $report
				done
			else
				echo " arquivo nao gerado $query"
			fi
		done
	done
}

generate_graphs() {
	echo -e "\n\n${GREEN}%%% Starting the generation of the graphs. ${RESET}"
	CURR_DATE="2024-04-10"
	python3 ../scripts/graphs/report.py "$REPORT_DIR/$CURR_DATE/*-gcx-encoding" "$REPORT_DIR/$CURR_DATE/graphs" "compress" "en" "report"
	#python3 ../scripts/graphs/report.py "$REPORT_DIR/$CURR_DATE/*-gcx-extract" "$REPORT_DIR/$CURR_DATE/graphs" "extract" "en" "report"
	echo -e "\n\n${GREEN}%%% FINISHED. ${RESET}"
}

if [ "$0" = "$BASH_SOURCE" ]; then
	#check_and_create_folder
	#download_files
	#compress_and_decompress_with_gcx()
	#run_extract
	generate_graphs
fi
