#include "./cstr_manip.h"

// Set all bytes to the value of "value", starting at the location "to", for the length of "length"
void cmemset(void* to, const unsigned char value, const unsigned int length){
	unsigned char* to_loc = to;
	unsigned int i = 0;
	while (i < length) {
		to_loc[i] = value;
		i++;
	}
	return;
}

// Copy memory, one byte at a time, from one location to another, for a given length in bytes
void cmemcpy(const void* from, void* to, const unsigned int length){
	const char* from_loc = from;
	char* to_loc = to;
	unsigned int i = 0;
	while (i < length) {
		to_loc[i] = from_loc[i];
		i++;
	}
	return;
}

// Get the length of the cstring including the NULL terminator
unsigned int cstrlen(const unsigned char *src) {
	unsigned int i = 0;
	while(src[i] != 0) {
		i++;
	}
	return i + 1;
}

// Copy a string from one buffer/pointer to another and return the length copied.  
// The NULL terminator is copied and the retured length includes it.
unsigned int cstrcpy(const unsigned char *src, unsigned char *dest) {
	unsigned int i = 0;
	while (src[i] != 0) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
	return i + 1;
}

// Compare 2 NULL terminated strings.  Returns 1 if equal, Returns 0 if not equal.
unsigned int cstrcmp(const unsigned char *string1, const unsigned char *string2) {
	unsigned int i = 0;
	while (string1[i] != 0 && string2[i] != 0) {
		if (string1[i] != string2[i]) {
			return 0;
		}
		i++;
	}
	if (string1[i] != string2[i]) {
		return 0;
	}
	return 1;
}

// char* buf should be of at least length of 12 or will overflow (ONLY APPLICABLE TO BASE 10)
void itoa(int32_t num, char* buf, int buf_len, int base) {
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
		unsigned char i_then_length = 0;
		unsigned char i_2 = 0;
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
				tmpstr[(int)i_then_length] = '0' + i_2;
			}else{
				tmpstr[(int)i_then_length] = '7' + i_2;
			}
			num /= base;
			i_then_length++;
		}
		//i_then_length is now a length count for char array
		//Loop to fix character order
		i_2 = 0;
		while(i_2 < i_then_length && i_2 < buf_len){
			buf[(int)i_2] = tmpstr[(int)((i_then_length - i_2) - 1)];
			i_2++;
		}
		if(i_2 < buf_len){
			buf[(int)i_2] = 0;
		}else{
			buf[(int)(i_2 - 1)] = 0;
		}
	}
	return;
}

// Convert a NULL terminated cstring, text, of base, base, to an integer
int catoi(char* text, unsigned int base) {
	if (text == 0 || base < 2 || base > 16) {
		return 0;
	}
	unsigned int i = 0;
	signed int working_value = 0;
	signed int multiplier = 1;
	unsigned int good_value;
	unsigned char val;
	while (text[i] != 0) {
		val = text[i];
		if (val == '-' && i == 0) {
			multiplier *= -1;
		} else {
			good_value = 0;
			working_value *= base;
			if (val >= 'a' && val <= 'f') {
				val -= 0x20;
			}
			if (val >= 'A' && val <= 'F') {
				val -= 'A' - 10;
				good_value = 1;
			} else if (val >= '0' && val <= '9') {
				val -= '0';
				good_value = 1;
			}
			if (val < base && good_value == 1) {
				working_value += val;
			} else {
				return 0;
			}
		}
		i++;
	}
	return working_value * multiplier;
}
