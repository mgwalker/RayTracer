#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <string>
#include <limits>
#include "../libgm/gmMat4.h"

class Material
{
	public:
		Material()
		{
			this->_phong = 0;
			this->_reflectOn = false;
			this->_refractOn = false;
			this->_specularOn = false;
			this->_refrIndex = 0;
			this->_txformOn = false;
			this->_blurOn = false;
			this->_glossOn = false;
		}
		void read(std::istream& ins)
		{
			std::string	cmd;
			bool seen_end = false;

			while(!ins.eof() && !seen_end)
			{
				ins >> cmd;
				if(cmd == "end_material")
					seen_end = true;
				else if(cmd == "color")
					ins >> this->_color;
				else if(cmd == "ambient")
					ins >> this->_ambient;
				else if(cmd == "phong")
				{
					ins >> this->_phong;
					this->_specularOn = true;
				}
				else if(cmd == "reflectivity")
				{
					ins >> this->_refl;
					this->_reflectOn = true;
				}
				else if(cmd == "refract_extinct")
				{
					ins >> this->_refrExtinct;
					this->_refrExtinct[0] = -log(this->_refrExtinct[0]);
					this->_refrExtinct[1] = -log(this->_refrExtinct[1]);
					this->_refrExtinct[2] = -log(this->_refrExtinct[2]);
					this->_refractOn = true;
				}
				else if(cmd == "refract_index")
				{
					ins >> this->_refrIndex;
					this->_refractOn = true;
				}
				else if(cmd == "time_xform")
				{
					ins >> this->_txform;
					this->_txformOn = true;
				}
				else if(cmd == "blur")
				{
					ins >> this->_blur;
					if(this->_blur > 0)
						this->_blurOn = true;
				}
				else if(cmd == "gloss")
				{
					ins >> this->_gloss;
					if(this->_gloss > 0)
						this->_glossOn = true;
				}
				else if(cmd == "#")
					ins.ignore(std::numeric_limits<int>::max(), '\n');
			}
		}
		void print(std::ostream &os)
		{
			os << "\t\t\tColor:        ";
			os << this->_color << std::endl;
			os << "\t\t\tAmbient:      ";
			os << this->_ambient << std::endl;
			if(this->_specularOn)
			{
				os << "\t\t\tPhong:        ";
				os << this->_phong << std::endl;
			}
			if(this->_reflectOn)
			{
				os << "\t\t\tReflectivity: ";
				os << this->_refl << std::endl;
			}
			if(this->_refractOn)
			{
				os << "\t\t\tRefractive E: ";
				os << this->_refrExtinct << std::endl;
				os << "\t\t\tRefractive I: ";
				os << this->_refrIndex << std::endl;
			}
			if(this->_txformOn)
			{
				os << "\t\t\tTime Transform:\n";
				os << "\t\t\t\t" << this->_txform[0][0] << " " << this->_txform[0][1] << " " << this->_txform[0][2] << " " << this->_txform[0][3] << std::endl;
				os << "\t\t\t\t" << this->_txform[1][0] << " " << this->_txform[1][1] << " " << this->_txform[1][2] << " " << this->_txform[1][3] << std::endl;
				os << "\t\t\t\t" << this->_txform[2][0] << " " << this->_txform[2][1] << " " << this->_txform[2][2] << " " << this->_txform[2][3] << std::endl;
				os << "\t\t\t\t" << this->_txform[3][0] << " " << this->_txform[3][1] << " " << this->_txform[3][2] << " " << this->_txform[3][3] << std::endl;
			}
			if(this->_blurOn)
				os << "\t\t\tBlur:         " << this->_blur << std::endl;
			if(this->_glossOn)
				os << "\t\t\tGloss:        " << this->_gloss << std::endl;
		}

	//private:
		gmVector3	_color;
		gmVector3	_ambient;
		double		_phong;
		gmVector3	_refl;
		gmVector3	_refrExtinct;
		double		_refrIndex;
		bool		_reflectOn;
		bool		_refractOn;
		bool		_specularOn;
		gmMatrix4	_txform;
		bool		_txformOn;
		double		_gloss;
		double		_blur;
		bool		_glossOn;
		bool		_blurOn;
	/*
	 * color           - gmVector3
	 * ambient         - gmVector3
	 * phong           - double
	 * reflectivity    - gmVector3
	 * refract_extinct - gmVector3
	 * refract_index   - double
	 */
};

#endif
