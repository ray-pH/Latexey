#include "main.h"

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit(){

	this->execDir = this->getDirPath();

	Frame* frame = new Frame(wxT("Latexey"), this->execDir);
	frame->goShow();

	return true;
}

std::string MyApp::getDirPath(){
	char result[ PATH_MAX ];
	ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
	std::string execPath = std::string( result, (count > 0) ? count : 0 );
	std::string execDir = execPath.substr(0, execPath.find_last_of("/"));
	return execDir;
}