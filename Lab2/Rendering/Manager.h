#ifndef MANAGER_H
#define MANAGER_H

// keeps records for both the GeometryViewer and the IFSViewer

#include "Common/TinyGeom.h" 
#include "Rendering/Transformation.h" 
#include <list> 
#include <map> 
#include <set> 
#include <string> 

using namespace std; 

class GeometryHistory{
protected: 
	list<list<pair<Geom2*,Color> >*> _stack; 
public: 
	list<pair<Geom2*,Color> >* pushNew(){
		list<pair<Geom2*,Color> >* lg = new list<pair<Geom2*,Color> >(); 
		_stack.push_back(lg); 
		return lg; 
	}

	list<pair<Geom2*,Color> >* getTop(){
		if(_stack.size()>0)
			return _stack.back(); 
		return NULL; 
	}

	void popTop(){
		list<pair<Geom2*,Color> >* lg = _stack.back(); 
		for(list<pair<Geom2*,Color> >::iterator i=lg->begin();i!=lg->end();i++)
			delete i->first; 
		delete lg; 
		_stack.pop_back(); 
	}

	int size() { return (int) _stack.size();  }
}; 

class TransformEntry{
protected: 
	Tri2 _base; 
	list<Tri2*> _geoms; 
	map<Pt2*,Tri2*> _p2geom; 
	map<Tri2*,Transformation*> _tri2trans; 
	Pt2 _transCenter; // center of transformation in IFS Viewer
public: 
	TransformEntry(){
		_transCenter = Pt2(0,0); 
		_p2geom[&_transCenter] = NULL;  // so that we can move the transformation center
	}

	~TransformEntry(){
		clear(); 
	}

	void clear(){
		for(list<Tri2*>::iterator i=_geoms.begin();i!=_geoms.end();i++){
			delete *i; 
		}
		for(map<Tri2*,Transformation*>::iterator i=_tri2trans.begin();i!=_tri2trans.end();i++){
			delete i->second; 
		}	
		_geoms.clear(); 
		_p2geom.clear(); 
		_tri2trans.clear(); 

		_p2geom[&_transCenter] = NULL;
	}

	list<Tri2*>* getGeoms() { return &_geoms; }
	map<Pt2*,Tri2*>* getP2Geom() { return &_p2geom; }
	map<Tri2*,Transformation*>* getTri2Trans() { return &_tri2trans; }
	Tri2* getBase() { return &_base; }
	Pt2* getTransCenter() { return &_transCenter; }

	void remove(Tri2* t){
		list<Tri2*> ngeoms; 
		map<Pt2*,Tri2*> np2geom; 
		map<Tri2*,Transformation*> ntri2trans; 

		for(list<Tri2*>::iterator i=_geoms.begin();i!=_geoms.end();i++){
			if(*i!=t)
				ngeoms.push_back(*i); 
		}

		for(map<Pt2*,Tri2*>::iterator i=_p2geom.begin();i!=_p2geom.end();i++){
			if(i->second!=t)
				np2geom[i->first] = i->second; 
		}

		for(map<Tri2*,Transformation*>::iterator i=_tri2trans.begin();i!=_tri2trans.end();i++){
			if(i->first!=t)
				ntri2trans[i->first] = i->second; 
			else
				delete i->second; 
		}

		bool del = _geoms.size()!=ngeoms.size(); 

		_geoms = ngeoms; 
		_p2geom = np2geom; 
		_tri2trans = ntri2trans; 

		if(del) delete t; 
	}

	void add(Tri2* nt){
		_geoms.push_back(nt); 
		for(int j=0;j<nt->size();j++)
			_p2geom[nt->get(j)] = nt; 

		Transformation* trans = new Transformation(); 
		trans->setAs3PtTransform(_base,*nt); 
		_tri2trans[nt] = trans; 
	}

	void set(TransformEntry* ent){
		clear(); 

		_base = ent->_base; 
		map<Tri2*,Tri2*> old2new; 
		for(list<Tri2*>::iterator i=ent->_geoms.begin();i!=ent->_geoms.end();i++){
			Tri2* t = new Tri2(); 
			(*t) = *(*i); 
			_geoms.push_back(t); 
			old2new[*i] = t; 
		}

		for(list<Tri2*>::iterator i=_geoms.begin();i!=_geoms.end();i++){
			for(int j=0;j<(*i)->size();j++)
				_p2geom[(*i)->get(j)] = *i; 
		}

		for(map<Tri2*,Transformation*>::iterator i=ent->_tri2trans.begin();i!=ent->_tri2trans.end();i++){
			Transformation* trans = new Transformation(); 
			(*trans) = *i->second; 
			_tri2trans[old2new[i->first]] = trans; 
		}
	}
}; 

class TransformManager{
protected: 
	map<string,TransformEntry*> _entries; 
public: 

	~TransformManager(){
		list<string> names = getEntryNames(); 
		for(list<string>::iterator i=names.begin();i!=names.end();i++)
			removeEntry(*i); 
	}

	TransformEntry* newEntry(const string& s){
		if(_entries.find(s)==_entries.end()){
			TransformEntry* te = new TransformEntry(); 
			_entries[s] = te; 
			return te; 
		}
		return NULL; 
	}

	TransformEntry* getEntry(const string& s){
		if(_entries.find(s)!=_entries.end())
			return _entries[s]; 
		return NULL; 
	}

	void removeEntry(const string& s){
		if(_entries.find(s)!=_entries.end()){
			delete _entries[s]; 
			_entries.erase(s); 
		}
	}

	void removeAllEntry(){
		list<string> names = getEntryNames(); 
		for(list<string>::iterator i=names.begin();i!=names.end();i++)
			removeEntry(*i); 
	}

	string getName(TransformEntry* ent){
		for(map<string,TransformEntry*>::iterator i=_entries.begin();i!=_entries.end();i++)
			if(i->second==ent)
				return i->first; 
		return ""; 
	}

	list<string> getEntryNames(){
		list<string> ret; 
		for(map<string,TransformEntry*>::iterator i=_entries.begin();i!=_entries.end();i++)
			ret.push_back(i->first); 
		return ret; 
	}
}; 

/**/
#endif