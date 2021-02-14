#ifndef __POLYGON_H
#define __POLYGON_H

#include <iostream>
#include <string.h>
#include "../libgm/gmVec3.h"
#include "../libgm/gmVec4.h"
#include "../libgm/gmMat4.h"
#include "material.h"

class polygon_t : public surface_t
{
	public:
		polygon_t()
		{
			this->_vertices = 0;
		}
		~polygon_t(){}

		// What is this particular surface?
		char* name() { return "Polygon"; }

		// ensures: surface description read from in
		void read(std::istream& in){}

		// requires: length( ray.dir ) != 0, t0 > 0, t1 > 0, t1 > t0
		//  ensures: IFF ray strikes this surface, THEN 
		//           this->intersect( ray, t0, t1, hit ) == true, AND
		//           hit contains the relevant information
		// POLYGON BETTER BE PLANAR, ELSE THIS THING'S GONNA 'SPLODE
		bool intersect(ray_t& ray, double t0, double t1, hit_t& hit)
		{
			gmVector3 v0,v1,v2;
			bool ret = false;
			unsigned int j,k;
			gmVector4* vertexList = new gmVector4[this->_vertices];
			gmMatrix4 T;
			if(this->_material._txformOn)
			{
				T = (ray.getTime() * this->_material._txform) + ((1 - ray.getTime()) * gmMatrix4::identity());
				for(unsigned int i = 0; i < this->_vertices; i++)
					vertexList[i] = T * this->_vertexL[i];
			}
			else
				for(unsigned int i = 0; i < this->_vertices; i++)
					vertexList[i] = this->_vertexL[i];

			for(unsigned int i = 0; i < this->_vertices; i++)
			{
				j = i + 1;
				if(j == this->_vertices)
					j = 0;
				k = j + 1;
				if(k == this->_vertices)
					k = 0;

				/** /
				v0[0] = this->_vertexL[i][0];
				v0[1] = this->_vertexL[i][1];
				v0[2] = this->_vertexL[i][2];

				v1[0] = this->_vertexL[j][0];
				v1[1] = this->_vertexL[j][1];
				v1[2] = this->_vertexL[j][2];

				v2[0] = this->_vertexL[k][0];
				v2[1] = this->_vertexL[k][1];
				v2[2] = this->_vertexL[k][2];

				if(this->_material._txformOn)
				{

				}
				//*/

				/**/
				v0[0] = vertexList[i][0];
				v0[1] = vertexList[i][1];
				v0[2] = vertexList[i][2];

				v1[0] = vertexList[j][0];
				v1[1] = vertexList[j][1];
				v1[2] = vertexList[j][2];

				v2[0] = vertexList[k][0];
				v2[1] = vertexList[k][1];
				v2[2] = vertexList[k][2];
				//*/

				gmVector3 u = (v1 - v0);
				gmVector3 v = (v2 - v0);
				gmVector3 norm = cross(u, v);
				norm.normalize();

				double d,t;

				//a = norm[0]; b = norm[1]; c = norm[2];
				d = dot(-v0, norm);
				double denom = dot(norm, ray.getDir());

				if(gmFuzEQ(denom,0))	// parallel
				{
					delete vertexList;
					return false;
				}

				t = (-(dot(norm, ray.getOrigin()) + d)) / denom;
				if(gmFuzLEQ(t,0))		// ray moves away
				{
					delete vertexList;
					return false;
				}

				gmVector3 I = ray.getOrigin() + (t * ray.getDir());
				gmVector3 w = I - v0;

				denom = (dot(u,v) * dot(u,v)) - (dot(u,u)*dot(v,v));
				double si = ((dot(u,v)*dot(w,v)) - (dot(v,v)*dot(w,u))) / denom;
				double ti = ((dot(u,v)*dot(w,u)) - (dot(u,u)*dot(w,v))) / denom;

				if((si >= 0) && (ti >= 0) && ((si + ti) <= 1))
				{
					hit.t = t;
					hit.n = norm;
					hit.r = ray.getOrigin() + (t * ray.getDir());//I - ray.getOrigin();
					ret = true;
				}
			}

			delete vertexList;
			return ret;
		}

