#include<stdio.h>
#include<stdlib.h>

int main(){
  char *numero = "1089.20";
  char *desconto = "003";

  printf("%.2f\n\n", atof(numero) - (atof(numero)*atof(desconto)/100));
  return 0;
}
