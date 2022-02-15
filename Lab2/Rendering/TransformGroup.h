#ifndef TRANSFORM_GROUP_H
#define TRANSFORM_GROUP_H

#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Select_Browser.H>
#include "GUI/Button.h" 


class TransformGroup : public Fl_Group{
protected: 

	Fl_Group* _regtrans; 
	Fl_Group* _srotgroup; 

	Fl_Float_Input* _transx; 
	Fl_Float_Input* _transy; 
	Fl_Float_Input* _scale; 
	Fl_Float_Input* _nuscalex; 
	Fl_Float_Input* _nuscaley; 
	Fl_Float_Input* _rot; 

	Button* _appbut; 

	Button* _transbut; 
	Button* _rotatebut; 
	Button* _scalebut; 
	Button* _nuscalebut; 

/*
	Fl_Float_Input* _specrot; 
	Button* _specrotbut; 
*/

public: 
	TransformGroup(int x, int y, int w, int h); 
	~TransformGroup(){
		delete _regtrans; 
		delete _srotgroup; 
		delete _transx; 
		delete _transy; 
		delete _scale; 
		delete _nuscalex; 
		delete _nuscaley; 
		delete _rot; 
		delete _appbut; 
		delete _transbut; 
		delete _rotatebut; 
		delete _scalebut; 
		delete _nuscalebut; 
	}

	Button* getTranslateBut() { return _transbut; }
	Button* getRotateBut() { return _rotatebut; }
	Button* getScaleBut() { return _scalebut; }
	Button* getNUScaleBut() { return _nuscalebut; }

	static void applyTranslateCb(Fl_Widget* widget,void* userdata); 
	static void applyRotateCb(Fl_Widget* widget,void* userdata); 
	static void applyScaleCb(Fl_Widget* widget,void* userdata); 
	static void applyNUScaleCb(Fl_Widget* widget,void* userdata); 
}; 

class TransformBrowser : public Fl_Select_Browser{
protected: 
public: 
	TransformBrowser(int x,int y, int w, int h); 
}; 

#endif