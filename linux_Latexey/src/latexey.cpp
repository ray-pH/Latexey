#include "latexey.h"

Frame::Frame(const wxString& title, std::string execDir)
	: wxFrame(NULL, wxID_ANY, title, wxDefaultPosition){

	this->execDir 		= execDir;
	this->readConfig();

	//creating components
	this->panel 		= new wxPanel(this, wxID_ANY, wxDefaultPosition);
	this->textInput 	= new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, 
		                                 wxSize(this->INPUT_WIDTH,-1));
	this->textOutput 	= new wxStaticText(this, wxID_ANY, "");
	this->taskBar 		= new TaskBarIcon(this, this->execDir);
	this->moduleSizer 	= new wxBoxSizer(wxVERTICAL);
	this->timer			= new wxTimer();
	this->keyChecker	= new KeyChecker();
	this->pyth 			= new PyMod(execDir);
	this->dictPopup		= new Dict(this->panel, wxSize(this->INPUT_WIDTH, this->DICT_HEIGHT));
	this->xdo   		= xdo_new(":0.0");

	//initializing variables
	this->displayHeight= wxDisplay().GetGeometry().GetHeight();
	this->lastMousePos = wxPoint(0,0);
	this->val 		   = wxString("");
	this->inputHead    = "";
	this->inputMid     = "";
	this->inputTail    = "";

	//do some initialization procedures
	this->initSizer();
	this->refitSizer();
	this->initBinding();
	this->timer->Start(this->KEYBOARD_TIMER);

	//setting icon
	this->icon = wxIcon(); 
	this->icon.LoadFile(this->execDir+(std::string)"/data/icon.png");
	this->SetIcon(this->icon);

	// this->goShow();
	// this->dictPopup->Show();
	Centre();
}

void Frame::initBinding(){
	this->panel    ->Bind(wxEVT_LEFT_DOWN,&Frame::onLeftClick,  this);
	this->panel    ->Bind(wxEVT_MOTION,   &Frame::onDrag, 		this);
	this->panel    ->Bind(wxEVT_KEY_DOWN, &Frame::onKeyDown, 	this);
	this->textInput->Bind(wxEVT_KEY_DOWN, &Frame::onKeyDown, 	this);
	this->textInput->Bind(wxEVT_KEY_UP,   &Frame::onKeyUp, 		this);
	this->timer    ->Bind(wxEVT_TIMER,    &Frame::onTimer,		this);
	this->			 Bind(EVT_READY_PASTE,&Frame::onPaste, 		this);
	// this->			 Bind(EVT_KEY_SHOW,	  &Frame::goShow, 		this);
}

void Frame::initSizer(){
	wxFont outputFont = this->textOutput->GetFont();
	outputFont.SetPointSize(this->OUTPUT_FONTSIZE);
	this->textOutput->SetFont(outputFont);
	wxFont inputFont = this->textInput->GetFont();
	inputFont.SetPointSize(INPUT_FONTSIZE);
	this->textInput->SetFont(inputFont);

	this->panel 	 ->SetSizer(this->moduleSizer);
	this->moduleSizer->AddSpacer(this->PANEL_VPAD);
	this->moduleSizer->Add(this->textInput,  0, wxALIGN_CENTER, 5);
	this->moduleSizer->AddSpacer(this->PANEL_VPAD);
	this->moduleSizer->Add(this->textOutput, 0, wxALIGN_CENTER, 5);
	this->moduleSizer->AddSpacer(this->PANEL_VPAD);
}

void Frame::refitSizer(){
	this->moduleSizer->SetSizeHints(this);
	this->panel 	 ->Layout();
	this->moduleSizer->Fit(this->panel);
}

void Frame::readConfig(){
	std::ifstream file(this->execDir+(std::string)"/config.txt");
	std::vector<std::string> vec;
	if (file.is_open()){
		std::string line;
		while (std::getline(file, line)) {
			if((line.length()>0) && (line.front()!='/')){
				vec = this->splitString(line, ":");
				int value = std::stoi(vec.back());
				if     (vec.front() == "INPUT_WIDTH")    this->INPUT_WIDTH = value;
				else if(vec.front() == "INPUT_FONTSIZE") this->INPUT_FONTSIZE = value;
				else if(vec.front() == "OUTPUT_FONTSIZE")this->OUTPUT_FONTSIZE = value;
				else if(vec.front() == "PANEL_VPAD")     this->PANEL_VPAD = value;
				else if(vec.front() == "KEYBOARD_TIMER") this->KEYBOARD_TIMER = value;
				else if(vec.front() == "PASTE_WAIT")     this->PASTE_WAIT = value;
				else if(vec.front() == "DICT_HEIGHT")    this->DICT_HEIGHT = value;
				else if(vec.front() == "DICT_VPAD")      this->DICT_VPAD = value;
			}
    	}
	}
	file.close();
}

void Frame::onKeyDown(wxKeyEvent& event){
	int key = event.GetKeyCode();
	if     (key == WXK_ESCAPE) this->goHide();
	// if     (key == WXK_ESCAPE) this->onQuit();
	else if(key == WXK_RETURN) this->onEnter();
	// move dict cursor
	else if(key == WXK_DOWN)   this->movDict(1);
	else if(key == WXK_UP)     this->movDict(-1);
	else event.Skip();
}

