#include "dict.h"

Dict::Dict(wxWindow* parent, wxSize size) : wxPopupWindow(parent){
	this->SetSize(size);
	this->listBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, this->GetSize());
}

void Dict::setItems(std::vector<std::string> listVec){
	this->listBox->Clear();
	wxArrayString wxArr;
	for(std::string st : listVec){
		wxString wxStr(st.c_str(), wxConvUTF8);
		wxArr.Add(wxStr);
	}
	this->listBox->InsertItems(wxArr,0);
}

void Dict::reposition(wxPoint pos){
	this->SetPosition(pos);
}

void Dict::goHide(){
	this->Show(false);
}

void Dict::goShow(){
	this->Show();
}