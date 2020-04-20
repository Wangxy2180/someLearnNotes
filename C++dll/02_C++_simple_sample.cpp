#include <iostream>
#include "02_C++_simple_sample.h"
#include <cstring>
/*
����C++������Ҫ����extern "C"{}���������
���ⲿ�ִ��밴C���Եķ�ʽ���б���
*/
extern "C"
{
	__declspec(dllexport)
		int sum(int a, int b)
	{
		std::cout << "result is " <<a + b << std::endl;
		return a + b;
	}

}


extern "C"
{
	//����һ�ֵ������к����ķ����������԰����е�ÿһ�������� __declspec(dllexport)
	dll_test dll__test;

	__declspec(dllexport)
	void print_welcome_c()
	{
		dll__test.print_welcome();
	}

	/*
	��Python�е���ʱ����Ҫ����д
	print_custom_c('��������'.encode('utf8'))����
	print_custom_c('��������'.encode('ascii'))
	���ֻ���ַ�������ת���룬ֻ�������һ���ַ�
	*/
	__declspec(dllexport)
	void print_custom_c(char* str )
	{
		dll__test.print_custom(str);
	}

	//��������������ˣ�������
	//__declspec(dllexport)
	//	char* ret_str()
	//{
	//	char aa[] = "aaaaa";
	//	return aa;
	//}
}
