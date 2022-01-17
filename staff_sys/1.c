#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int data[100];
int *pdata = data;

void func(int i,int *value)
{
	*(pdata +i) = *value;
	printf("pdata[2]=%d value=%d\n",*(pdata+1),*value);
	*value = 0;
	printf("i=%d\n",i);
	printf("pdata[2]=%d value=%d\n",*(pdata+i),*value);
}
char *GetMemory(char *p){
	p=(char *)malloc(13);
	return p;
}
int main(int argc, const char *argv[])
{
	char *str= NULL;
	printf("%ld\n",sizeof(str));

	str = GetMemory(str);  //获取10字节 的堆kongj
	strcpy(str, "hello world");  //要越界
	printf("%s\n",str);
	free(str);

	int n=9;
	func(5,&n);
	
	return 0;
}


