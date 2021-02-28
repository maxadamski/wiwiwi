#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>

#define MAX 100 
void zadanie1() { 
 char line[MAX] = "TeSt"; 
 //scanf("%s", line); 
 char *c = line; 
 while (*c) { 
  *c = *c + 1; 
  c++; 
 } 
 printf("%s\n", line); 
} 



void zadanie2() { 
 char c; 
 int sum = 0; 
 while ((c = getchar()) != EOF) { 
  if (c >= '0' && c <= '9') 
   sum += c - '0'; 
 } 
 printf("%d\n", sum); 
} 



void proc(char str[]) { 
 int i; 
 int size = strlen(str); 
 for (i = 0;i<size;i++) { 
  str[size - 1 - i] = str[i]; 
 } 
 printf("%s\n", str); 
} 

void zadanie3() { 
 char b[] = "Fajny-mam-program"; 
 proc(b); 
} 


int main(void) {
	zadanie2();
	return 0;
}

