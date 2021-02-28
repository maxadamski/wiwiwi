#include <stdio.h> 
  
#define k 1
#include <stdio.h> 
#define N sizeof(F)/sizeof(int) 
#define False 0 
#define True  1 
int main(void) { 
    int F[] = { 1,  2,  3,  4,  5, 
        6,  7,  8,  9, 10, 
        11, 12, 13, 14 }; 
    int i, OK; 
    for (i = k; i < N; i++) 
        F[i] = F[i - 1] + F[i - k]; 
    /*** Testing: ***/ 
    OK = True; 
    for (i = 0; i < N - 1; i++) 
        if (F[i + 1] / F[i] != 2) 
            OK = False; 
    if (OK) printf("OK\n"); 
    else   printf("Wrong\n"); 
    return 0; 
} 
