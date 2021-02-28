//#include <stdio.h>
//#include <math.h>
//#include <stdlib.h>
//
//typedef struct Item Item;
//struct Item {
//	int value;
//	Item *next;
//};

#include <stdio.h> 
struct p3d  
{ 
    int a[3]; 
    int b[3]; 
}; 
typedef struct p3d aPoint; 
int cs(int n, aPoint dim) 
{ 
     return *(dim.a+(--n)) + dim.b[n]; 
} 
int main() 
{ 
    aPoint a = { { 1,2,3 },{ 4,5,6 } }; 
    printf("%d", cs(2, a) % cs(1, a)); 
} 

//int main(void) {
//	int *p = malloc(sizeof(int));
//	*p = 64;
//	int x = *p;
//	printf("%p %d\n", p, x);
//
//	Item *l;
//	Item *i = malloc(sizeof(Item));
//	i->value = 2;
//	i->next = l;
//
//	printf("%p %d\n", i, i->value);
//	int as[N] = { 1, 2, 3, 4 };
//	char *ss[N];
//	ss[0] = "hello, world\0";
//	ss[1] = "foo\0";
//	ss[2] = "bar\0";
//	ss[3] = "baz\0";
//
//	for (int i = 0; i < N; i += 1)
//		printf("as[%d] = %d\n", i, as[i]);
//
//	for (int i = 0; i < N; i += 1)
//		printf("ss[%d] = %s\n", i, ss[i]);
//
//	return 0;
//}
