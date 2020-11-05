#include <Python.h>
#include <string>
#include <iostream>

class PyMod{
	public:
		PyMod(std::string execPath);
		std::string evalString(std::string st);
		std::string getList(std::string inp);
		void onExit();

	private:
		PyObject* pModule;
		PyObject* pEvalFunc;
		PyObject* pListFunc;

		std::string evalModule(PyObject* pF, std::string st);

};