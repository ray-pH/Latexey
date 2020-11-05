#include "latexey.h"

TaskBarIcon::TaskBarIcon(Frame* frame, std::string execDir):
	wxTaskBarIcon(){
	
	this->frame = frame;
	this->execDir = execDir;

	this->icon = wxIcon(); 
	this->icon.LoadFile(this->execDir+(std::string)"/data/trayicon.png");
	this->SetIcon(this->icon);

	this->Bind(wxEVT_TASKBAR_LEFT_DOWN, &TaskBarIcon::onLeftClick, this);
}

wxMenu* TaskBarIcon::CreatePopupMenu(){
	wxMenu* menu = new wxMenu();
	wxMenuItem* showItem = new wxMenuItem(menu, wxID_ANY, wxT("showItem"));
	wxMenuItem* quitItem = new wxMenuItem(menu, wxID_ANY, wxT("quitItem"));
	menu->Append(showItem->GetId(), wxT("Show Panel"));
	menu->Append(quitItem->GetId(), wxT("Quit"));
	menu->Bind(wxEVT_MENU, &TaskBarIcon::onQuit, this, quitItem->GetId());
	menu->Bind(wxEVT_MENU, &TaskBarIcon::onShow, this, showItem->GetId());
	return menu;
}

void TaskBarIcon::onQuit(wxEvent& event){
	this->frame->onQuit();
}

void TaskBarIcon::onShow(wxEvent& event){
	this->frame->goShow();
}

void TaskBarIcon::onLeftClick(wxTaskBarIconEvent& event){
	if   (this->frame->IsShown())
		 this->frame->goHide();
	else this->frame->goShow();
	event.Skip();
}

