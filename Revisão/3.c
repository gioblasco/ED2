#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[]){
    char str;
    int conta = 0, palavras = 0;
    

    if (argc != 2){
        printf("A chamada do programa deve ser composta pelo nome de um arquivo de texto\n");
        return 0;
    }

    FILE *texto;
    if((texto = fopen(argv[1], "r")) == NULL)
        printf("O arquivo %s nao pode ser aberto\n", argv[1]);
    else{
        while ((str = fgetc(texto)) != EOF){
            conta++;
            if(str == ' ')
                palavras++;
        }
    }

    printf("Numero de caracteres: %d\n\nNumero de palavras: %d\n", conta, (conta > 0 ? ++palavras : palavras)); 
    fclose(texto);

    return 0;
}
