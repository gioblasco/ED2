#include<stdio.h>

int main(int argc, char *argv[]){
    char str;

    if (argc != 2){
        printf("A chamada do programa deve ser composta pelo nome de um arquivo de texto\n");
        return 0;
    }

    FILE *texto;
    if((texto = fopen(argv[1], "r")) == NULL)
        printf("O arquivo %s nao pode ser aberto\n", argv[1]);
    else{
        while ((str = fgetc(texto)) != EOF){
            putchar(str);
        }
    }    

    fclose(texto);

    return 0;
}