		void print(std::ostream &os)
		{
			os << "\t\tVertices: ";
			for(unsigned int i = 0; i < this->_vertices; i++)
				os << this->_vertexL[i];
			os << std::endl;
			this->_material.print(os);
		}

		void addVertex(gmVector3 vtx)
		{
			gmVector4 vtxx;
			vtxx[0] = vtx[0];
			vtxx[1] = vtx[1];
			vtxx[2] = vtx[2];
			vtxx[3] = 1.0;
			this->_vertices++;
			gmVector4* tmp = this->_vertexL;
			this->_vertexL = new gmVector4[this->_vertices];
			memcpy(this->_vertexL, tmp, sizeof(gmVector4) * (this->_vertices - 1));
			this->_vertexL[this->_vertices - 1] = vtxx;
		}

		void setMaterial(Material mat)//gmVector3 col, gmVector3 amb, gmVector3 ref, double phg, gmVector3 re, double ri, bool reOn, bool rfOn, bool spOn)
		{
			this->_material = mat;
		}

		void transform(gmMatrix4 T)
		{
			for(unsigned int i = 0; i < this->_vertices; i++)
			{
				this->_vertexL[i] = T * this->_vertexL[i];
				this->_vertexL[i][0] = this->_vertexL[i][0] / this->_vertexL[i][3];
				this->_vertexL[i][1] = this->_vertexL[i][1] / this->_vertexL[i][3];
				this->_vertexL[i][2] = this->_vertexL[i][2] / this->_vertexL[i][3];
				this->_vertexL[i][3] = this->_vertexL[i][3] / this->_vertexL[i][3];
			}
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

	private:
		gmVector4*		_vertexL;
		unsigned int	_vertices;
		Material		_material;
};

class poly
{
	public:
		static char* begin_tag(){ return "begin_poly"; };
		static char* end_tag(){ return "end_poly"; };

		static polygon_t** read(std::istream& in, unsigned int& polys)
		{
			std::string		cmd;
			bool			seen_end = false;
			gmVector3		vec;
			gmVector3*		vecl;
			unsigned int	vecs = 0;
			polygon_t**		ret;
			polygon_t*		p;
			gmMatrix4		T;
			bool			tm = false;
			Material		mat;

			polys = 0;

			while(!in.eof() && !seen_end)
			{
				if(cmd != "poly")
					in >> cmd;
				if(cmd == "poly")
				{
					p = new polygon_t;
					in >> cmd;
					while(isdigit(cmd.at(0)))
					{
						p->addVertex(vecl[atoi(cmd.c_str())]);
						in >> cmd;
					}

					polys++;
					polygon_t** tmp = ret;
					ret = new polygon_t*[polys];
					memcpy(ret, tmp, sizeof(polygon_t*) * (polys - 1));
					ret[polys - 1] = p;
				}

				if(cmd == poly::end_tag())
					seen_end = true;
				if(cmd == "#")
					in.ignore(std::numeric_limits<int>::max(), '\n');
				else if(cmd == "vertex")
				{
					vecs++;
					in >> vec;

					gmVector3* tmp = vecl;
					vecl = new gmVector3[vecs];
					memcpy(vecl, tmp, sizeof(gmVector3) * (vecs - 1));
					vecl[vecs - 1] = vec;
				}
				else if(cmd == "begin_material")
					mat.read(in);
				else if(cmd == "transform")
				{
					in >> T;
					tm = true;
				}
			}

			for(unsigned int i = 0; i < polys; i++)
			{
				if(tm)
					ret[i]->transform(T);
				ret[i]->setMaterial(mat);//color, ambient, reflect, phong, refrExt, refrIndex, reflectOn, refractOn, specularOn);
			}

			return ret;
		}
	//private:
};

#endif
