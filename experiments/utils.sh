#!/bin/bash

#constants
GREEN='\033[0;32m'
BLUE='\033[34m'
YELLOW='\033[33m'
RED='\033[31m'
RESET='\033[0m'
CURR_DATE=$(date +"%Y-%m-%d")

#files to compress
files=()
compressed_success_files=()

#directories
REPORT_DIR="../report"
COMP_DIR="../dataset/compressed_files"
RAW_FILES_DIR="../dataset/raw_files"

if [ "$(uname -s)" = "Darwin" ]; then
    stat_options="-f %z"
    CURR_DATE="$CURR_DATE-mac"
    FILE_URLS=$(cat files_paths/initial_test_files.txt)
else
    stat_options="-c %s"
    FILE_URLS=$(cat files_paths/repetitive_text.txt files_paths/regular_text.txt)
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
    echo -e "\n${GREEN}%%% Download files from a list, then descompress the files and remove the compressed files.${RESET}.
    "
    for url in $FILE_URLS; do
        type=$(echo "$url" | awk -F/ '{print $(NF-1)}')
        file=$(basename "$url")
        file_name="${type}-${file}"

        compressed_file="$RAW_FILES_DIR/$file_name"
        descompressed_file="$RAW_FILES_DIR/${file_name%.*}"

        echo -e "\n${BLUE}  % $file_name ${RESET}"
        if [ ! -e "$compressed_file" ] && [ ! -e "$descompressed_file" ]; then
	        echo -e "\t--Download file\n"
            curl -L -o "$compressed_file" $url
	    fi
	    if [ ! -e "$descompressed_file" ]; then
            extension="${file_name##*.}"
            echo -e "\t--Unzipping the file (format $extension)"
	        if [ "$extension" = "bz2" ]; then
	            bzip2 -d "$compressed_file"
	        elif [ "$extension" = "gz" ]; then
	            gzip -d "$compressed_file"
	        else
                echo -e "\tUnidentifed file format for descompression."
                echo -e "\tURL=$url."
            fi
        fi
        if [ -e "$descompressed_file" ]; then
            files+="${file_name%.*} "
        fi
    done
}

checks_equality() {
    if ! cmp -s "$1" "$2"; then
        echo -e "\n\t\t\t ${RED} The files are the differents. ${RESET}\n"
        echo "$1 and $2 are different." >> "$GENERAL_REPORT/errors/errors-$3-compress.txt"
    elif [ "$3" = "extract" ]; then
        echo -e "\n\t\t\t ${GREEN} The files are the same. ${RESET}\n"
    elif [ "$3" = "dcx" ]; then
        echo -e "\n\t\t\t ${GREEN} The files are the same. ${RESET}\n"
        local file=$1
        compressed_success_files+="${file##*/} "
    fi 
}
