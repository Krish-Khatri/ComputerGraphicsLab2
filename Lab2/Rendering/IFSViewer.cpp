#include "Rendering/IFSViewer.h"
#include "Rendering/GeometryViewer.h"
#include "Common/TinyGeom.h" 
#include <FL/gl.h> 
#include <FL/fl_draw.H> 
#include <GL/glu.h>
#include "GUI/PopUp.h"

#include <iostream>
#include <fstream> 
using namespace std; 

extern "C"{
#include "Common/bmpfile.h"
}


const int GCW[3] = {
	glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'a'),
	glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'b'),
	glutBitmapWidth(GLUT_BITMAP_HELVETICA_18,'c')}; 

const int GCH = glutBitmapHeight(GLUT_BITMAP_HELVETICA_18); 

using namespace TinyGeom; 

void IFSViewer::setAsEntry(TransformEntry* ent){
	IFSViewer* viewer = this; 
	viewer->_tentry = ent; 
	viewer->_transgrid->setBase(viewer->_tentry->getBase()); 
	viewer->_editing.clear(); 
	viewer->_t2color.clear(); 
	list<Tri2*>* tris = viewer->_tentry->getGeoms(); 
	for(list<Tri2*>::iterator i=tris->begin();i!=tris->end();i++){
		double r = min(1,rand()/(1.*RAND_MAX)+.1); 
		double g = min(1,rand()/(1.*RAND_MAX)+.1); 
		double b = min(1,rand()/(1.*RAND_MAX)+.1); 
		viewer->_t2color[*i] = Color(r,g,b); 
	}
}

IFSViewer::IFSViewer(int x, int y, int w, int h, const char* l)
: Fl_Gl_Window(x,y,w,h,l){
	Fl::repeat_timeout(REFRESH_RATE,IFSViewer::updateCb,this);
	_w = w; 
	_h = h; 
	_selected = NULL; 
	_highlighted = NULL;  
	_selectedPt = NULL; 
	_highlightedPt = NULL; 
	_transgrid = NULL; 

	_tbrowser = NULL; 
	_tentry = _tmanager.newEntry("default"); 
	Tri2* tri = _tentry->getBase(); 
	(*tri->get(0)) = Pt2(0,0); 
	(*tri->get(1)) = Pt2(w/2*.8,0); 
	(*tri->get(2)) = Pt2(0,h/2*.8); 

	_dspaceLL = Pt2(-w/2,-h/2); 
	_dspaceUR = Pt2(w/2,h/2); 

	_baseEdit = false; 
	_doSnap = true;  
}

IFSViewer::~IFSViewer(){
	Fl::remove_timeout(IFSViewer::updateCb,this); 
}

void IFSViewer::init(){
	glClearColor(0,0,0,1); 
	glMatrixMode(GL_PROJECTION); 
	glOrtho(0,this->w(),0,this->h(),-100,100); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 
	glDisable(GL_DEPTH_TEST); 
	glLineWidth(1.f); 
	glPointSize(8.f); 
	glEnable(GL_POINT_SMOOTH); 
	glEnable(GL_POLYGON_SMOOTH); 
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 

	set2DProjection(); 
}

