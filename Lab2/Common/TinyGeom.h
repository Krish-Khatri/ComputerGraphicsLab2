#ifndef TINY_GEOM_H
#define TINY_GEOM_H

#include "Common/Common.h" 
#include "Common/Matrix.h" 

namespace TinyGeom {
	const int CIRCLE_SIZE = 100;
	typedef Vector3D Pt2; // use 3d vector for affine transform 
	typedef Pt2 Vec2;
	typedef Vector4D Color;

	typedef Matrix<double, 3> Mat3;
	class Geom2Visitor;

	class Geom2 {
	protected:
		Pt2* _pts;
	public:
		Geom2() { _pts = NULL; }
		~Geom2() { delete[] _pts; }
		virtual int size() const = 0;
		virtual Pt2* get(int i) { return &_pts[i]; }
		const Pt2* get(int i) const { return &_pts[i]; }
		virtual void init() { _pts = new Pt2[this->size()]; }
		virtual void operator=(const Geom2& g) {
			assert(size() == g.size());
			for (int j = 0; j < g.size(); j++)
				_pts[j] = g._pts[j];
		}
		virtual void accept(Geom2Visitor* visitor, void* data) = 0;
	};

	class Tri2;
	class Quad2;
	class Hex2;
	class Oct2;
	class Circ2;
	class Pent2;
	// TODO: need to declare additional shapes here

	class Geom2Visitor {
	public:
		virtual void visit(Tri2* geom, void* data) = 0;
		virtual void visit(Quad2* geom, void* data) = 0;
		virtual void visit(Hex2* geom, void* data) = 0;
		virtual void visit(Oct2* geom, void* data) = 0;
		virtual void visit(Pent2* geom, void* data) = 0;
		virtual void visit(Circ2* geom, void* data) = 0;
		// TODO: add visit functions for additional shapes
	};

	// util functions
	class Utils {
	public:
		static bool isPtInterior(Geom2* g, const Pt2& p);
		static bool isConvex(Geom2* g);
		static double cross2d(const Vec2& v, const Vec2& w);
		static double dist2d(const Pt2& a, const Pt2& b);
		static Pt2 centroid(Geom2* g);
	};

	// TODO: need to TGShape enum needs to be expanded for addtional shapes
	enum TGShape { TG_TRIANGLE, TG_QUAD, TG_HEX, TG_OCT, TG_CIRC, TG_PENT };

	class Tri2 : public Geom2 {
	public:
		Tri2();
		Tri2(const Pt2& a, const Pt2& b, const Pt2& c);
		Tri2(const Pt2& c, double r);

		virtual int size() const { return 3; }

		virtual void accept(Geom2Visitor* visitor, void* data) {
			visitor->visit(this, data);
		}
	};

	class Quad2 : public Geom2 {
	public:
		Quad2();
		Quad2(const Pt2& c, double r);

		virtual int size() const { return 4; }

		virtual void accept(Geom2Visitor* visitor, void* data) {
			visitor->visit(this, data);
		}
	};

	class Hex2 : public Geom2 {
	public:
		Hex2();
		Hex2(const Pt2& c, double r);

		virtual int size() const { return 6; }

		virtual void accept(Geom2Visitor* visitor, void* data) {
			visitor->visit(this, data);
		}
	};

	class Oct2 : public Geom2 {
	public:
		Oct2();
		Oct2(const Pt2& c, double r);

		virtual int size() const { return 8; }

		virtual void accept(Geom2Visitor* visitor, void* data) {
			visitor->visit(this, data);
		}
	};

	class Pent2 : public Geom2 {
	public:
		Pent2();
		Pent2(const Pt2& c, double r);

		virtual int size() const { return 5; }

		virtual void accept(Geom2Visitor* visitor, void* data) {
			visitor->visit(this, data);
		}
	};

	class Circ2 : public Geom2 {
	public:
		Circ2();
		Circ2(const Pt2& c, double r);

		virtual int size() const { return CIRCLE_SIZE; }

		virtual void accept(Geom2Visitor* visitor, void* data) {
			visitor->visit(this, data);
		}
	};
	// TODO: subclass from Geom2 for additional shapes, follow the Triangle 
	//       and Quad example.  For the circle, you might need to include 
	//       addtional fields such as center and radius.  

}

#endif