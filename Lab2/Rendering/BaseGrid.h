#ifndef BASE_GRID_H
#define BASE_GRID_H

#include <Fl/Fl_Hor_Value_Slider.H> 
#include "Common/TinyGeom.h" 
#include <list> 
#include <vector> 

using namespace TinyGeom; 

template<typename T>
class triple{
public:
	T a,b,c; 
}; 

template<typename T> 
triple<T> make_triple(const T& a, const T& b, const T& c){
	triple<T> ret; 
	ret.a = a; 
	ret.b = b; 
	ret.c = c; 
	return ret; 
}

class BaseGrid{
protected: 
	Tri2* _base; 
	std::list<triple<int> > _tris; 
	std::list<pair<int,int> > _edges; 
	std::vector<Pt2> _pts; 
	int _level ; 

public: 
	BaseGrid(); 
	void setBase(Tri2* tri); 
	void subdivide(int times); 
	int level() { return _level; }
	const std::vector<Pt2>& getPts() { return _pts;}
	const std::list<pair<int,int> >& getEdges() { return _edges; }
	pair<double,int> findClosest(const Pt2& p) const; 
	static void subdivValueCb(Fl_Widget* widget, void* userdata); 
}; 

#endif