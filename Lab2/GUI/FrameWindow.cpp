#include "GUI/FrameWindow.h" 
#include "Common/Common.h" 

#include <iostream> 

using namespace std; 

const int WIN_LOWER_SPACE = 30; 

FrameWindow::FrameWindow(int x, int y, int w, int h, const char* l) 
 : Fl_Window(x,y,w,h+WIN_LOWER_SPACE,l){
	this->callback(escapeButtonCb,this);
	this->color(WIN_COLOR);
}

FrameWindow::~FrameWindow(){}

void FrameWindow::escapeButtonCb(Fl_Widget* widget, void* win){
	// comes here when the window "x" button is pushed
	exit(1);
}