void Frame::movDict(int dir){
	if(this->dictPopup->IsShown()) this->dictPopup->incrCur(dir);
}

void Frame::onKeyUp(wxKeyEvent& event){
	this->val      = this->textInput->GetValue();
	this->rawInput = (std::string)this->val.mb_str();

	std::string evaluated = this->pyth->evalString(this->rawInput);
	wxString converted(evaluated.c_str(), wxConvUTF8);
	this->uni = converted;
	this->textOutput->SetLabel(this->uni);

	this->processInput();
	std::string list = this->pyth->getList(this->inputMid);
	this->listVec = this->splitString(list,";");
	this->checkDict();

	this->refitSizer();
	// this->repositionPopup();
	event.Skip();
}

void Frame::onLeftClick(wxMouseEvent& event){
	this->lastMousePos = wxGetMousePosition() - this->panel->GetScreenPosition();
}

void Frame::onDrag(wxMouseEvent& event){
	if (event.LeftIsDown()) Move(wxGetMousePosition() - this->lastMousePos);
	if (this->dictPopup->IsShown()) this->repositionPopup();
}

void Frame::onEnter(){
	if(!(this->dictPopup->IsShown()) || (this->dictPopup->getSelectCur()==-1)){
		this->onReadyPaste();
	}else{
		std::string toAppend = this->splitString(this->dictPopup->getSelectString()," ").back();
		this->textInput->Clear();
		this->textInput->AppendText(this->inputHead);
		this->textInput->AppendText(toAppend);
		this->textInput->AppendText(this->inputTail);
		this->dictPopup->resetCur();
	}
}

void Frame::onReadyPaste(){
	this->goHide();
	wxCommandEvent event(EVT_READY_PASTE);
	wxPostEvent(this, event);
}

void Frame::onPaste(wxEvent& event){
	if (wxTheClipboard->Open()){
    	wxTheClipboard->SetData(new wxTextDataObject(this->uni));
    	wxTheClipboard->Close();
    	std::this_thread::sleep_for(std::chrono::milliseconds(this->PASTE_WAIT));
    	xdo_send_keysequence_window(this->xdo, CURRENTWINDOW, "Ctrl+v", 0);
	}
	event.Skip();
}

void Frame::onTimer(wxEvent& event){
	bool isKey = this->keyChecker->checkKey();
	if(isKey && !this->IsShown()) this->goShow();
	event.Skip();
}

void Frame::checkDict(){
	if(this->listVec.size() > 0){
		this->repositionPopup();
		if(!this->dictPopup->IsShown()) this->dictPopup->goShow();
		this->dictPopup->setItems(this->listVec);
	}else{
		this->dictPopup->goHide();
	}
}

void Frame::repositionPopup(){
	int leftX = this->textInput->GetScreenPosition().x;
	int topY  = this->panel->GetScreenPosition().y;
	int height = this->panel->GetSize().GetHeight();
	int popupWidth = this->INPUT_WIDTH, popupHeight = this->DICT_HEIGHT, pad = this->DICT_VPAD;
	wxPoint popupLoc;
	if (topY + height + popupHeight + pad > this->displayHeight) 
		  popupLoc = wxPoint(leftX, topY - popupHeight - pad);
	else  popupLoc = wxPoint(leftX, topY + height + pad);
	this->dictPopup->reposition(popupLoc);
}

void Frame::goHide(){
	this->dictPopup->goHide();
	this->Show(false);
}

void Frame::goShow(wxEvent& event){
	this->goShow();
}

void Frame::goShow(){
	this->Show(true);
	this->textInput->SetFocus();
}

void Frame::onQuit(){
	this->pyth   ->onExit();
	this->taskBar->Destroy();
    Close(true);
}

void Frame::processInput(){
	int caretPos = this->textInput->GetInsertionPoint();
	if (caretPos == 0) {
		this->inputHead = "";
		this->inputMid  = "";
		this->inputTail = this->rawInput;
	}else{
		std::string rawHead = (std::string)this->textInput->GetRange(0,caretPos).mb_str();
		this->inputTail     = (std::string)this->textInput->GetRange
		                      (caretPos,this->textInput->GetLineLength(0)).mb_str();

		//if '\' is not in rawHead
		if (rawHead.find('\\') != std::string::npos){ 
			std::vector<std::string> inpVec = this->splitString(rawHead, "\\");
			this->inputMid = "\\" + inpVec.back();

			if(inpVec.size()>1){
				inpVec.erase(inpVec.begin()+inpVec.size()-1);
				std::string tempst = ""; for(std::string s : inpVec) tempst += s + "\\";
				if(tempst.length()>0) tempst.pop_back();
				this->inputHead = tempst;
			}else{
				this->inputHead = "";
			}
		}else{
			this->inputMid = "";
			this->inputHead = rawHead;
		}
	}
}


std::vector<std::string> Frame::splitString(std::string str, std::string token){
    std::vector<std::string> result;
    while(str.size()){
        int index = str.find(token);
        if(index!=std::string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}