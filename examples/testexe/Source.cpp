
//#include <stdio.h>
//#include<string.h>
struct test_field
{
	char fd1 : 1;
	int fd2 : 2;
	int fd3 : 3;
	int fd4 : 4;
	unsigned int fd5 : 5;
	unsigned int fdr : 9;
};
int main(int argc, char* argv[])
{
	//printf("hello world");
	int sss = 0x123456;
	test_field tfd = { 1, 2, 3, 4, 5, 9 };
	test_field* ptfd = (test_field* )&sss;
	int fd1 = ptfd->fd5;
	return fd1;
}