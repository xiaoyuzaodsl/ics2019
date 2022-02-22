#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    char out[200];
	memset(out,'\0',sizeof(out));
	va_list ap;
	va_start(ap,fmt);
	vsprintf(out,fmt,ap);
	va_end(ap);
	int len = strlen(out);
	for(int i = 0;i < len;i++){
		_putc(out[i]);
	}
	return 0;
}
int num_to_str(int n,char *s){
	int num = n;
	int i = 0;
	char tmp[200];
	while(num > 0){
		int x = num % 10;
		num = (num - x) / 10;
		tmp[i] = x + '0';
		i++;
	}
	for(int j = 0;j < i;j++){
		s[j] = tmp[i-1-j];
	}
	s[i] = '\0';
	return 0;
}
int vsprintf(char *out, const char *fmt, va_list ap) {
	char *res = out;
	out[0] = '\0';
	while(*fmt){
		if(*fmt!='%'){
			int x = strlen(res);
			*(res+x) = *fmt;
			*(res+x+1) = '\0';
		}
		else{
			fmt++;
			char c = *fmt;
			switch(c){
				case 'd':
					{
						int num = 0;
						char p[200];
						num = va_arg(ap,int);
						/*下面是数字转化字符串*/
						num_to_str(num,p);
						strcat(res,p);
						break;
					}
				case 's':
					{
						char *p = va_arg(ap,char *);
						strcat(res,p);
						break;
					}
				default:
					break;
			}
		}
		fmt++;
	}
	return 0;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	int x = vsprintf(out,fmt,ap);
	va_end(ap);
	return x;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
