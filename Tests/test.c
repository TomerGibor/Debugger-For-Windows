#include <stdio.h>

void print(int i){
    printf("%d: The debugee is running!!!\n", i);
}
int main(){
    int i = 0;
    for (i = 0; i < 10; i++)
	    print(i);
	return 0;
}