void IFSViewer::set2DProjection(){
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(_dspaceLL[0], _dspaceUR[0], _dspaceLL[1], _dspaceUR[1]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void IFSViewer::draw(){
	if(!valid())
		init();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	set2DProjection(); // necessary for 2d drawing
	glDisable(GL_LIGHTING);

	if(_transgrid){
		if(_baseEdit)
			glColor4d(1.,0,0,1); 
		else
			glColor4d(1.,1,1,1); 

		const list<pair<int,int> >& edges = _transgrid->getEdges(); 
		const vector<Pt2>& pts = _transgrid->getPts(); 
		Tri2* base = _tentry->getBase(); 

		glBegin(GL_LINES); 
		for(list<pair<int,int> >::const_iterator i=edges.begin(); i!=edges.end(); i++){
			glVertex2d(pts[i->first][0],pts[i->first][1]); 
			glVertex2d(pts[i->second][0],pts[i->second][1]); 
		}
		glEnd(); 

		for(int j=0;j<base->size();j++){
			glRasterPos2d((*base->get(j))[0]-(GCW[j]/2),
				(*base->get(j))[1]+(GCH/2)); 
			string str=""; 
			str+=(char)('a'+j); 
			glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) str.c_str()); 
		}

		if(_baseEdit){
			glPointSize(8); 
			glColor3d(1,0,0); 
			for(int j=0;j<base->size();j++){
				Pt2* p = base->get(j); 
				if(p==_highlightedPt){
					glBegin(GL_POINTS); 
					glVertex2d((*p)[0],(*p)[1]); 
					glEnd(); 
				}
			}
		}
	}

	list<Tri2*>* geom = _tentry->getGeoms(); 

	for(list<Tri2*>::iterator i=geom->begin();i!=geom->end();i++){ 
		glBegin(GL_POLYGON); 
		if(_t2color.find(*i)!=_t2color.end()){
			Color color = _t2color[*i]; 
			glColor4d(color[0],color[1],color[2],.7);
		}
		else
			glColor4d(.5,.5,.8,.7);; 

		for(int j=0;j<(*i)->size();j++){
			Pt2* p = (*i)->get(j); 
			glVertex2d((*p)[0],(*p)[1]); 
		}
		glEnd(); 

		if(_editing.find(*i)!=_editing.end()){
			glLineWidth(3.f); 
			glColor3f(0.,1.,0); 
			glBegin(GL_LINE_LOOP); 
			for(int j=0;j<(*i)->size();j++){
				Pt2* p = (*i)->get(j); 
				glVertex2d((*p)[0],(*p)[1]); 
			}
			glEnd(); 
			glLineWidth(1.f); 
		}

		if(*i==_highlighted){
			glLineWidth(2.f); 
			glColor3f(1.,0,0); 
			glBegin(GL_LINE_LOOP); 
			for(int j=0;j<(*i)->size();j++){
				Pt2* p = (*i)->get(j); 
				glVertex2d((*p)[0],(*p)[1]); 
			}
			glEnd(); 

			glColor3d(1.,0.,0.);  
			for(int j=0;j<(*i)->size();j++){
				glRasterPos2d((*(*i)->get(j))[0]-(GCW[j]/2),
					(*(*i)->get(j))[1]+(GCH/4)); 
				string str=""; 
				str+=(char)('a'+j); 
				glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*) str.c_str()); 
			}
			glLineWidth(1.f); 
		}

		for(int j=0;j<(*i)->size();j++){
			Pt2* p = (*i)->get(j); 
			if(p==_highlightedPt){
				glBegin(GL_POINTS); 
				glColor3f(1,0,0); 
				glVertex2d((*p)[0],(*p)[1]); 
				glEnd(); 
			}
		}
	}

	if(_tentry->getTransCenter()==_highlightedPt)
		glColor4d(1,0,0,1.); 
	else
		glColor4d(.95,.65,0,1.); 

	Pt2 tcenter = (*_tentry->getTransCenter()); 
	glPointSize(10.f); 
	glBegin(GL_POLYGON); 
	glVertex2d(tcenter[0]-11,tcenter[1]-1.5); 
	glVertex2d(tcenter[0]+11,tcenter[1]-1.5); 
	glVertex2d(tcenter[0]+11,tcenter[1]+1.5); 
	glVertex2d(tcenter[0]-11,tcenter[1]+1.5); 
	glEnd(); 
	glBegin(GL_POLYGON); 
	glVertex2d(tcenter[0]-1.5,tcenter[1]-11); 
	glVertex2d(tcenter[0]-1.5,tcenter[1]+11); 
	glVertex2d(tcenter[0]+1.5,tcenter[1]+11); 
	glVertex2d(tcenter[0]+1.5,tcenter[1]-11); 
	glEnd(); 


	swap_buffers(); 
}

Pt2 IFSViewer::win2Screen(int x, int y){
	Vec2 winv(x/(double)getWidth(),(getHeight()-y)/(double)getHeight(),0); 
	Vec2 diff = _dspaceUR-_dspaceLL; 
	diff[0]*=winv[0]; 
	diff[1]*=winv[1]; 

	return (_dspaceLL+diff); 
}

