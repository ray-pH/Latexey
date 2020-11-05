#include "keyboard.h"

KeyChecker::KeyChecker(){
	this->display = XOpenDisplay(NULL);
}

bool KeyChecker::checkKey(){
	char keys_return[32];
	// bool keyPressed = false;
	bool isCtrl = false, isApos = false, isElse = false;
	XQueryKeymap(this->display,keys_return);
	for(int i=0; i<32; i++){
		int code = getVal(keys_return[i],i);
		if (code != 0){
			if (code == 37) 	 isCtrl = true;
			else if (code == 48) isApos = true;
			else 				 isElse = true;
			// keyPressed = true;
			// std::cout << code << " ";
		}
	}
	// if(keyPressed) std::cout << std::endl ;
	return (!isElse && (isCtrl && isApos ));
}

void KeyChecker::onExit(){
	XCloseDisplay(this->display);
}

int KeyChecker::getVal(char c, int i){
	int pos = 0; int num = c;
	while (pos < 8) {
		if ((num & 0x01) == 1) return i*8+pos;
		pos++; num /= 2;
	}
	return 0;
}