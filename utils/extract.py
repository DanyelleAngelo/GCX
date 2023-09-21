"""
Script usado para extrair substring em um intervalo l,r de um arquivo de texto descompactado. O objetivo é usar a substring extraída para validar o método extract do DCX.
"""
import sys

def extract(input_file, output_file, input_queries):
    with open(input_file, 'r') as file:
        text = file.read()
    i=0
    with open(input_queries, 'r') as queries:
        for line in queries:
            l, r = line.split(' ')
            l = int(l)
            r = int(r)+1
            extracted_text = text[l:r]
            with open(output_file, 'a+') as file:
                file.write(f"[{l},{r}]\n")
                file.write(extracted_text)
                file.write('\n')
            extracted_text = ''

def main(argv):
    input_file = argv[1]
    output_file = argv[2]
    input_queries = argv[3]

    extract(input_file, output_file, input_queries)

if __name__ == '__main__':
    sys.exit(main(sys.argv))
