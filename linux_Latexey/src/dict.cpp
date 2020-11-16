#include "dict.h"

Dict::Dict(wxWindow* parent, wxSize size) : wxPopupWindow(parent){
	this->SetSize(size);
	this->cursor = -1;
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
	int num = this->listBox->GetCount();
	if(this->cursor <  -1 ) this->cursor = -1;
	if(this->cursor >= num) this->cursor = num-1;
	this->listBox->SetSelection(this->cursor);
}

void Dict::incrCur(int n){
	this->cursor += n;
}

void Dict::resetCur(){
	this->cursor = -1;
}

int Dict::getSelectCur(){
	return this->listBox->GetSelection();
}

std::string Dict::getSelectString(){
	return (std::string)this->listBox->GetString(this->cursor).mb_str();
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