int IFSViewer::handle(int ev){
	// input in 2d mode
	if(ev==FL_PUSH){
		if(Fl::event_button()==FL_LEFT_MOUSE){
			_prevpos = win2Screen(Fl::event_x(),Fl::event_y()); 
			if(_highlighted){
				_selected = _highlighted; 
				pair<double,int> check = make_pair(10000000,-1); 
				int goodind = -1; 
				for(int j=0;j<_selected->size();j++){
					pair<double,int> tc = _transgrid->findClosest(*_selected->get(j)); 
					if(tc.first < check.first){
						check = tc; 
						goodind = j; 
					}
				}

				if(check.first==0)
					_snapped = true; 

				_snappos = _prevpos; 
				for(int j=0;j<_selected->size();j++)
					(*_snapvs.get(j)) = (*_selected->get(j))-_snappos; 
			}
			else if(_highlightedPt){
				_selectedPt = _highlightedPt; 
				if(!_baseEdit){
					pair<double,int> check = _transgrid->findClosest(*_selectedPt); 
					if(check.first==0)
						_snapped = true; 
					_snappos = _prevpos; 
				}
			}
			else
				_panning = true; 
		}
		else if(Fl::event_button()==FL_RIGHT_MOUSE){
			if(!_highlighted && !_highlightedPt){
				_prevpos = Pt2(Fl::event_x(),Fl::event_y()); 
				_zooming = true; 
			}
		}

		return 1;  // must return 1 here to ensure FL_MOVE is sent
	}
	else if(ev==FL_DRAG){
		if(Fl::event_button()==FL_LEFT_MOUSE){
			Pt2 mpos = win2Screen(Fl::event_x(),Fl::event_y()); 
			if(_selected){
				int goodind = -1; 
				const Pt2* best = NULL; 
				double bestd = 1000000; 

				for(int j=0;j<_selected->size();j++){
					pair<double,int> tc = _transgrid->findClosest(*_selected->get(j)); 
					if(tc.first < bestd){
						goodind = j; 
						bestd = tc.first;
						best = &(_transgrid->getPts()[tc.second]); 
					}
				}

				for(map<Pt2*,Tri2*>::iterator i=_tentry->getP2Geom()->begin();i!=_tentry->getP2Geom()->end();i++){
					if(i->second==_selected) continue; 
					for(int j=0;j<_selected->size();j++){
						double nd = Utils::dist2d(*_selected->get(j),*i->first); 
						if(nd<bestd){
							goodind = j; 
							bestd = nd; 
							best = i->first; 
						}
					}
				}

				if(((bestd<15 && Utils::dist2d(mpos,_snappos)<15) || (bestd<15 && !_snapped))&&_doSnap){
					if(!_snapped){
						_snapped = true; 
						Vec2 v = *best - (*_selected->get(goodind)); 
						for(int j=0;j<_selected->size();j++)
							(*_selected->get(j))+=v; 

						_snappos = (*_selected->get(0))-(*_snapvs.get(0)); 
					}
				}
				else{
					for(int j=0;j<_selected->size();j++)
						(*_selected->get(j))=mpos+(*_snapvs.get(j)); 
					_snapped = false; 
				}

				_prevpos = mpos; 
			}
			else if(_selectedPt){
				if(!_baseEdit){
					pair<double,int> check = _transgrid->findClosest(*_selectedPt); 

					bool useGrid = false; 
					double bestd=10000; 
					Pt2* best = NULL; 

					for(map<Pt2*,Tri2*>::iterator i=_tentry->getP2Geom()->begin();i!=_tentry->getP2Geom()->end();i++){
						if(i->first==_selectedPt) continue; 
						double nd = Utils::dist2d(*(i->first),mpos); 
						if(nd<bestd){
							bestd = nd; 
							best = i->first; 
						}
					}

					if(check.first<bestd){
						useGrid = true; 
						bestd = check.first; 
					}

					if(((bestd<15 && Utils::dist2d(mpos,_snappos)<15) || (bestd<15 && !_snapped))&&_doSnap){
							if(!_snapped){
								_snapped = true; 
								_snappos = useGrid ? _transgrid->getPts()[check.second] : *best; 
								(*_selectedPt) = _snappos; 
							}
					}
					else{
						(*_selectedPt) = mpos; 
						_snapped = false; 
					}
				}
				else{
					double nx = ((int)(mpos[0]/10+.5))*10; 
					double ny = ((int)(mpos[1]/10+.5))*10; 
					(*_selectedPt) = Pt2(nx,ny); 
					_transgrid->setBase(_tentry->getBase()); 
				}

				_prevpos = mpos; 
			}
			else if(_panning){
				Vec2 change = mpos-_prevpos; 
				_dspaceLL-=change; 
				_dspaceUR-=change; 
			}
		}
		else if(Fl::event_button()==FL_RIGHT_MOUSE){
			if(_zooming){
				Pt2 mpos(Fl::event_x(),Fl::event_y()); 
				double diffy = _prevpos[1]-mpos[1]; 
				Pt2 center = (_dspaceLL+_dspaceUR)*.5; 
				Vec2 lv = _dspaceLL-center; 
				Vec2 rv = _dspaceUR-center; 

				double fac = 1-(diffy)/500; 
				_dspaceLL = center+(fac*lv); 
				_dspaceUR = center+(fac*rv); 
				_prevpos = mpos; 
			}
		}
	}
	else if(ev==FL_RELEASE){
		if(Fl::event_button()==FL_RIGHT_MOUSE){
			if(_highlighted){
				if(_editing.find(_highlighted)==_editing.end()){
					_editing.insert(_highlighted); 
				}
				else{
					_editing.erase(_highlighted); 
				}
			}
		}

		if(_selected!=NULL){
			(*_tentry->getTri2Trans())[_selected]->setAs3PtTransform(*_tentry->getBase(),*_selected); 
		}

		if(_selectedPt!=NULL && !_baseEdit && _selectedPt!=_tentry->getTransCenter()){
			Tri2* curt = (*_tentry->getP2Geom())[_selectedPt]; 
			(*_tentry->getTri2Trans())[curt]->setAs3PtTransform(*_tentry->getBase(),*curt); 
		}

		if(_baseEdit){
			list<Tri2*>* geoms = _tentry->getGeoms(); 
			for(list<Tri2*>::iterator i=geoms->begin();i!=geoms->end();i++){
				(*_tentry->getTri2Trans())[*i]->setAs3PtTransform(*_tentry->getBase(),*(*i)); 	
			}
		}

		_selected = NULL; 
		_highlighted = NULL; 
		_selectedPt = NULL; 
		_highlightedPt = NULL; 
		_snapped = false; 
		_panning = false; 
		_zooming = false; 
	}
	else if(ev==FL_MOVE){
		Pt2 mpos = win2Screen(Fl::event_x(),Fl::event_y()); 
		double ratio = Utils::dist2d(_dspaceLL,_dspaceUR)/600;

		if(!_baseEdit){
			_highlightedPt = NULL; 
			double bestd=10000; 
			Pt2* best = NULL; 
			for(map<Pt2*,Tri2*>::iterator i=_tentry->getP2Geom()->begin();i!=_tentry->getP2Geom()->end();i++){
				double nd = Utils::dist2d(*(i->first),mpos); 
				if(nd<bestd){
					bestd = nd; 
					best = i->first; 
				}
			}
			if(best && bestd<5*ratio)
				_highlightedPt = best; 


			_highlighted = NULL; 
			if(_highlightedPt==NULL){
				for(list<Tri2*>::reverse_iterator i=_tentry->getGeoms()->rbegin();i!=_tentry->getGeoms()->rend();i++){
					if(Utils::isPtInterior(*i,mpos)){
						_highlighted = *i; 
						break; 
					}
				}
			}

		}
		else{
			_highlightedPt = NULL; 
			double bestd=10000; 
			Pt2* best = NULL; 
			Tri2* t = _tentry->getBase(); 

			for(int j=0;j<3;j++){
				const Pt2* p = t->get(j); 
				double d = Utils::dist2d(*p,mpos); 
				if(d<bestd){
					bestd = d; 
					best = t->get(j); 
				}
			}

			if(best && bestd<5*ratio)
				_highlightedPt = best; 
		}
	}
	else if(ev==FL_KEYDOWN){}
	else if(ev==FL_KEYUP){
		if(Fl::event_key()==FL_Delete){
			cout<<"deleting"<<endl;
		}
		return 1; 
	}

	return Fl_Gl_Window::handle(ev);
}

