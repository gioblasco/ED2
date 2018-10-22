/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
  char qtd[5] = "0099";
  int num;

  num = atoi(qtd+3);
  printf("%d\n", num);


  return 0;
}
