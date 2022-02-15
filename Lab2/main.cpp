//#include "GUI/MainWindow.h" 
#include "Common/Common.h" 
#include "GUI/FrameWindow.h" 
#include "Rendering/GeometryViewer.h" 
#include "Rendering/IFSViewer.h" 
#include "Rendering/BaseGrid.h" 
#include "Rendering/TransformGroup.h" 
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include "GUI/Button.h" 
#include "GUI/PopUp.h"

#include <iostream> 
#include <string> 

using namespace std; 

int main() { 
	FrameWindow m(700,50,1050,670,"Lab - Transformation"); 

	int ovwidth = 500; 
	int ovheight = 500; 
	BaseGrid tg; 

	GeometryViewer ov(10,10,500,500,"opengl"); 
	ov.box(FL_FLAT_BOX); 
	ov.color(FL_BLACK); 
	ov.end(); 

	ov.setBaseGrid(&tg); 

	IFSViewer tv(530,10,500,500,"transform"); 
	tv.box(FL_FLAT_BOX); 
	tv.color(FL_BLACK); 
	tv.end(); 
	tv.setBaseGrid(&tg); 

	pair<GeometryViewer*,IFSViewer*> viewers; 
	viewers.first = &ov; 
	viewers.second = &tv; 

	TransformGroup tfgroup(540,565,165,250); 
	tfgroup.box(FL_FLAT_BOX); 

	Button* saveImageBt = new Button(20, 670, 100, 20, "Save Image");
	saveImageBt->callback(GeometryViewer::saveImageBufferCb, &ov);

	Fl_Group shapes(20,530,215,100,"Shapes");  
	shapes.box(FL_BORDER_BOX); 
	shapes.color(WIN_COLOR); 
	shapes.begin(); 

	// TODO: add buttons for your new shapes
	pair<GeometryViewer*,TGShape> shapeData1 = make_pair(&ov,TG_TRIANGLE); 
	Button* addtri = new Button(25, 535, 100, 20, "Triangle");
	addtri->callback(GeometryViewer::addShapeCb, &shapeData1);

	pair<GeometryViewer*,TGShape> shapeData2 = make_pair(&ov,TG_QUAD); 
	Button* addquad = new Button(130, 535, 100, 20, "Quad");
	addquad->callback(GeometryViewer::addShapeCb, &shapeData2);

	shapes.end(); 

	int max = 320; 
	int may = 530; 

	Button* delGeom = new Button(243, may+5, 70, 20, "Del Ss");
	delGeom->callback(GeometryViewer::delEditingShapesCb,&ov); 

	Fl_Group mainActions(max,may,182,30,"Actions"); 
	mainActions.box(FL_BORDER_BOX); 
	mainActions.color(WIN_COLOR); 
	mainActions.begin(); 
	Button* undo = new Button(max+5, may+5, 80, 20, "Undo");
	undo->callback(GeometryViewer::undoCb, &ov);
	Button* apply = new Button(max+95, may+5, 80, 20, "Apply");
	apply->callback(IFSViewer::applyIFSCb, &viewers);
	mainActions.end();

	Button* objGrid = new Button(295, 670, 100, 20, "Grid Off");
	objGrid->callback(GeometryViewer::toggleGridCb,&ov); 

	Button* defObjView = new Button(400, 670, 100, 20, "Default View");
	defObjView->callback(GeometryViewer::defaultViewCb,&ov); 

	Button* addtrans = new Button(540, 520, 100, 20, "Transformation");
	addtrans->callback(IFSViewer::addOneTransformCb, &tv);

	Button* deltrans = new Button(650, 520, 55, 20, "Del Ts");
	deltrans->callback(IFSViewer::delEditingTransformsCb, &tv);

	Fl_Hor_Value_Slider* slider = new Fl_Hor_Value_Slider(242,570,260,20); 
	slider->box(FL_BORDER_BOX); 
	slider->color(WIN_COLOR); 
	slider->label("Grid subdivision"); 
	slider->bounds(0,5); 
	slider->step(1); 
	slider->precision(0); 
	slider->callback(BaseGrid::subdivValueCb,&tg); 
	slider->value(1); 
	tg.subdivide(1); 

	TransformBrowser browser(730,520, 165, 145); 
	tv.setTransformBrowser(&browser); 
	browser.callback(IFSViewer::IFSBrowserSelectCb,&tv); 

	Button* saveTrans = new Button(730, 670, 75, 20, "Save IFS"); 
	saveTrans->callback(IFSViewer::saveCurrentIFSCb,&tv); 
	Button* delTrans = new Button(820, 670, 75, 20, "Del IFS"); 
	delTrans->callback(IFSViewer::delCurrentIFSCb,&tv); 

	Button* saveTfile = new Button(920, 520, 100, 20, "Save File"); 
	saveTfile->callback(IFSViewer::saveIFSsToFile,&tv); 
	Button* openTfile = new Button(920, 545, 100, 20, "Open File"); 
	openTfile->callback(IFSViewer::openIFSsToFile,&tv); 

	Button* defTransView = new Button(920, 670, 100, 20, "Default View");
	defTransView->callback(IFSViewer::defaultViewCb,&tv); 

	Button* transBase = new Button(920, 645, 100, 20, "Base Off");
	transBase->callback(IFSViewer::toggleBaseMode,&tv); 

	Button* transSnap = new Button(920, 620, 100, 20, "Snap On");
	transSnap->callback(IFSViewer::toggleSnap,&tv); 


	pair<IFSViewer*,TransformGroup*> tbundle = make_pair(&tv,&tfgroup); 
	tfgroup.getTranslateBut()->callback(TransformGroup::applyTranslateCb,&tbundle); 
	tfgroup.getRotateBut()->callback(TransformGroup::applyRotateCb,&tbundle); 
	tfgroup.getScaleBut()->callback(TransformGroup::applyScaleCb,&tbundle); 
	tfgroup.getNUScaleBut()->callback(TransformGroup::applyNUScaleCb,&tbundle); 

	m.end(); 
	m.show(); 

	return Fl::run(); 
}