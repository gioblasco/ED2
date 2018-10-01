#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(){
  char string[8] = "qwerty";
  memset(string, 0, sizeof(string));
  printf("%s\n\n", string);

  strcat(string, "oi");
  printf("%s\n\n", string);

  return 0;
}
