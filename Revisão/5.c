#include<stdio.h>

int main(){
    int num1, num2;

    FILE *texto, *result;
    if((texto = fopen("arq.in", "r")) == NULL)
        printf("O arquivo arq.in nao pode ser aberto\n");
    else{
        if((result = fopen("arq.out", "w")) == NULL)
            printf("O arquivo arq.out nao pode ser aberto\n");
        else{
            while (fscanf(texto, "%d %d", &num1, &num2) != EOF){
                fprintf(result, "%d %d %d\n", num1, num2, num1+num2);
            }
        }
    }    

    fclose(result);
    fclose(texto);

    return 0;
}
