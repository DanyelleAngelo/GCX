#!/bin/bash

#constants
GREEN='\033[0;32m'
BLUE='\033[34m'
RESET='\033[0m'
CURR_DATE=$(date +"%Y-%m-%d")

#files to compress
FILE_URLS= ($(cat files_paths/repetitive_text.txt) $(cat files_paths/regular_text.txt))
FILES= ()

#directories
REPORT_DIR="../report"
COMP_DIR="../dataset/compressed_files"
RAW_FILES_DIR="../dataset/raw_files"

if [ "$(uname -s)" = "Darwin" ]; then
    stat_options="-f %z"
    CURR_DATE="$CURR_DATE-mac"
else
    stat_options="-c %s"
fi

GENERAL_REPORT="$REPORT_DIR/$CURR_DATE"

check_and_create_folder() {
    echo -e "\n\n${GREEN}%%% Creating directories for files in case don't exist ${RESET}."
    if [ ! -d "$RAW_FILES_DIR" ]; then
        mkdir -p "$RAW_FILES_DIR"
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
    echo -e "\n${GREEN}%%% Download files from a list, then descompress the files and remove the compressed files.${RESET}."

    for url in $FILE_URLS; do
        file_name=$(basename "$url")
        compressed_file="$RAW_FILES_DIR/$file_name"
        descompressed_file="$RAW_FILES_DIR/${file_name%.*}"

        echo -e "\n${BLUE}  % $file_name ${RESET}"
        if [ ! -e "$compressed_file" ] && [ ! -e "$descompressed_file" ]; then
            curl -L -o "$compressed_file" $url
        if [ ! -e "$descompressed_file" ]; then
            gzip -d "$compressed_file"
            FILES+=(${file_name%.*})
        fi
    done
}

validate_compression_and_decompression() {
    if ! cmp -s "$1" "$2"; then
        echo "$1 and $2 are different." >> "$GENERAL_REPORT/errors/errors-compress.txt"
    fi 
}
