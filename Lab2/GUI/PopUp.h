#ifndef POPUP_H
#define POPUP_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include "Common/Common.h"
#include "GUI/Button.h" 
#include <FL/Fl_Output.H>

#include <iostream> 

using namespace std; 

class NamePopup : public Fl_Window{
protected: 
	static NamePopup* _popup; 

	Button* _ok; 
	Button* _cancel; 
	Fl_Input* _input; 

	bool  _canceled; 

public: 

	NamePopup(); 
	static NamePopup* singleton() { 
		if(!_popup) 
			_popup = new NamePopup(); 
		return _popup; 
	}

	static bool isOk() { 
		return !singleton()->_canceled; 
	}

	static string query(int x, int y){
		singleton()->resize(x,y,singleton()->w(),singleton()->h()); 
		singleton()->show(); 
		singleton()->redraw(); 
		while(singleton()->shown()); 
		//singleton()->modal();

		if(singleton()->_canceled)
			return ""; 
		return string(singleton()->_input->value()); 
	}

	static void okPressed(Fl_Widget* widget, void* userdata){
		singleton()->_canceled = false; 
		singleton()->hide(); 
	}

	static void cancelPressed(Fl_Widget* widget, void* userdata){
		singleton()->_canceled = true; 
		singleton()->hide(); 
	}
};

#endif