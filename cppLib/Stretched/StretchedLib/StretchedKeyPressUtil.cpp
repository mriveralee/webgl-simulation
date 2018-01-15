#include "StretchedKeyPressUtil.h"
#include <ctype.h>

extern bool compareKeyPress(int inputKey, int key) {
	return  (inputKey == key);
}

extern bool compareKeyPressIgnoreCase(int inputKey, int key) {
	return compareKeyPress(inputKey, key) 
		|| compareKeyPress(toupper(inputKey), toupper(key));
}
