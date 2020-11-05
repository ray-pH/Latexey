#include <wx/wx.h>
#include <wx/taskbar.h>
#include <string.h>
class Frame;

class TaskBarIcon : public wxTaskBarIcon {
	public:
		TaskBarIcon(Frame* frame, std::string execDir);
		virtual wxMenu* CreatePopupMenu();
		void onLeftClick(wxTaskBarIconEvent& event);
		void onShow(wxEvent& event);
		void onQuit(wxEvent& event);

	private:
		Frame* frame;
		wxIcon icon;
		std::string execDir;
};