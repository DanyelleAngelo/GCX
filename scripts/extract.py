"""
Script usado para extrair substring em um intervalo l,r de um arquivo de texto descompactado. O objetivo é usar a substring extraída para validar o método extract do DCX.
"""
import sys

def extract(input_file, output_file, input_queries):
    with open(input_file, 'rb') as file:
        text = file.read()
    i=0
    with open(input_queries, 'r') as queries:
        for line in queries:
            l, r = line.split(' ')
            l = int(l)
            r = int(r)
            extracted_text = text[l:r+1]
            with open(output_file, 'ab+') as file:
                file.write(f"[{l},{r}]\n".encode('utf-8'))
                file.write(extracted_text)
                file.write(b'\n')
            extracted_text = ''

def main(argv):
    input_file = argv[1]
    output_file = argv[2]
    input_queries = argv[3]

    extract(input_file, output_file, input_queries)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
