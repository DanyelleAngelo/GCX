
## Difference Cover Module 3 (DC3)

O DC3 é um algoritmo de divisão e conquista que tem por objetivo gerar um vetor de sufixos para um texto de entrada T.<br>O procedimento do DC3 consiste em dividir o texto de entrada em 2 strings, gerar o vetor de sufixos de cada uma destas, e por último combinar o resultado das duas soluções para gerar o vetor de sufixos do texto de entrada.
<br>

Temos 1 arquivo de teste neste repositório que valida o vetor de sufixos criado pelo DC3. Para rodar os testes, execute no terminal:

```
make test
```

Você pode executar o DC3 para qualquer texto de entrada, executando no terminal o comando:
```
# usando o Makefile
make main FILE=seu_arquivo

# compilando e executando o programa
g++ -c dc3.cpp -o dc3.o
g++ -c main.cpp -o main.o
g++ -o main dc3.o main.o 
./main seu_arquivo
```

A saída do programa imprimirá o tempo gasto para criar o vetor de sufixos. Para visualizar o vetor de sufixos gerado, use a flag `-D DEBUG=1` no momento de compilação do arquivo main.cpp.