void IFSViewer::resize(int x, int y, int width, int height){
	make_current();
	_w = width;
	_h = height; 

	Fl_Gl_Window::resize(x,y,width,height);
}


void IFSViewer::addOneTransformCb(Fl_Widget* widget,void* userdata){
	IFSViewer* viewer = (IFSViewer*) userdata; 
	if(viewer){
		int nw = viewer->getWidth() * .7; 
		int nh = viewer->getHeight() * .7; 

		int nx = rand()%nw - nw/2; 
		int ny = rand()%nh - nh/2; 

		Tri2* t = viewer->_tentry->getBase(); 

		double val = pow(.5,viewer->_transgrid->level()); 
		Vec2 v1 = val*(*t->get(1) - *t->get(0)); 
		Vec2 v2 = val*(*t->get(2) - *t->get(0)); 

		Tri2* nt = new Tri2(*t->get(0),v1+*t->get(0),v2+*t->get(0)); 

		double r = rand()/(1.*RAND_MAX)+.1;
		double g = rand()/(1.*RAND_MAX)+.1;
		double b = rand()/(1.*RAND_MAX)+.1; 
		r = min(r,1.); 
		g = min(g,1.); 
		b = min(b,1.); 
		viewer->_t2color[nt] = Color(r,g,b); 
		viewer->_tentry->add(nt); 
	}
}

