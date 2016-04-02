#include<stdio.h>
#include<stdlib.h>

char *a;

char *print(char *ptr)
{
	char *p=malloc(sizeof(char *));
	p=ptr;
	printf("helo world\n");
	
	return p;
}

void destroy(void)
{
	printf("11111111111111\n");
	free(a);
	perror("free");
}

int main(void)
{
	char *(*p)(char ptr);
	a=(*print)("guiqinghai");
	printf("%s\n",a);
//	free(a);
	atexit(destroy);
	return 0;
}
