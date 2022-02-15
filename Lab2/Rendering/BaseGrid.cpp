#include "Rendering/BaseGrid.h" 
#include <cmath>
#include <map>
#include <list> 
#include <set> 

using namespace std; 

BaseGrid::BaseGrid(){
	_level = 1; 
	_base = NULL; 
}

void BaseGrid::setBase(Tri2* tri){
	_level = 1; 
	_base = tri; 
	subdivide(_level); 
}

void BaseGrid::subdivide(int times){
	if(!_base) return; 

	_level = times; 
	_pts.clear(); 
	_tris.clear(); 
	_edges.clear(); 

	_pts.push_back(*_base->get(0)); 
	_pts.push_back(*_base->get(1)); 
	_pts.push_back(*_base->get(2)); 

	_tris.push_back(make_triple(0,1,2)); 
	_edges.push_back(make_pair(0,1)); 
	_edges.push_back(make_pair(1,2)); 
	_edges.push_back(make_pair(2,0)); 

	for(int tt=0;tt<times;tt++){
		map<pair<int,int>,int> e2p; 
		list<triple<int> > ntris; 
		for(list<triple<int> >::iterator i=_tris.begin();i!=_tris.end();i++){
			pair<int,int> es[3]={make_pair(i->a,i->b),make_pair(i->b,i->c),make_pair(i->c,i->a)}; 
			int ntri[3]; 
			for(int j=0;j<3;j++){
				if(e2p.find(es[j])==e2p.end()){
					Pt2 np = .5*(_pts[es[j].first]+_pts[es[j].second]); 
					e2p[es[j]] = _pts.size(); 
					ntri[j] = _pts.size(); 
					_pts.push_back(np); 
				}
				else
					ntri[j] = e2p[es[j]]; 
			}
			triple<int> ntrip; 
			// middle triangle
			ntrip.a = ntri[0]; 
			ntrip.b = ntri[1]; 
			ntrip.c = ntri[2]; 
			ntris.push_back(ntrip); 

			// top triangle
			ntrip.a = i->a; 
			ntrip.b = ntri[0]; 
			ntrip.c = ntri[2]; 
			ntris.push_back(ntrip);

			// left triangle
			ntrip.a = ntri[0]; 
			ntrip.b = i->b; 
			ntrip.c = ntri[1]; 
			ntris.push_back(ntrip); 

			// left triangle
			ntrip.a = ntri[2]; 
			ntrip.b = ntri[1]; 
			ntrip.c = i->c; 
			ntris.push_back(ntrip); 

			_edges.push_back(make_pair(ntri[0],ntri[1])); 
			_edges.push_back(make_pair(ntri[1],ntri[2])); 
			_edges.push_back(make_pair(ntri[2],ntri[0])); 
		}
		_tris = ntris; 
	}
}

pair<double,int> BaseGrid::findClosest(const Pt2& p) const{
	double bestd = Utils::dist2d(_pts[0],p); 
	int best = 0; 
	for(unsigned int j=1;j<_pts.size();j++){
		double nd = Utils::dist2d(_pts[j],p); 
		if(nd<bestd){
			bestd = nd; 
			best = j; 
		}
	}

	return make_pair(bestd,best); 
}

void BaseGrid::subdivValueCb(Fl_Widget* widget, void* userdata){
	BaseGrid* grid = (BaseGrid*) userdata; 
	Fl_Hor_Value_Slider* slider = (Fl_Hor_Value_Slider*) widget; 
	if(grid){
		int level = slider->value(); 
		grid->subdivide(level); 
	}
}