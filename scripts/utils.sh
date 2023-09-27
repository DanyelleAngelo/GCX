#!/bin/bash

#constants
GREEN='\033[0;32m'
BLUE='\033[34m'
RESET='\033[0m'
CURR_DATE=$(date +"%Y-%m-%d")

#files to compress
FILES=$(cat ../file_names.txt)

#directories
REPORT_DIR="../report"
COMP_DIR="../dataset/compressed_files"
PIZZA_DIR="../dataset/pizza_chilli"

if [ "$(uname -s)" = "Darwin" ]; then
    stat_options="-f %z"
    CURR_DATE="$CURR_DATE-mac"
else
    stat_options="-c %s"
fi

GENERAL_REPORT="$REPORT_DIR/$CURR_DATE"

check_and_create_folder() {
    echo -e "\n\n${GREEN}%%% Creating directories for files in case don't exist ${RESET}."
    if [ ! -d "$PIZZA_DIR" ]; then
        mkdir -p "$PIZZA_DIR"
    fi
    if [ ! -d "$COMP_DIR" ]; then
        mkdir -p "$COMP_DIR"
    fi
    if [ ! -d "$COMP_DIR/$CURR_DATE" ]; then
        mkdir -p "$COMP_DIR/$CURR_DATE"
    fi
    if [ ! -d "$REPORT_DIR" ]; then
        mkdir -p "$REPORT_DIR"
    fi
    if [ ! -d "$REPORT_DIR/$CURR_DATE" ]; then
        mkdir -p "$REPORT_DIR/$CURR_DATE/extract"
        mkdir -p "$REPORT_DIR/$CURR_DATE/graphs"
        mkdir -p "$REPORT_DIR/$CURR_DATE/errors"
    fi
}

download_files() {
    PIZZA_URL="http://pizzachili.dcc.uchile.cl/repcorpus"

    echo -e "\n${GREEN}%%% Download files from a list, then descompress the files and remove the compressed files.${RESET}."

    for file in $FILES; do
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
        echo "$1 and $2 are different." >> "$GENERAL_REPORT/errors/errors-compress.txt"
    fi 
}