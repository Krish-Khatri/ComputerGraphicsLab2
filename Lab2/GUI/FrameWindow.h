#ifndef FRAME_WINDOW_H
#define FRAME_WINDOW_H

#include <FL/Fl_Window.H>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <FL/glut.h> 
#include <FL/glu.h>

#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Button.H> 
#include "Common/Common.h"

#include <vector> 

class FrameWindow : public Fl_Window{
protected: 
public: 
	FrameWindow(int x, int y, int w, int h, const char* l=0); 
	~FrameWindow();
protected: 
	static void escapeButtonCb(Fl_Widget* widget, void* win); 
}; 

#endif