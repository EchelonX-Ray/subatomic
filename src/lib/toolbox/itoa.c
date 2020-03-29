#include "carray.h"

// char* buf should be of at least length of 12 or will overflow (ONLY APPLICABLE TO BASE 10)
void itoa(int32_t num, char* buf, int buf_len, int base){
	if(base > 16 || base < 2){
		return;
	}
	if(buf_len < 2){
		return;
	}
	//Check if zero since this will fail to loop and can be easily handled now
	if(num == 0){
		buf[0] = '0';
		buf[1] = 0;
	}else{
		char tmpstr[32] = {[0 ... 31] = 0};
		char i_then_length = 0;
		char i_2 = 0;
		//i_then_length is now an index
		//Append "-" character for negatives
		if(num < 0){
			num = -num;
			buf[0] = '-';
			buf++;
		}
		//Find Characters
		while(num > 0){
			i_2 = num % base;
			if(i_2 < 10){
				tmpstr[i_then_length] = '0' + i_2;
			}else{
				tmpstr[i_then_length] = '7' + i_2;
			}
			num /= base;
			i_then_length++;
		}
		//i_then_length is now a length count for char array
		//Loop to fix character order
		i_2 = 0;
		while(i_2 < i_then_length && i_2 < buf_len){
			buf[i_2] = tmpstr[i_then_length - i_2 - 1];
			i_2++;
		}
		if(i_2 < buf_len){
			buf[i_2] = 0;
		}else{
			buf[i_2 - 1] = 0;
		}
	}
	return;
}
