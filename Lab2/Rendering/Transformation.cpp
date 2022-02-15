#include <cmath> 
#include "Rendering/Transformation.h" 

void Transformation::setAsIdentity(){
	_mat.identity(); 
}

void Transformation::setAsTranslate(const Vec2& v){
	//TODO: fill this in
}


void Transformation::setAsRotate(double r, const Pt2& q){
	//TODO: fill this in
}

void Transformation::setAsScale(double s, const Pt2& p){
	//TODO: fill this in
}


void Transformation::setAsNUScale(const Vec2& s, const Pt2& q){
	//TODO: fill this in
}

void Transformation::composeTranslate(const Vec2& v){
	//TODO: fill this in
}

void Transformation::composeRotate(double r, const Pt2& q){
	//TODO: fill this in
}

void Transformation::composeScale(double s, const Pt2& q){
	//TODO: fill this in
}

void Transformation::composeNUScale(const Vec2& sw, const Pt2& q){
	//TODO: fill this in
}

void Transformation::compose3PtTransform(const Tri2& src, const Tri2& dest){
	//TODO: fill this in
}


void Transformation::setAs3PtTransform(const Tri2& src, const Tri2& dest){
	//TODO: fill this in
}
