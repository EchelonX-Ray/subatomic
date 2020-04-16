#include "./cstr_manip.h"
#include <stdlib.h>

// Delete content from cstr at offset offset and of length, length
// If offset is negative, it is considered relative to the end of the string
// If length is negative, it is read to the left of offset in stead of the right
// If data to be deleted is out of bounds of the cstr, out of bounds data is ignored
// The NULL terminator is not deletable by this function.  It will always be appended to the end.
void cdelstr(char **heap_buffer_ptr, signed int offset, signed int length) {
	if (length == 0 || heap_buffer_ptr == 0) {
		return;
	}
	char *heap_buffer;
	heap_buffer = *heap_buffer_ptr;
	if (heap_buffer[0] == 0) {
		return;
	}
	signed int heap_buffer_len;
	heap_buffer_len = cstrlen(heap_buffer);
	if (offset < 0) {
		offset += heap_buffer_len;
	}
	signed int tmp;
	signed int end;
	end = offset + length;
	if (length < 0) {
		tmp = end;
		end = offset;
		offset = tmp;
	}
	if (offset < 0) {
		offset = 0;
	}
	if (end >= heap_buffer_len) {
		end = heap_buffer_len - 1;
	}
	if (end <= offset) {
		return;
	}
	length = end - offset;
	
	char *tmp_heap_buffer;
	tmp_heap_buffer = malloc(heap_buffer_len - length);
	tmp = 0;
	end = 0;
	while (tmp < heap_buffer_len) {
		if (tmp == offset) {
			tmp += length;
		}
		tmp_heap_buffer[end] = heap_buffer[tmp];
		tmp++;
		end++;
	}
	*heap_buffer_ptr = tmp_heap_buffer;
	free(heap_buffer);
	return;
}

// Insert cstr into cstr
// A negative offset means relative to the end of the heap_buffer
void cinsstr(const char *payload, char **heap_buffer_ptr, signed int offset) {
	if (payload == 0 || heap_buffer_ptr == 0) {
		return;
	}
	if (payload[0] == 0) {
		return;
	}
	char *heap_buffer;
	heap_buffer = *heap_buffer_ptr;
	if (heap_buffer == 0) {
		if (offset == 0) {
			heap_buffer = malloc(cstrlen(payload) * sizeof(char));
			cstrcpy(payload, heap_buffer);
			*heap_buffer_ptr = heap_buffer;
		}
		return;
	}
	signed int heap_buffer_len;
	signed int payload_buffer_len;
	heap_buffer_len = cstrlen(heap_buffer);
	payload_buffer_len = cstrlen(payload) - 1;
	if (offset < 0) {
		offset += heap_buffer_len;
	}
	if (offset >= heap_buffer_len || offset < 0) {
		return;
	}
	char *tmp_heap_buffer;
	tmp_heap_buffer = malloc((heap_buffer_len + payload_buffer_len) * sizeof(char));
	cmemcpy(heap_buffer, tmp_heap_buffer, offset * sizeof(char));
	cmemcpy(payload, tmp_heap_buffer + offset, payload_buffer_len * sizeof(char));
	cmemcpy(heap_buffer + offset, tmp_heap_buffer + offset + payload_buffer_len, (heap_buffer_len - offset) * sizeof(char));
	*heap_buffer_ptr = tmp_heap_buffer;
	free(heap_buffer);
	return;
}

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
unsigned int cstrlen(const char *src) {
	unsigned int i = 0;
	while(src[i] != 0) {
		i++;
	}
	return i + 1;
}

// Copy a string from one buffer/pointer to another and return the length copied.  
// The NULL terminator is copied and the retured length includes it.
unsigned int cstrcpy(const char *src, char *dest) {
	unsigned int i = 0;
	while (src[i] != 0) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = 0;
	return i + 1;
}

// Compare 2 NULL terminated strings.  Returns 1 if equal, Returns 0 if not equal.
unsigned int cstrcmp(const char *string1, const char *string2) {
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
void citoa(int32_t num, char* buf, int buf_len, int base) {
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
