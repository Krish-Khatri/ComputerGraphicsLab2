#ifndef GEOMETRY_VIEWER_H
#define GEOMETRY_VIEWER_H

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h> 
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include "Common/TinyGeom.h" 

#include "GUI/Button.h" 

#include "Rendering/BaseGrid.h" 
#include "Rendering/Manager.h" 

#include <list> 
#include <map>
#include <fstream> 

using namespace std; 
using namespace TinyGeom; 

#define REFRESH_RATE .001

class GeometryViewer : public Fl_Gl_Window{
protected: 
	int _w,_h; 

	Pt2 _dspaceLL, _dspaceUR; // drawing space lower left corner and upper left corner

	map<Pt2*,Geom2*> _p2geom; 
	Geom2* _selected; 
	Geom2* _highlighted; 
	Pt2* _selectedPt; 
	Pt2* _highlightedPt; 

	bool _panning; 
	bool _zooming; 
	bool _showGrid; 

	set<Geom2*> _editing; 
	GeometryHistory _geomhist; 
	Pt2 _prevpos; 

	BaseGrid* _transgrid; 
	inline int getWidth() { return _w; } 
	inline int getHeight() { return _h; } 

	void addGeom(Geom2* g); 
	Pt2 win2Screen(int x, int y); 

	void defaultView(){
		int w = getWidth(); 
		int h = getHeight(); 
		_dspaceLL[0] = -w/2.; 
		_dspaceLL[1] = -h/2.; 

		_dspaceUR[0] = w/2.; 
		_dspaceUR[1] = h/2.;
	}

public: 
	GeometryViewer(int x, int y, int w, int h, const char* l=0);
	~GeometryViewer(); 
	void draw(); 
	int handle(int flag); 
	void init();

	static void updateCb(void* userdata){
		GeometryViewer* viewer = (GeometryViewer*) userdata; 
		viewer->redraw(); 
		Fl::repeat_timeout(REFRESH_RATE,GeometryViewer::updateCb,userdata);
	}

	void setBaseGrid(BaseGrid* tg) { 
		_transgrid = tg; 
	}

	GeometryHistory* getGeomHistory() { return &_geomhist; }

	void resize(int x, int y, int width, int height);
	void set2DProjection(); 

	void prepareGeom(list<pair<Geom2*,Color> >* lg){
		_editing.clear(); 
		_p2geom.clear(); 
		for(list<pair<Geom2*,Color> >::const_iterator i=lg->begin();i!=lg->end();i++)
			for(int j=0;j<i->first->size();j++)
				_p2geom[i->first->get(j)] = i->first; 
	}

	static void saveImageBufferCb(Fl_Widget* widget,void* userdata); 
	static void addShapeCb(Fl_Widget* widget,void* userdata); 
	static void delEditingShapesCb(Fl_Widget* widget,void* userdata); 
	static void undoCb(Fl_Widget*, void* userdata); 
	static void defaultViewCb(Fl_Widget*, void* userdata); 
	static void toggleGridCb(Fl_Widget* w, void* userdata){
		GeometryViewer* ov = (GeometryViewer*) userdata; 
		Button* b = (Button*) w; 
		if(ov){
			ov->_showGrid = !ov->_showGrid;
			if(ov->_showGrid)
				b->label("Grid Off"); 
			else
				b->label("Grid On"); 
		}
	}

	friend class IFSViewer; 
};


#endif 