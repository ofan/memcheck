/*
 * =====================================================================================
 *
 *       Filename:  memcheck.cpp
 *
 *    Description:  Check memory leak in compile-time & runtime.
 *
 *        Version:  0.1
 *        Created:  2010/4/11 16:55:59
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  OFAN (OFAN), odayfans@gmail.com
 *        Company:  OFAN
 *      Copyright:  PUBLIC DOMAIN
 * =====================================================================================
 */
/*!NOTE: Don't include this file DIRECTLY,include the header.Compile this file and link to your program. */
/*!NOTE: This implementation is not thread safe,so dont use it in your multi-thread programs **/
/*!TODO: Make it thread safe */
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <new>

//#define MC_NO_NAME_COPY

#ifdef _MSC_VER
#pragma warning(disable: 4073)
#pragma init_seg(lib)
#endif

//just give warnings,don't abort the program
#ifndef MC_ABORT_WHEN_LEAK
#define MC_CONTINUE_WHEN_LEAK
#endif

//display "filename"  in the output
#ifndef MC_NO_FILENAME
#define MC_FILENAME
#endif
//display "line"  number in the output
#ifndef MC_NO_LINE_NUMBER
#define MC_LINE_NUMBER
#endif
//display "size"  of mem allocated in the output
#ifndef MC_NO_SIZE
#define MC_SIZE
#endif
//display "function"  name in the output
#ifndef MC_NO_FUNCTION
#define MC_FUNCTION
#endif
//display "pointer"  address in the output
#ifndef MC_NO_ADDRESS
#define MC_ADDRESS
#endif

//give at least one status of memory information
#if    !defined(MC_FILENAME)\
	&& !defined(MC_FUNCTION)\
	&& !defined(MC_LINE_NUMBER)\
	&& !defined(MC_ADDRESS)
#error "You have to specify at least one status option:MC_FILENAME,MC_FUNCTION,MC_LINE,MC_ADDRESS!"
#endif // MC_FILENAME || MC_FUNCTION ....

//Defaultlly,this program will copy file name and function name to cookie structs
//because we found that the addresses of __FILE__ and __FUNCTION will be not accessible somtimes
//if it worked fine on your compiler without copying,dont copy them :)
#ifndef MC_NO_NAME_COPY
#define MC_NAME_COPY
#endif

//Max char will be copyed
//defined MC_NAME_LEN as 0 takes the same effect as defining MC_NO_NAME_COPY
#ifndef MC_NAME_LEN
#define MC_NAME_LEN 20
#endif

#if MC_NAME_LEN==0 && !defined(MC_NO_NAME_COPY)
#define MC_NO_NAME_COPY
#endif

//hash table size,that limits the MAX allocation
#ifndef MC_HASHTABLESIZE
//you can allocate 1024 times by default
#define MC_HASHTABLESIZE 16384
#endif
//hash function
#ifndef MC_HASH
#define MC_HASH(x) (((unsigned)(x)) >> 8) % MC_HASHTABLESIZE
#endif

//struct for memory block information
struct mc_block_node_t{
#ifndef MC_NO_NAME_COPY
#ifdef MC_FILENAME
	char				file[MC_NAME_LEN+1];
#endif
#ifdef MC_FUNCTION
	char				func[MC_NAME_LEN+1];
#endif
#else //MC_NO_NAME_COPY
#ifdef MC_FILENAME
	const char*			file;
#endif
#ifdef MC_FUNCTION
	const char*			func;
#endif
#endif //MC_NO_NAME_COPY
#ifdef MC_LINE_NUMBER
	int					line;
#endif
	size_t				size;
	mc_block_node_t*	next;
};//struct mc_block_node_t

//hash table
static mc_block_node_t* pTable[MC_HASHTABLESIZE];
/* this function check the memory leak,it's called automatically when\
 * program exits if MC_NO_AUTOCHECK is not defined
 */
bool mc_checkmem(){
	bool memLeaked=false;
	for(int i=0;i<MC_HASHTABLESIZE;++i){
		mc_block_node_t* ptr=pTable[i];
		if(ptr==NULL){
			continue;
		}
		memLeaked=true;
		while(ptr){
			printf("Leaked object (size %u ",ptr->size);
#ifdef MC_ADDRESS
			printf("at 0x%p ",(ptr+1));
#endif
#ifdef MC_FILENAME
			printf("in %s",ptr->file);
#endif
#ifdef MC_FUNCTION
			printf(":%s()",ptr->func);
#endif
#ifdef MC_LINE_NUMBER
			printf(":%d",ptr->line);
#endif
			printf(")\n");
			ptr=ptr->next;
		}
	}
	return memLeaked;
}

//autocheck memory leak when program terminates
static bool MC_AUTOCHECK = true;
//display detailed memory allocating & freeing information
static bool MC_DETAIL = true;

