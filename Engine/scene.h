//*****************************************************************************
//    file: view.h
// created: 2-6-2005
//  author: Greg Walker
// purpose: Define a scene class
//*****************************************************************************

#ifndef SCENE_H
#define SCENE_H

#include <iostream>
//#include "sphere.h"
#include "material.h"
#include "polygon.h"
#include "light.h"

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
			gmVector3 position;
			double radius;
			if(this->_material._txformOn)
			{
				T = (ray.getTime() * this->_material._txform) + ((1 - ray.getTime()) * gmMatrix4::identity());

				gmVector4 x;
				x[0] = this->_position[0];
				x[1] = this->_position[1];
				x[2] = this->_position[2];
				x[3] = 1.0;
				if((T[0][0] == T[1][1]) && (T[1][1] == T[2][2]))
					radius = this->_radius * T[0][0];
				T[0][0] = T[1][1] = T[2][2] = 1;
				x = T * x;
				position[0] = x[0];
				position[1] = x[1];
				position[2] = x[2];
			}
			else
			{
				position = this->_position;
				radius = this->_radius;
			}

			// Initial variables...
			// Vector from ray origin to sphere center
			gmVector3 oc = position - ray.getOrigin();
			// Squared length version of previous
			double oc2 = oc.lengthSquared();
			// Closest point on the ray to sphere center
			double tca = dot(oc, ray.getDir());
			// Square of sphere radius
			double sr2 = radius * radius;
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
				hit.n = hit.r - position;
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
		double    getGloss()   { return this->_material._gloss; }
		double    getBlur()    { return this->_material._blur; }

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

class scene_t
{
	public:
		scene_t()		// Constructor
		{
			_bgColor.assign(0.0,0.0,0.0);	// Initial black background
			this->_numSurfaces = 0;
			this->_pIndex = 0;
			this->_lIndex = 0;
			this->_sRate = 1;
			this->_sRateSqr = 1;
		}

		~scene_t(){}	// Destructor

		// requires: ins contains the scene commands in any order.  ins may contain
		//           comment lines beginning with "#".
		//  ensures: The commands are read from ins and the scene fields are set to the
		//           appropriate values.  If ins contains multiple copies of the same 
		//           command, the values given by the last command instance take effect.
		void read(std::istream& ins)
		{
			std::string cmd;
			bool seen_end_tag = false;
			sphere_t* s;
			polygon_t** p;
			unsigned int polys;
			Light* l;

			// Loop until the end of the file or the end tag is reached
			while(!ins.eof() && !seen_end_tag)
			{
				ins >> cmd;

				// Skip comments
				if(cmd == "#")
					ins.ignore(std::numeric_limits<int>::max(), '\n');

				// Check for the end tag
				else if(cmd == this->end_tag())
					seen_end_tag = true;

				else if(cmd == "background")
					ins >> this->_bgColor;

				else if(cmd == "sample_rate")
				{
					ins >> this->_sRate;
					this->_sRateSqr = this->_sRate * this->_sRate;
				}

				// Read in a sphere surface
				else if(cmd == sphere_t::begin_tag())
				{
					s = new sphere_t;
					s->read(ins);
					this->addSurface(s);
				}

				else if(cmd == poly::begin_tag())
				{
					p = poly::read(ins, polys);
					for(unsigned int i = 0; i < polys; i++)
						this->addSurface(p[i]);
				}

				// Read in a light
				else if(cmd == Light::begin_tag())
				{
					l = new Light;
					l->read(ins);
					this->addLight(l);
				}
			}
		}

		// Add a surface to the array
		void addSurface(surface_t *s)
		{
			this->_numSurfaces++;

			surface_t** tmp = this->_surfaces;
			this->_surfaces = new surface_t*[this->_numSurfaces];
			memcpy(this->_surfaces, tmp, sizeof(surface_t*) * (this->_numSurfaces - 1));
			this->_surfaces[this->_numSurfaces - 1] = s;
		}

		// Return the next surface in the array
		surface_t* getNextSurface()
		{
			// Make sure the internal index is valid
			if(this->_pIndex < this->_numSurfaces)
			{
				this->_pIndex++;
				return this->_surfaces[this->_pIndex - 1];
			}

			// Else reset to 0 and return null.  Further calls
			// to this function will return surfaces again,
			// starting at index 0
			this->_pIndex = 0;
			return NULL;
		}

		unsigned int numSurfaces(){ return this->_numSurfaces; }
		surface_t* fetchSurface(unsigned int in)
		{
			if(in < this->_numSurfaces)
				return this->_surfaces[in];
			return NULL;
		}

		Light* fetchLight(unsigned int _li){ return this->_lights[_li]; }
		unsigned int numLights() { return this->_numLights; }

		// Add a light to the array
		void addLight(Light *l)
		{
			this->_numLights++;

			Light** tmp = this->_lights;
			this->_lights = new Light*[this->_numLights];
			memcpy(this->_lights, tmp, sizeof(Light*) * (this->_numLights - 1));
			this->_lights[this->_numLights - 1] = l;
		}

		// Return the next light in the array
		Light* getNextLight()
		{
			// Make sure the internal index is valid
			if(this->_lIndex < this->_numLights)
			{
				this->_lIndex++;
				return this->_lights[this->_lIndex - 1];
			}

			// Else reset to 0 and return null.  Further calls
			// to this function will return lights again,
			// starting at index 0
			this->_lIndex = 0;
			return NULL;
		}

		bool lightingEnabled()
		{
			if(this->_numLights > 0)
				return true;
			return false;
		}

		gmVector3 getBGColor()    { return this->_bgColor; }
		unsigned int getSRate()   { return this->_sRate; }
		unsigned int getSRateSqr(){ return this->_sRateSqr; }

		//  ensures: A representation of the scene is printed to os.
		friend std::ostream& operator << (std::ostream& os, const scene_t&);

		// Tags that delimit a scene block
		char* begin_tag() { return "begin_scene"; }
		char* end_tag()   { return "end_scene"; }

	private:
		surface_t**		_surfaces;		// Array of scene surfaces
		unsigned int	_numSurfaces;	// Number of scene surfaces
		unsigned int	_pIndex;		// Index into surface array
		gmVector3		_bgColor;		// Scene background color
		Light**			_lights;		// Scene lights
		unsigned int	_numLights;		// Number of lights
		unsigned int	_lIndex;		// Index into light array
		unsigned int	_sRate;			// Samples across/up
		unsigned int	_sRateSqr;		// Samples per pixel
};

std::ostream& operator << (std::ostream& os, const scene_t& s)
{
	os << "SURFACES: " << s._numSurfaces << "\n";
	for(unsigned int i = 0; i < s._numSurfaces; i++)
	{
		os << "\t" << i << ": " << (s._surfaces[i])->name() << std::endl;
		os << *(s._surfaces[i]);
	}

	os << "LIGHTS: " << s._numLights << "\n";
	for(unsigned int i = 0; i < s._numLights; i++)
	{
		os << "\tLight " << i << std::endl;
		os << *(s._lights[i]);
	}

    return os;
}

#endif
