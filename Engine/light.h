#ifndef __LIGHT_H
#define __LIGHT_H

#include "../libgm/gmVec3.h"
#include <iostream>
#include <string>

class Light
{
	public:
		Light()
		{
			this->_dirpos.assign(0,0,0);
			this->_posdir = false;
		}
		~Light(){}

		static std::string begin_tag()
		{
			return "begin_light";
		}

		void read(std::istream& ins)
		{
			bool seen_end_tag = false;
			std::string cmd;

			while(!ins.eof() && !seen_end_tag)
			{
				ins >> cmd;
				if(cmd == "end_light")
					seen_end_tag = true;
				else if(cmd == "#")
					ins.ignore(std::numeric_limits<int>::max(), '\n');
				else if(cmd == "position")
				{
					this->_posdir = true;
					ins >> this->_dirpos;
				}
				else if(cmd == "direction")
				{
					this->_posdir = false;
					ins >> this->_dirpos;
				}
				else if(cmd == "color")
					ins >> this->_color;
				//else if(cmd == "diffuse")
				//	ins >> this->_diffuse;
				//else if(cmd == "specular")
				//	ins >> this->_specular;
			}
		}

		gmVector3 getDirPos(){ return this->_dirpos; }
		gmVector3 getColor() { return this->_color; }
		bool      isPositional(){ return this->_posdir; }

		friend std::ostream& operator<<(std::ostream &os, Light &l);

	private:
		gmVector3 _dirpos;
		bool      _posdir;	// false if a directional light, true if positional
		gmVector3 _color;
};

std::ostream& operator<<(std::ostream &os, Light& l)
{
	os << "\t\t";
	if(l._posdir)
		os << "Position:  ";
	else
		os << "Direction: ";
	os << l._dirpos << std::endl;

	os << "\t\tAmbient:   " << l._color << std::endl;
	//os << "\t\tDiffuse:   " << l._diffuse << std::endl;
	//os << "\t\tSpecular:  " << l._specular << std::endl;

	return os;
}

#endif
