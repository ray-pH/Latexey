#include "pymod.h"

PyMod::PyMod(std::string execPath){
    Py_Initialize();
    std::string path = (std::string)"sys.path.append(\""  + execPath + (std::string)"\")";

    PyRun_SimpleString("import sys");
    PyRun_SimpleString(path.c_str());
    this->pModule   = NULL;
    this->pEvalFunc = NULL;
    this->pListFunc = NULL;
    this->pModule   = PyImport_ImportModule("helper");
    this->pEvalFunc = PyObject_GetAttrString(pModule, "getReplace");
    this->pListFunc = PyObject_GetAttrString(pModule, "getList");
}


std::string PyMod::evalModule(PyObject* pF, std::string arg){
    PyObject * pArgs = PyTuple_New(1);
    PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(arg.c_str()));
    PyObject * pResult = PyEval_CallObject(pF, pArgs);
    std::string res = PyUnicode_AsUTF8(pResult);
    return res;
}

std::string PyMod::evalString(std::string st){
    std::string res = evalModule(this->pEvalFunc, st);
    return res;
}

std::string PyMod::getList(std::string inp){
    std::string res = evalModule(this->pListFunc, inp);
    return res;
}

void PyMod::onExit(){
    Py_Finalize();
}