void IFSViewer::delEditingTransformsCb(Fl_Widget* widget,void* userdata){
	IFSViewer* viewer = (IFSViewer*) userdata; 
	if(viewer){
		for(set<Tri2*>::iterator i=viewer->_editing.begin();i!=viewer->_editing.end();i++)
			viewer->_tentry->remove(*i); 
	}
}



void IFSViewer::applyIFSCb(Fl_Widget* widget,void* userdata){
	pair<GeometryViewer*,IFSViewer*>* viewers = (pair<GeometryViewer*,IFSViewer*>*) userdata; 
	GeometryViewer* ov = (GeometryViewer*) viewers->first; 
	IFSViewer* tv = (IFSViewer*) viewers->second; 

	list<pair<Geom2*,Color> >* geoms = ov->getGeomHistory()->getTop(); 
	list<Transformation*> trans = tv->getTransforms(); 
	list<pair<Geom2*,Color> >* ngeoms = ov->getGeomHistory()->pushNew(); 
	for(list<pair<Geom2*,Color> >::iterator i=geoms->begin();i!=geoms->end();i++){
		for(list<Transformation*>::iterator j=trans.begin();j!=trans.end();j++){
			Geom2* ng; 
			i->first->accept(*j,&ng); 
			ngeoms->push_back(make_pair(ng,i->second)); 
		}
	}

	ov->prepareGeom(ngeoms); 
}

void IFSViewer::saveCurrentIFSCb(Fl_Widget* widget,void* userdata){
	IFSViewer* tv = (IFSViewer*) userdata; 

	if(tv){
		while(true){
			const char* ret = fl_input("Enter a name",NULL); 
			if(ret){			
				string sret(ret); 
				if(tv->_tmanager.getEntry(sret)!=NULL){
					cout<<"Name already exists"<<endl;
				}
				else{
					TransformEntry* oldent = tv->_tentry; 
					TransformEntry* newent = tv->_tmanager.newEntry(sret); 
					tv->_transgrid->setBase(tv->_tentry->getBase()); 

					newent->set(oldent); 
					tv->setAsEntry(newent); 
					tv->updateBrowser(); 

					break;
				}
			}
			else{
				break; 
			}
		}
	}
}

void IFSViewer::delCurrentIFSCb(Fl_Widget* widget,void* userdata){
	IFSViewer* viewer = (IFSViewer*) userdata; 

	if(viewer){
		int selected = viewer->_tbrowser->value(); 
		if(selected>0 && viewer->_tbrowser->size()>1){
			string name(viewer->_tbrowser->text(selected)); 
			viewer->_tbrowser->remove(selected); 
			viewer->_tbrowser->select(1); 

			viewer->_tmanager.removeEntry(name); 
			name = string(viewer->_tbrowser->text(1)); 
			TransformEntry* ent = viewer->_tmanager.getEntry(name); 
			viewer->setAsEntry(ent); 
		}
	}
}

