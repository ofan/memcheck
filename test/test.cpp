/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  memcheck test
 *
 *        Version:  0.1
 *        Created:  2010/4/12 12:09:34
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  OFAN (OFAN), odayfans@gmail.com
 *        Company:  OFAN
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdexcept>
#include <new>
#include "memcheck.h"

struct A{
	int* data;
	public:
		A(){
			data=new int;
			//printf("A::A():(int)data==0x%p\n",data);
		}
		~A(){}
};

class E{
	int* data;
	public:
		E(int n){
			if(n==0){
				throw std::runtime_error("0 not allowed");
			}
			data=new int[n];
			//printf("E::E():(int[%d])data==0x%p\n",n,data);
		}
		~E(){
			delete[] data;
		}
};

int main(){
	A aa;
	E e(3);
	char* a=new char[128];
	//printf("main():a==0x%p\n",a);
	for(int i=0;i<128;++i){
		a[i]='\0';
	}
	int* b=new int[10];
	//printf("main():b==0x%p\n",b);
	for(int i=0;i<10;++i){
		b[i]=i;
	}
	delete b;
	try{
	    E* ee=new E(0);
	    //printf("main():ee==0x%p\n",ee);
	    delete ee;
	}catch(std::runtime_error& ex){
        printf("Exception:%s\n",ex.what());
	};
	return 0;
}