void* operator new(size_t size,const char* file,const char* func,const int line){
    //calculate the total memory needs to be allocated
	size_t fsize = size+sizeof(mc_block_node_t);
	mc_block_node_t* ptr=(mc_block_node_t*)malloc(fsize);\
	//give information when allocation failed then exit
	if(ptr == NULL){
		fprintf(stderr,"new: out of memory when allocating %u bytes\n",
				size);
		abort();
	}
	//pointer of user date
	void* memBlock=(void*)(ptr+1);
	//fill the cookie data
	ptr->next = NULL ;//pTable[hashindex]
#ifdef MC_NO_NAME_COPY
	ptr->file=file;
	ptr->func=func;
#else
	ptr->file[MC_NAME_LEN]='\0';
	ptr->func[MC_NAME_LEN]='\0';
	strncpy(ptr->file,file,MC_NAME_LEN);
	strncpy(ptr->func,func,MC_NAME_LEN);
#endif
	ptr->line = line;
	ptr->size = size;

	/** insert cookie struct to hash table **/
	size_t hashindex = MC_HASH(memBlock);
	//check collision
	if(pTable[hashindex] != NULL){
		mc_block_node_t *pp,*p=pTable[hashindex];
		while(p){
			pp=p;
			p=pp->next;
		}
		pp->next=ptr;
	}else{
		pTable[hashindex] = ptr;
	}
    if(MC_DETAIL){
        printf("new: allocating ");
    #ifdef MC_SIZE
        printf("%u bytes",ptr->size);
    #endif
    #ifdef MC_ADDRESS
        printf(" at 0x%p",memBlock);
    #endif
    #ifdef MC_FILENAME
        printf(" %s",ptr->file);
    #endif
    #ifdef MC_FUNCTION
        printf(":%s()",ptr->func);
    #endif
    #ifdef MC_LINE_NUMBER
        printf(":%d",ptr->line);
    #endif
        printf(".\n");
    }//if(MC_DETAIL
	return memBlock;
}

void* operator new[](size_t size,const char* file,const char* func,const int line){
	return operator new(size,file,func,line);
}

void* operator new(size_t size){
	return operator new(size,"<Unknown>","<Unknown>",0);
}

void* operator new[](size_t size){
	return operator new(size);
}

void* operator new(size_t size,const std::nothrow_t&)throw(){
	return operator new(size);
}

void* operator new[](size_t size,const std::nothrow_t&)throw(){
	return operator new[](size);
}

void operator delete(void* pointer){
	if(!pointer){
		return;
	}
	size_t hashindex = MC_HASH(pointer);
	mc_block_node_t* ptr_last,*ptr;
	ptr_last=ptr=pTable[hashindex];
	if(ptr){
        while(ptr->next && (ptr+1)!=pointer){
            ptr_last=ptr;
            ptr     =ptr->next;
        }
    }
	//if pointer is not in the hash table
	//then give an error message
	if(!ptr){
		fprintf(stderr,"delete: invalid pointer 0x%p.\n",pointer);
		#ifdef MC_ABORT_WHEN_LEAK
		abort();
		#else
		return;
		#endif
	}
	//display detail,it's useful if you want to trace memory allocating and freeing
	if(MC_DETAIL){
        printf("delete: freeing ");
    #ifdef MC_SIZE
        printf("%u bytes",ptr->size);
    #endif
    #ifdef MC_ADDRESS
        printf(" at 0x%p",pointer);
    #endif
    #ifdef MC_FILENAME
        printf(" %s",ptr->file);
    #endif
    #ifdef MC_FUNCTION
        printf(":%s()",ptr->func);
    #endif
    #ifdef MC_LINE_NUMBER
        printf(":%d",ptr->line);
    #endif
        printf(".\n");
	}//if(MC_DETAIL)
	ptr_last->next=ptr->next;
	free(ptr);
	if(pTable[hashindex] == ptr){
		pTable[hashindex] = NULL;
	}
	return;
}

void operator delete[](void* pointer){
	operator delete(pointer);
}

#ifndef MC_NO_PLACEMENT_DELETE
void operator delete(void* pointer,const char* file,const char* func,const int line){
    //this delete operator is to catch exceptions when object initialization failed
	printf("info: exception thrown on initializing object");
#ifdef MC_ADDRESS
	printf(" at 0x%p",pointer);
#endif
#if defined(MC_SIZE) || defined(MC_FILE) || defined(MC_FUNCTION) || defined(MC_LINE_NUMBER)
	printf(" (");
#endif
#ifdef MC_FILENAME
	printf("%s",file);
#else
	printf(":");
#endif
#ifdef MC_FUNCTION
	printf(":%s()",func);
#else
	printf(":");
#endif
#ifdef MC_LINE_NUMBER
	printf(":%d",line);
#else
	printf(":");
#endif
#if defined(MC_SIZE) || defined(MC_FILE) || defined(MC_FUNCTION) || defined(MC_LINE_NUMBER)
	printf(" )");
#endif
	printf(".\n");
	operator delete(pointer);
}

void operator delete[](void* pointer,const char* file,const char* func,const int line){
	operator delete(pointer,file,func,line);
}

void operator delete(void* pointer,const std::nothrow_t&){
	operator delete(pointer,"<Unknown>","<Unknown>",0);
}

void operator delete[](void* pointer,const std::nothrow_t&){
	operator delete(pointer,std::nothrow);
}
#endif // MC_NO_PLACEMENT_DELET

class MemCheck{
	public:
		MemCheck(){
			memset(pTable,0,sizeof(mc_block_node_t*) * MC_HASHTABLESIZE);
		}
		~MemCheck(){
		    //auto check memory leak when exit the program
		    //define MC_NO_AUTOCHECK to turn it off.
			if(MC_AUTOCHECK){
				if(mc_checkmem()){
				    #ifdef MC_ABORT_WHEN_LEAK
					abort();
					#endif
				}
			}
		}
};
//auto check object
static MemCheck mc_autocheck_object
#ifdef __GNUC__
__attribute__((init_priority (101)))
#endif
;
