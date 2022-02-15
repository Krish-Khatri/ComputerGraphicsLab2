#include "Rendering/TransformGroup.h" 
#include "Rendering/IFSViewer.h" 
#include "Common/Common.h" 

TransformGroup::TransformGroup(int x, int y, int w, int h): Fl_Group(x,y,w,h){

	begin(); 

	int inputw = 30; 
	int offsetx = 30; 

	_regtrans = new Fl_Group(x,y,inputw*3+offsetx*2+20,105); 
	_regtrans->color(WIN_COLOR); 
	_regtrans->box(FL_BORDER_BOX); 
	_regtrans->label("Affine transformation"); 
	_regtrans->begin(); 

	_transx = new Fl_Float_Input(x+offsetx,y+5,inputw,20,"Tx"); 
	_transy = new Fl_Float_Input(x+offsetx+inputw+20,y+5,inputw,20,"Ty"); 
	_scale = new Fl_Float_Input(x+offsetx,y+30,inputw,20,"S"); 
	_nuscalex = new Fl_Float_Input(x+offsetx,y+55,inputw,20,"Sx"); 
	_nuscaley = new Fl_Float_Input(x+offsetx+inputw+20,y+55,inputw,20,"Sy"); 
	_rot = new Fl_Float_Input(x+offsetx,y+80,inputw,20,"Rot"); 

	_transbut = new Button(x+offsetx*2+inputw*2,y+5,inputw+15,20,"Apply"); 
	_scalebut = new Button(x+offsetx*2+inputw*2,y+30,inputw+15,20,"Apply"); 
	_nuscalebut = new Button(x+offsetx*2+inputw*2,y+55,inputw+15,20,"Apply"); 
	_rotatebut = new Button(x+offsetx*2+inputw*2,y+80,inputw+15,20,"Apply"); 

	_transx->box(FL_BORDER_BOX); 
	_transy->box(FL_BORDER_BOX); 
	_scale->box(FL_BORDER_BOX); 
	_nuscalex->box(FL_BORDER_BOX); 
	_nuscaley->box(FL_BORDER_BOX); 
	_rot->box(FL_BORDER_BOX); 

	_regtrans->end(); 

	end(); 

	this->color(WIN_COLOR);

	_transx->value(Str::toString(0).c_str()); 
	_transy->value(Str::toString(0).c_str()); 
	_scale->value(Str::toString(1).c_str()); 
	_nuscalex->value(Str::toString(1).c_str()); 
	_nuscaley->value(Str::toString(0).c_str()); 
	_rot->value(Str::toString(0).c_str()); 

}


void TransformGroup::applyTranslateCb(Fl_Widget* widget,void* userdata){
	pair<IFSViewer*,TransformGroup*>* pa = (pair<IFSViewer*,TransformGroup*>*) userdata; 
	IFSViewer* viewer = pa->first; 
	TransformGroup* group = pa->second; 

	if(viewer){
		list<Tri2*> tris = viewer->getEditingTris(); 

		double tx = Str::parseDouble(string(group->_transx->value())); 
		double ty = Str::parseDouble(string(group->_transy->value())); 

		Transformation trans; 
		trans.setAsTranslate(Vec2(tx,ty,0)); 

		for(list<Tri2*>::iterator i=tris.begin();i!=tris.end();i++){
			Tri2* nt; 
			(*i)->accept(&trans,&nt); 
			(*(*i))= *nt; 
			delete nt; 

			(*viewer->_tentry->getTri2Trans())[*i]->setAs3PtTransform(
				*viewer->_tentry->getBase(),*(*i)); 
		}
	}
} 

void TransformGroup::applyRotateCb(Fl_Widget* widget,void* userdata){
	pair<IFSViewer*,TransformGroup*>* pa = (pair<IFSViewer*,TransformGroup*>*) userdata; 
	IFSViewer* viewer = pa->first; 
	TransformGroup* group = pa->second; 

	if(viewer){
		list<Tri2*> tris = viewer->getEditingTris(); 
		double rot = Str::parseDouble(string(group->_rot->value())); 

		Transformation trans; 
		trans.setAsRotate(rot,*viewer->_tentry->getTransCenter()); 

		for(list<Tri2*>::iterator i=tris.begin();i!=tris.end();i++){
			Tri2* nt; 
			(*i)->accept(&trans,&nt); 
			(*(*i))= *nt; 
			delete nt; 

			(*viewer->_tentry->getTri2Trans())[*i]->setAs3PtTransform(
				*viewer->_tentry->getBase(),*(*i)); 
		}
	}
} 

void TransformGroup::applyNUScaleCb(Fl_Widget* widget,void* userdata){
	pair<IFSViewer*,TransformGroup*>* pa = (pair<IFSViewer*,TransformGroup*>*) userdata; 
	IFSViewer* viewer = pa->first; 
	TransformGroup* group = pa->second; 

	if(viewer){
		list<Tri2*> tris = viewer->getEditingTris(); 

		double sx = Str::parseDouble(string(group->_nuscalex->value())); 
		double sy = Str::parseDouble(string(group->_nuscaley->value())); 

		Transformation trans; 
		trans.setAsNUScale(Vec2(sx,sy,0),*viewer->_tentry->getTransCenter()); 

		for(list<Tri2*>::iterator i=tris.begin();i!=tris.end();i++){
			Tri2* nt; 
			(*i)->accept(&trans,&nt); 
			(*(*i))= *nt; 
			delete nt; 

			(*viewer->_tentry->getTri2Trans())[*i]->setAs3PtTransform(
				*viewer->_tentry->getBase(),*(*i)); 
		}
	}
} 

void TransformGroup::applyScaleCb(Fl_Widget* widget,void* userdata){
	pair<IFSViewer*,TransformGroup*>* pa = (pair<IFSViewer*,TransformGroup*>*) userdata; 
	IFSViewer* viewer = pa->first; 
	TransformGroup* group = pa->second; 

	if(viewer){
		list<Tri2*> tris = viewer->getEditingTris(); 

		double s = Str::parseDouble(string(group->_scale->value())); 

		Transformation trans; 
		trans.setAsScale(s,*viewer->_tentry->getTransCenter()); 

		for(list<Tri2*>::iterator i=tris.begin();i!=tris.end();i++){
			Tri2* nt; 
			(*i)->accept(&trans,&nt); 
			(*(*i))= *nt; 
			delete nt; 

			(*viewer->_tentry->getTri2Trans())[*i]->setAs3PtTransform(
				*viewer->_tentry->getBase(),*(*i)); 
		}
	}
} 

TransformBrowser::TransformBrowser(int x, int y, int w, int h): 
Fl_Select_Browser(x,y,w,h){
	box(FL_BORDER_BOX); 
}
