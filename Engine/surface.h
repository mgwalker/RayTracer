//*****************************************************************************
//    file: surface.h
// created: 1-30-2005
//  author: J. Edward Swan II
// purpose: Surfaces are geometric entities which know how to intersect 
//          themselves with rays.  This is the most basic geometric object 
//          class.
//*****************************************************************************

#ifndef SURFACE_H
#define SURFACE_H

#include <iostream>
#include "ray.h"
#include "hit.h"
#include "material.h"
#include "../libgm/gmMat4.h"

class surface_t 
{
	public:
		virtual ~surface_t(){}

		// What is this particular surface?
		virtual char* name() { return "surface"; }

		// Tags that delimit surface block
		//virtual char* begin_tag();
		//virtual char* end_tag();

		virtual void read(std::istream& in){}
		// ensures: surface description read from in

		virtual bool intersect(ray_t& ray, double t0, double t1, hit_t& hit){ return false; }
		// requires: length( ray.dir ) != 0, t0 > 0, t1 > 0, t1 > t0
		//  ensures: IFF ray strikes this surface, THEN 
		//           this->intersect( ray, t0, t1, hit ) == true, AND
		//           hit contains the relevant information

		virtual void print(std::ostream &os){}

		virtual gmVector3 getColor()            { return this->_material._color; }
		virtual gmVector3 getAmbient()          { return this->_material._ambient; }
		virtual gmVector3 getReflect()          { return this->_material._refl; }
		virtual double    getPhong()            { return this->_material._phong; }
		virtual gmVector3 getRefractE()         { return this->_material._refrExtinct; }
		virtual double    getRefractI()         { return this->_material._refrIndex; }
		virtual double    getGloss()            { return this->_material._gloss; }
		virtual double    getBlur()             { return this->_material._blur; }
		virtual void      doTTransform(gmMatrix4 T){}

		virtual bool doReflect() { return this->_material._reflectOn; }
		virtual bool doRefract() { return this->_material._refractOn; }
		virtual bool doSpecular(){ return this->_material._specularOn; }
		virtual bool doGloss()   { return this->_material._glossOn; }
		virtual bool doBlur()    { return this->_material._blurOn; }

		friend std::ostream& operator<<(std::ostream &os, surface_t &s);

	private:
		Material	_material;
};

std::ostream& operator<<(std::ostream &os, surface_t &s)
{
    s.print(os);
    return os;
}

#endif
