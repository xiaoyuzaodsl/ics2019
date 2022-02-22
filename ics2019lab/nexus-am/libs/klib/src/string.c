#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	size_t len = 0;
	while(*(s+len) != '\0'){
		len++;
	}
    return len;
}

char *strcpy(char* dst,const char* src) {
    /*size_t tmp = 0;
	while(*(src+tmp)){
		*(dst+tmp) = *(src+tmp);
		tmp++;
	}
	*(dst+tmp) = '\0';
	return dst;*/
	size_t x = strlen(src);
	return strncpy(dst,src,x);
}

char* strncpy(char* dst, const char* src, size_t n) {
	size_t len_src = strlen(src);
	if(n > len_src) n = len_src;
	size_t x = 0;
	if(src + n > dst){
		while(x < n){
			*(dst+(n-1)-x) = *(src+(n-1)-x);
			x++;
		}
	}
	else{
		while(x < n){
			*(dst+x) = *(src+x);
			x++;
		}
	}
	*(dst+n)='\0';
	return dst;
}

char* strcat(char* dst, const char* src) {
	char* res = dst;
	size_t x = strlen(dst);
	dst = dst+ x;
	strcpy(dst,src);
	return res;
}

int strcmp(const char* s1, const char* s2) {
	int res = 0;
	while(res==0&&*s1&&*s2){
		res = *s1 - *s2;
		s1++;
		s2++;
	}
	return res;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	int res = 0;
	while(res==0&&*s1&&*s2&&(n>=0)){
		n--;
		res = *s1 - *s2;
		s1++;
		s2++;
	}
	return res;
}

void* memset(void* v,int c,size_t n) {
	void* res = v;
	while(n--){
		*((char*)v) = (char)c;
		v = (char*)v + 1;
	}
	return res;
}

void* memcpy(void* out, const void* in, size_t n) {
	void* res = out;
	char* s1 = (char*)in;
	char* s2 = (char*)out;
	if(in+n>out){
		while(n--){
			*(s2+n-1) = *(s1+n-1);
		}
	}
	else{
		while(n--){
			*s2 = *s1;
			s2++;
			s1++;
		}
	}
	return res;
}

int memcmp(const void* s1, const void* s2, size_t n){
	char* x1 = (char*)s1;
	char* x2 = (char*)s2;
	return strncmp(x1,x2,n);
}

#endif
