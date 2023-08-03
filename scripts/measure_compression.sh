#!/bin/bash
old_ifs="$IFS"
GREEN='\033[0;32m'
BLUE='\033[34m'
RESET='\033[0m'

PIZZA_DIR="dataset/pizza_chilli"
COMPRESSED_DIR="dataset/compressed_files"
FILE_PATHS=$(cat file_names.txt)
PIZZA_URL="http://pizzachili.dcc.uchile.cl/repcorpus"

echo -e "\n\n${GREEN}%%% Creating directories for files in case don't exist ${RESET}."
if [ ! -d "$PIZZA_DIR" ]; then
    mkdir -p "$PIZZA_DIR"
fi
if [ ! -d "$COMPRESSED_DIR" ]; then
    mkdir -p "$COMPRESSED_DIR"
fi
if [ ! -d "report" ]; then
    mkdir -p "report"
fi

(
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
)

if  [ ! -e  "compressor/main" ]; then
    echo -e "\n${GREEN}%%% Compile files to generate a grammar with rules stored as integer ${RESET}."
    make compile CODEC=int -C compressor/
fi

(
echo -e "\n${GREEN}%%% Compresses the files from pizza_chilli, decompresses them, and compares the result with the original downloaded version.${RESET}."

for plain_file in $FILE_PATHS; do
    IFS="/" read -ra file_name <<< "$plain_file"
    in_plain="../$PIZZA_DIR/${file_name[1]}"
    out_compressed="../$COMPRESSED_DIR/${file_name[1]}-r3-int"
    out_descompressed=$out_compressed-plain
    file_report="report/compressed-rules3-encodedInteger.csv"

    if [ -e "$file_report" ]; then
        #para gerarmos um relatório novo
        rm $file_report
    fi

    echo -e "\n${BLUE}####### FILE: ${file_name[1]} ${RESET}"
    #compress
    make run_compressor MODE=e CODEC=int RULES=3 IN_PLAIN_TEXT_FILE=$in_plain COMPRESSED_FILE=$out_compressed -C compressor/
    #decompress
    make run_compressor MODE=d CODEC=int RULES=3 IN_PLAIN_TEXT_FILE=$in_plain COMPRESSED_FILE=$out_compressed OUT_PLAIN_TEXT_FILE=$out_descompressed -C compressor/
    break
done
)

make clean -C compressor/