#include <wx/wx.h>
#include <wx/popupwin.h>
#include <string>

class Dict : public wxPopupWindow{
	public:
		Dict(wxWindow* parent, wxSize size);
		void reposition(wxPoint pos);
		void setItems(std::vector<std::string> listVec);
		void incrCur(int n);
		void resetCur();
		void goHide();
		void goShow();
		int  getSelectCur();
		std::string getSelectString();
	private:
		wxListBox* listBox;
		int cursor;
};