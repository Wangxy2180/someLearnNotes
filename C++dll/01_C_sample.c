/*
������ͨ��C���ԣ�ֻ�������һ��͹���
*/


/*
__declspec(dllexport)����Windows�еĶ�̬���У�
���������������ࡢ����ȹ��������
*/
#include <stdio.h>
__declspec(dllexport)
int sum(int a, int b)
{
	return a + b;
}

__declspec(dllexport)
int minus(int a, int b)
{
	return a - b;
}

__declspec(dllexport)
void print_welcome()
{
	printf("welcome to learn dll");
}