void IFSViewer::IFSBrowserSelectCb(Fl_Widget* widget, void* userdata){
	IFSViewer* viewer = (IFSViewer*) userdata; 

	if(viewer){
		int selected = viewer->_tbrowser->value(); 
		if(selected>0){
			string name(viewer->_tbrowser->text(selected)); 
			TransformEntry* ent = viewer->_tmanager.getEntry(name); 
			if(ent)
				viewer->setAsEntry(ent); 
			else
				cout<<"error in browser selection"<<endl;
		}
	}
}

void IFSViewer::defaultViewCb(Fl_Widget*, void* userdata){
	IFSViewer* viewer = (IFSViewer*) userdata; 
	if(viewer)
		viewer->defaultView(); 
}

void IFSViewer::saveIFSsToFile(Fl_Widget*, void* userdata){
	IFSViewer* viewer = (IFSViewer*) userdata; 

	if(viewer){
		char* newfile = fl_file_chooser("Save IFS", ".txt (*.txt)", "./files", 0);
		if(!newfile) {cout<<"Save IFS canceled"<<endl; return;}

		fstream outf(newfile,ios::out); 
		list<string> names = viewer->_tmanager.getEntryNames(); 
		outf<<names.size()<<endl;  // first line is the number of IFSs

		for(list<string>::iterator j=names.begin();j!=names.end();j++){
			outf<<*j<<endl; // output the name of this IFS
			TransformEntry* ent = viewer->_tmanager.getEntry(*j); 

			// output the base triangle on the next line
			Tri2* tri = ent->getBase(); 
			for(int k=0;k<tri->size();k++){
				Pt2* p = tri->get(k); 
				outf<<(*p)[0]<<" "<<(*p)[1]<<" "; 
			}
			cout<<endl;

			list<Tri2*>* tris = ent->getGeoms(); 
			outf<<tris->size()<<endl; // output the number of transformations in this IFS

			// output the triangles in the IFS
			for(list<Tri2*>::iterator i=tris->begin();i!=tris->end();i++){
				Tri2* tri = *i; 
				for(int k=0;k<tri->size();k++){
					Pt2* p = tri->get(k); 
					outf<<(*p)[0]<<" "<<(*p)[1]<<" "; 
				}
				outf<<endl;
			}
		}
		outf.close(); 
	}
}

void IFSViewer::openIFSsToFile(Fl_Widget*, void* userdata){
	IFSViewer* viewer = (IFSViewer*) userdata; 

	if(viewer){
		char* newfile = fl_file_chooser("Open IFS", ".txt (*.txt)", "./files", 0);
		if(!newfile) {cout<<"Open IFS canceled"<<endl; return;}

		fstream inf(newfile,ios::in); 
		int nifs = 0; 
		inf>>nifs; 

		if(nifs<1) return; 

		viewer->_tmanager.removeAllEntry(); 
		viewer->_tbrowser->clear(); 

		for(int j=0;j<nifs;j++){
			string name; 
			inf>>name; 

			TransformEntry* ent = viewer->_tmanager.newEntry(name); 

			Tri2* tri = ent->getBase(); 
			for(int i=0;i<3;i++){
				Pt2* p= tri->get(i); 
				double d0,d1; 
				inf>>d0>>d1;
				(*p) = Pt2(d0,d1); 
			}

			int ntris; 
			inf>>ntris; 
			for(int k=0;k<ntris;k++){
				Tri2* tri = new Tri2(); 
				for(int i=0;i<3;i++){
					Pt2* p= tri->get(i); 
					double d0,d1; 
					inf>>d0>>d1;
					(*p) = Pt2(d0,d1); 
				}

				ent->add(tri); 
			}

			viewer->_tbrowser->add(name.c_str()); 
		}
		viewer->_editing.clear(); 

		viewer->_tbrowser->value(1); 
		int selected = viewer->_tbrowser->value(); 
		if(selected>0){
			string name(viewer->_tbrowser->text(selected)); 
			TransformEntry* ent = viewer->_tmanager.getEntry(name); 
			if(ent)
				viewer->setAsEntry(ent); 
			else
				cout<<"error in browser selection"<<endl;
		}
	}
}