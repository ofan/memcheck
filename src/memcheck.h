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
/*!NOTE: Include before your 'new' operations you want to trace & after system and standard headers. like <vector>...*/
/*!NOTE: This implentation is not thread safe,so dont use it in your multi-thread programs **/
/*!TODO: Make it thread safe */
#ifndef MEMCHECK //multi-include protection
#define MEMCHECK
//these declarations are required by Visual C++ 6.0
void* operator new(size_t size, const char* file,const char* func,const int line);
void* operator new[](size_t size, const char* file,const char* func,const int line);
/* some ancient compilers don't support the replacement of delete operator\
 * make sure to define MC_NO_PLACEMENT_DELETE when use these compilers
 */
#ifndef MC_NO_PLACEMENT_DELETE
void operator delete(void* pointer, const char* file,const char* func,const int line);
void operator delete[](void* pointer, const char* file,const char* func,const int line);
#endif // MC_NO_PLACEMENT_DELETE
void operator delete[](void*);

//check memory leak function,called automatically on exit or manually on runtime.
bool mc_checkmem();
/****user *MUST* include these definitions if want to check memory leak***/
#ifndef MC_NO_REDEFINITION
	#define new MC_NEW
	#define MC_NEW new(__FILE__,__FUNCTION__,__LINE__)
	#define mc_new new
#else
//use defined function name instead of keyword new & delete
	#define debug_new new(__FILE__,__FUNCTION__,__LINE__)
#endif

//define MC_REDEFINE_MALLOC_FREE to trace malloc & free
#ifdef MC_REDEFINE_MALLOC_FREE
#define malloc(s) ((void*)mc_new unsigned char[s])
#define free(p)   (delete [] (char*)(p));
#endif
#endif // MEMCHECK
