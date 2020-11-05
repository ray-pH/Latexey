#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

class KeyChecker{
	public:
		KeyChecker();
		bool checkKey();
		void onExit();
	private:
		Display* display;
		int getVal(char c, int i);
};