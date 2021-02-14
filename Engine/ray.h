//*****************************************************************************
//    file: ray.h
// created: 1-30-2005
//  author: J. Edward Swan II
// purpose: Define a ray
//*****************************************************************************

#ifndef RAY_H
#define RAY_H

#include <iostream>
#include "../libgm/gmVec3.h"

class ray_t 
{
	private:
		gmVector3    origin;		// Where ray starts
		gmVector3    dir;			// Direction ray points
		gmVector3    dir_norm;		// Normalized ray direction
		unsigned int generation;	// How many times this ray has bounced
		double       time;

	public:
		ray_t(){ this->generation = 0; }
		void set_origin(gmVector3 _origin);

		void set_dir(gmVector3 direction);
		// requires: length( direction ) > 0
		//  ensures: this->dir == direction, this->dir_norm == direction / length( direction )

		void set_time(double timestep);

		gmVector3    getOrigin(){ return this->origin; }
		gmVector3    getDir()   { return this->dir_norm; }
		double       getTime()  { return this->time; }
		bool         bounce()   { this->generation++; return(this->generation < 15); }
		unsigned int gen()      { return this->generation; }
		void setGen(unsigned int gen){ this->generation = gen; }

		friend std::ostream& operator << ( std::ostream& os, const ray_t& );
		//  ensures: A representation of the ray is printed to os.
};

inline void ray_t::set_origin(gmVector3 _origin)
{
	this->origin = _origin;
}

inline void ray_t::set_dir( gmVector3 direction )
{
	assert( direction.length() > gmEPSILON );
	this->dir = direction;
	this->dir_norm = direction;
	this->dir_norm.normalize();
}

inline void ray_t::set_time(double timestep)
{
	this->time = timestep;
}

inline std::ostream& operator << ( std::ostream& os, const ray_t& r )
{
	os << "\tOrigin:  " << r.origin << "\n";
	os << "\tDir:     " << r.dir << "\n";
	os << "\tDirNorm: " << r.dir_norm << "\n";
	return os;
}

#endif
