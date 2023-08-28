"""
Script usado para extrair substring em um intervalo l,r de um arquivo de texto descompactado. O objetivo é usar a substring extraída para validar o método extract do DCX.
"""
import sys

def extract(input_file, l, r, output_file):
    with open(input_file, 'r') as file:
        text = file.read()

    extracted_text = text[l:r]
    with open(output_file, 'w') as file:
        file.write(extracted_text)

def main(argv):
    input_file = argv[1]
    output_file = argv[2]
    l = int(argv[3])
    r = int(argv[4])

    extract(input_file, l, r, output_file)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
