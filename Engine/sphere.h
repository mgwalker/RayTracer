//*****************************************************************************
//    file: surface.h
// created: 2-5-2005
//  author: Greg Walker
// purpose: Sphere surface
//*****************************************************************************
/*
#ifndef SPHERE_H
#define SPHERE_H

#include <iostream>
#include "surface.h"
#include "ray.h"
#include "hit.h"
#include "light.h"
#include "material.h"

class sphere_t : public surface_t
{
	public:
		sphere_t()
		{
			this->_radius = 0;
		}
		// What is this particular surface?
		char* name() { return "Sphere"; }

		// Tags that delimit surface block
		static char* begin_tag(){ return "begin_sphere"; };
		static char* end_tag(){ return "end_sphere"; };

		// ensures: surface description read from in
		void read(std::istream& ins)
		{
			std::string	cmd;
			bool		seen_end		= false;
			//bool		seen_mat_end	= false;

			while(!ins.eof() && !seen_end)
			{
				ins >> cmd;
				if(cmd == sphere_t::end_tag())
					seen_end = true;
				else if(cmd == "position")
					ins >> this->_position;
				else if(cmd == "radius")
					ins >> this->_radius;
				else if(cmd == "#")
					ins.ignore(std::numeric_limits<int>::max(), '\n');
				else if(cmd == "begin_material")
					this->_material.read(ins);
			}
		}

		// requires: length( ray.dir ) != 0, t0 > 0, t1 > 0, t1 > t0
		//  ensures: IFF ray strikes this surface, THEN 
		//           this->intersect( ray, t0, t1, hit ) == true, AND
		//           hit contains the relevant information
		bool intersect(ray_t& ray, double t0, double t1, hit_t& hit)
		{
			gmMatrix4 T;
			if(this->_material._txformOn)
			{
				T = (ray.getTime() * this->_material._txform) + ((1 - ray.getTime()) * gmMat4::identity());
			}

			// Initial variables...
			// Vector from ray origin to sphere center
			gmVector3 oc = this->_position - ray.getOrigin();
			// Squared length version of previous
			double oc2 = oc.lengthSquared();
			// Closest point on the ray to sphere center
			double tca = dot(oc, ray.getDir());
			// Square of sphere radius
			double sr2 = this->_radius * this->_radius;
			// Inside or outside?
			bool outside = !(oc2 < sr2);

			if(tca < 0 && outside)	// ray points away from the sphere
				return false;

			// Half-chord distance
			double thc2 = sr2 - oc2 + (tca * tca);

			if(thc2 < 0 && outside)	// ray points toward the sphere, but misses
				return false;

			double ti;	// distance from ray origin to sphere surface
			if(outside)
				ti = tca - sqrt(thc2);
			else
				ti = tca + sqrt(thc2);

			if(ti >= t0 && ti <= t1)
			{
				// Store and return
				hit.t = ti;
				hit.r = ray.getOrigin() + (ti * ray.getDir());
				hit.n = hit.r - this->_position;
				hit.n.normalize();
				return true;
			}
			return false;
		}

		gmVector3 getColor()   { return this->_material._color; }
		gmVector3 getAmbient() { return this->_material._ambient; }
		gmVector3 getReflect() { return this->_material._refl; }
		double    getPhong()   { return this->_material._phong; }
		gmVector3 getRefractE(){ return this->_material._refrExtinct; }
		double    getRefractI(){ return this->_material._refrIndex; }

		bool doReflect() { return this->_material._reflectOn; }
		bool doRefract() { return this->_material._refractOn; }
		bool doSpecular(){ return this->_material._specularOn; }
		bool doGloss()   { return this->_material._glossOn; }
		bool doBlur()    { return this->_material._blurOn; }

		void print(std::ostream &os)
		{
			os << "\t\tPosition: " << this->_position << std::endl;
			os << "\t\tRadius:   " << this->_radius << std::endl;
			this->_material.print(os);
		}

	private:
		gmVector3	_position;
		double		_radius;
		Material	_material;
};

#endif
*/