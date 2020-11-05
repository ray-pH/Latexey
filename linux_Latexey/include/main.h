#include <wx/wx.h>
#include <string.h>
#include "latexey.h"
// class TaskBarIcon;

class MyApp : public wxApp{
	public:
		virtual bool OnInit();
		std::string getDirPath();
	private:
		std::string execDir;
};