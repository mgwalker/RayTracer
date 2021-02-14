//*****************************************************************************
//    file: view.h
// created: 1-29-2005
//  author: J. Edward Swan II
// purpose: Define a view class
//*****************************************************************************

#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include "../libgm/gmVec3.h"

class view_t
{
	public:
		view_t();	// Constructor
		~view_t();	// Destructor

	void read(std::istream& ins);
	// requires: ins contains the view commands in any order: "coi, eye, vwidth, 
	//           aspect, xres, focal, up", followed by "end_view".  ins may contain
	//           comment lines beginning with "#".
	//  ensures: The commands are read from ins and the view fields are set to the
	//           appropriate values.  If ins contains multiple copies of the same 
	//           command, the values given by the last command instance take effect.

	friend std::ostream& operator << ( std::ostream& os, const view_t& );
	//  ensures: A representation of the view is printed to os.

	// Tags that delimit a view block
	char* begin_tag() { return "begin_view"; }
	char* end_tag()   { return "end_view"; }

	unsigned int numXPixels()  { return this->num_x_pixels; }
	unsigned int numYPixels()  { return this->num_y_pixels; }
	double       VPWidth()     { return this->viewport_u; }
	double       VPHeight()    { return this->viewport_v; }
	gmVector3    getEye()      { return this->eye; }
	gmVector3    getU()        { return this->u; }
	gmVector3    getV()        { return this->v; }
	gmVector3    getW()        { return this->w; }
	double       getFocalLen() { return this->focal_length; }

	private:
		// Parameters calculated from public items
		gmVector3 u,v,w;		// Viewing frame; a right-handed Cartesian coord frame.
								// u = width, v = height, w = points backward from coi
		double    viewport_v;	// Viewport height
		unsigned  num_y_pixels;	// Num vertical pixels

		// Parameters read in from view block
		gmVector3 coi;			// Center of interest: where are we looking
		gmVector3 eye;			// Eye point: where is our eye?
		double    viewport_u;	// Viewport width
		double    aspect_ratio;	// Aspect ratio = width / height
		unsigned  num_x_pixels;	// Num horizontal pixels
		double    focal_length;	// Distance from eye to near clipping plane
		gmVector3 up;			// Approximate viewport up vector

};

#endif
