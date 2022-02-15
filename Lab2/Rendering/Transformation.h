#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "Common/TinyGeom.h" 

using namespace TinyGeom; 

class Transformation : public Geom2Visitor{
protected: 
	Mat3 _mat; 

	void applyPrime(Geom2* g, Geom2* out){
		for(int j=0;j<g->size();j++)
			(*out->get(j)) = apply(*g->get(j)); 
	}
public: 

	Mat3* getmat() { return &_mat; }

	// TODO: complete the following functions in Transformation.cpp
	void setAsIdentity(); 
	void setAsTranslate(const Vec2& v); 
	void setAsRotate(double r, const Pt2& p); 
	void setAsScale(double s, const Pt2& p); 
	void setAsNUScale(const Vec2& s, const Pt2& p); 
	void setAs3PtTransform(const Tri2& src, const Tri2& dest); 

	void composeTranslate(const Vec2& v); 
	void composeRotate(double r, const Pt2& p); 
	void composeScale(double s, const Pt2& p); 
	void composeNUScale(const Vec2& s, const Pt2& p); 
	void compose3PtTransform(const Tri2& src, const Tri2& dest); 

	Pt2 apply(const Pt2& p){ return p*_mat; }

	// TODO: need to add more visits for more shapes
	void visit(Tri2* geom, void* data){
		Tri2* ret = new Tri2(); 
		applyPrime(geom,ret); 
		*((Geom2**) data) = ret; 
	}

	void visit(Quad2* geom, void* data){
		Quad2* ret = new Quad2(); 
		applyPrime(geom,ret); 
		*((Geom2**) data) = ret; 
	}

}; 

#endif