/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
  char qtd[40] = "giovannablascomartin";
  char over[20] = "mateus";
  int num;

  memmove(qtd+5, over, strlen(over));
  printf("%s\n", qtd);


  return 0;
}
