#ifndef IFS_VIEWER_H
#define IFS_VIEWER_H

#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <FL/glut.H>
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include "Common/TinyGeom.h"
#include "Rendering/BaseGrid.h"
#include "Rendering/Transformation.h"
#include "Rendering/IFSViewer.h"
#include "Rendering/Manager.h"
#include "Rendering/TransformGroup.h"

#include <list>
#include <map>
#include <fstream>
#include <set>

using namespace std;
using namespace TinyGeom;

#define REFRESH_RATE .001

class IFSViewer : public Fl_Gl_Window{
protected:
	int _w,_h;

	Pt2 _dspaceLL, _dspaceUR; // drawing space lower left corner and upper left corner

	Tri2* _selected;
	Tri2* _highlighted;
	Pt2* _selectedPt;
	Pt2* _highlightedPt;
	Pt2 _prevpos;

	Pt2 _snappos;
	Tri2 _snapvs;
	bool _snapped;

	bool _panning;
	bool _zooming;

	bool _doSnap;

	set<Tri2*> _editing;

	TransformManager _tmanager;
	TransformEntry* _tentry;

	BaseGrid* _transgrid;
	TransformBrowser* _tbrowser;

	map<Tri2*,Color> _t2color;

	bool _baseEdit;

	inline int getWidth() { return _w; }
	inline int getHeight() { return _h; }

	Pt2 win2Screen(int x, int y);

	void defaultView(){
		int w = getWidth();
		int h = getHeight();
		_dspaceLL[0] = -w/2.;
		_dspaceLL[1] = -h/2.;

		_dspaceUR[0] = w/2.;
		_dspaceUR[1] = h/2.;
	}

	void setAsEntry(TransformEntry* ent);

public:
	IFSViewer(int x, int y, int w, int h, const char* l=0);
	~IFSViewer();
	void draw();
	int handle(int flag);
	void init();

	static void updateCb(void* userdata){
		IFSViewer* viewer = (IFSViewer*) userdata;
		viewer->redraw();
		Fl::repeat_timeout(REFRESH_RATE,IFSViewer::updateCb,userdata);
	}

	void setBaseGrid(BaseGrid* tg) {
		_transgrid = tg;
		_transgrid->setBase(_tentry->getBase());
	}

	void setTransformBrowser(TransformBrowser* tb) {
		_tbrowser = tb;
		updateBrowser();
	}

	void updateBrowser(){
		if(_tbrowser){
			list<string> names = _tmanager.getEntryNames();
			_tbrowser->clear();
			for(list<string>::iterator i=names.begin();i!=names.end();i++)
				_tbrowser->add(i->c_str());
			string name = _tmanager.getName(_tentry);
			for(int j=0;j<_tbrowser->size();j++){
				if(string(_tbrowser->text(j+1))==name){
					_tbrowser->select(j+1);
					break;
				}
			}
		}
	}

	void resize(int x, int y, int width, int height);
	void set2DProjection();

	list<Transformation*> getTransforms() const {
		list<Transformation*> ret;
		for(map<Tri2*,Transformation*>::const_iterator i=_tentry->getTri2Trans()->begin();
			i!=_tentry->getTri2Trans()->end();i++)
			ret.push_back(i->second);
		return ret;
	}

	list<Tri2*> getEditingTris() {
		list<Tri2*> ret;
		for(map<Tri2*,Transformation*>::const_iterator i=_tentry->getTri2Trans()->begin();
			i!=_tentry->getTri2Trans()->end();i++){
			if(_editing.find(i->first)!=_editing.end())
				ret.push_back(i->first);
		}
		return ret;
	}

	static void addOneTransformCb(Fl_Widget* widget,void* userdata);
	static void delEditingTransformsCb(Fl_Widget* widget,void* userdata);
	static void applyIFSCb(Fl_Widget* widget,void* userdata);
	static void saveCurrentIFSCb(Fl_Widget* widget,void* userdata);
	static void delCurrentIFSCb(Fl_Widget* widget,void* userdata);
	static void IFSBrowserSelectCb(Fl_Widget* widget, void* userdata);
	static void defaultViewCb(Fl_Widget*, void* userdata);
	static void saveIFSsToFile(Fl_Widget*, void* userdata);
	static void openIFSsToFile(Fl_Widget*, void* userdata);

	static void toggleBaseMode(Fl_Widget* w, void* userdata){
		IFSViewer* viewer = (IFSViewer*) userdata;
		Button* b = (Button*) w;
		if(viewer){
			viewer->_baseEdit = !viewer->_baseEdit;
			if(viewer->_baseEdit)
				w->label("Base On");
			else
				w->label("Base Off");
		}
	}

	static void toggleSnap(Fl_Widget* w, void* userdata){
		IFSViewer* viewer = (IFSViewer*) userdata;
		Button* b = (Button*) w;
		if(viewer){
			viewer->_doSnap = !viewer->_doSnap;
			if(viewer->_doSnap)
				w->label("Snap On");
			else
				w->label("Snap Off");
		}
	}

	friend class TransformGroup;
};


#endif 