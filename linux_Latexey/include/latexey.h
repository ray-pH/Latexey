#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <wx/display.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "pymod.h"
#include "taskbar.h"
#include "keyboard.h"
#include "dict.h"

#include <chrono>
#include <thread>

extern "C" {
	#include <xdo.h>
}

//custom event
wxDEFINE_EVENT(EVT_READY_PASTE, wxCommandEvent);
// wxDEFINE_EVENT(EVT_KEY_SHOW, wxCommandEvent);

class Frame : public wxFrame{
	public:
		Frame(const wxString& title, std::string execDir);
		void onKeyDown(wxKeyEvent& event);
		void onKeyUp(wxKeyEvent& event);
		void onLeftClick(wxMouseEvent& event);
		void onDrag(wxMouseEvent& event);
		void onPaste(wxEvent& event);
		void onTimer(wxEvent& event);
		void goShow(wxEvent& event);
		void checkDict();
		void repositionPopup();
		void processInput();
		void onReadyPaste();
		void goHide();
		void goShow();
		void onQuit();

	private:
		void initBinding();
		void initSizer();
		void refitSizer();
		void readConfig();
		std::vector<std::string> splitString(std::string str, std::string token);

		xdo_t* xdo;

		std::string execDir;
		std::string rawInput;
		std::string inputHead;
		std::string inputMid;
		std::string inputTail;
		std::vector<std::string> listVec;

		int displayHeight;

		int INPUT_WIDTH;
		int INPUT_FONTSIZE;
		int OUTPUT_FONTSIZE;
		int PANEL_VPAD;
		int KEYBOARD_TIMER;
		int PASTE_WAIT;
		int DICT_HEIGHT;
		int DICT_VPAD;

		TaskBarIcon* taskBar;

		wxPanel* panel;
		wxTextCtrl* textInput;
		wxStaticText* textOutput;
		wxBoxSizer* moduleSizer;
		PyMod* pyth;
		KeyChecker* keyChecker;
		Dict* dictPopup;
		wxTimer* timer;

		wxIcon icon;
		wxPoint lastMousePos;
		wxString val;
		wxString uni;
};