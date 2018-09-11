#include<stdio.h>

int main(int argc, char *argv[]){
    char str;

    if (argc != 2){
        printf("A chamada do programa deve ser composta pelo nome de um arquivo de texto\n");
        return 0;
    }

    FILE *texto, *copia;
    if((texto = fopen(argv[1], "r")) == NULL)
        printf("O arquivo %s nao pode ser aberto\n", argv[1]);
    else{
        if((copia = fopen("copia.txt", "w")) == NULL)
            printf("O arquivo copia nao pode ser aberto\n");
        else{
            while ((str = fgetc(texto)) != EOF){
                if((str >= 60 && str <= 90) || (str >= 97 && str <= 122))
                    fprintf(copia, "%c", str);
            }
        }
    }    

    fclose(copia);
    fclose(texto);

    return 0;
}
