#include <cmath> 
#include "Rendering/Transformation.h" 

void Transformation::setAsIdentity() {
	_mat.identity();
}

void Transformation::setAsTranslate(const Vec2& v) {
	setAsIdentity();
	composeTranslate(v);
}


void Transformation::setAsRotate(double r, const Pt2& q) {
	setAsIdentity();
	composeRotate(r, q);
}

void Transformation::setAsScale(double s, const Pt2& p) {
	setAsIdentity();
	composeScale(s, p);
}


void Transformation::setAsNUScale(const Vec2& s, const Pt2& q) {
	setAsIdentity();
	composeNUScale(s, q);
}

void Transformation::composeTranslate(const Vec2& v) {
	Mat3 trans;
	trans.identity();
	trans[2][0] = v[0];
	trans[2][1] = v[1];
	_mat = trans * _mat;
}

void Transformation::composeRotate(double r, const Pt2& q) {
	double rad = r * M_PI / 180;
	Mat3 rot;
	rot.identity();
	rot[0][0] = cos(rad);
	rot[0][1] = sin(rad);
	rot[1][0] = -sin(rad);
	rot[1][1] = cos(rad);
	rot[2][0] = q[0] * (1 - cos(rad)) + q[1] * sin(rad);
	rot[2][1] = -q[0] * sin(rad) + q[1] * (1 - cos(rad));
	_mat = rot * _mat;
}

void Transformation::composeScale(double s, const Pt2& q) {
	Mat3 scale;
	scale.identity();
	scale[0][0] = s;
	scale[1][1] = s;
	scale[2][0] = (1 - s) * q[0];
	scale[2][1] = (1 - s) * q[1];
	_mat = scale * _mat;
}

void Transformation::composeNUScale(const Vec2& sw, const Pt2& q) {
	Mat3 nuScale, M1, M2;
	double s = sqrt(sw[0] * sw[0] + sw[1] * sw[1]);
	if (sw[0] < 0 || sw[1] < 0) s *= -1;
	Vec2 w(sw[0] / s, sw[1] / s);
	M1[0][0] = w[0]; M1[0][1] = w[1]; M1[0][2] = 0;
	M1[1][0] = -w[1]; M1[1][1] = w[0]; M1[1][2] = 0;
	M1[2][0] = q[0]; M1[2][1] = q[1]; M1[2][2] = 1;

	M2[0][0] = sw[0]; M2[0][1] = sw[1]; M2[0][2] = 0;
	M2[1][0] = -w[1]; M2[1][1] = w[0]; M2[1][2] = 0;
	M2[2][0] = q[0]; M2[2][1] = q[1]; M2[2][2] = 1;
	nuScale = (!M1) * M2;
	_mat = nuScale * _mat;
}

void Transformation::compose3PtTransform(const Tri2& src, const Tri2& dest) {
	Mat3 trans, M1, M2;
	M1[0][0] = (*src.get(0))[0]; M1[0][1] = (*src.get(0))[1]; M1[0][2] = 1;
	M1[1][0] = (*src.get(1))[0]; M1[1][1] = (*src.get(1))[1]; M1[1][2] = 1;
	M1[2][0] = (*src.get(2))[0]; M1[2][1] = (*src.get(2))[1]; M1[2][2] = 1;

	M2[0][0] = (*dest.get(0))[0]; M2[0][1] = (*dest.get(0))[1]; M2[0][2] = 1;
	M2[1][0] = (*dest.get(1))[0]; M2[1][1] = (*dest.get(1))[1]; M2[1][2] = 1;
	M2[2][0] = (*dest.get(2))[0]; M2[2][1] = (*dest.get(2))[1]; M2[2][2] = 1;
	trans = (!M1) * M2;
	_mat = trans * _mat;
}


void Transformation::setAs3PtTransform(const Tri2& src, const Tri2& dest) {
	setAsIdentity();
	compose3PtTransform(src, dest);
}
