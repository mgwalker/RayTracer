//*****************************************************************************
//    file: view.cc
// created: 1-29-2005
//  author: J. Edward Swan II
// purpose: Define a view class
//*****************************************************************************

#include "view.h"
#include <limits>
#include <string>

using namespace std;

/*****************************************************************************/
view_t::view_t( void )
{
}

/*****************************************************************************/
view_t::~view_t( void )
{
}

/*****************************************************************************/
void view_t::read( istream& ins )
{
	string cmd;						// Buffer to hold each command
	bool   seen_end_tag = false;	// Stop reading at end of view block
	bool   seen_coi     = false;	// Must see each command
	bool   seen_eye     = false;
	bool   seen_vwidth  = false;
	bool   seen_aspect  = false;
	bool   seen_xres    = false;
	bool   seen_focal   = false;
	bool   seen_up      = false;

    // Loop and read until we reach the end of the view construct
	while(!ins.eof() && !seen_end_tag)
	{
		ins >> cmd;

		// Skip a comment line; ignore input until end-of-line
		if(cmd == "#")
			ins.ignore( numeric_limits<int>::max(), '\n' );

		// Detect end of the block
		else if(cmd == this->end_tag())
			seen_end_tag = true;

		// Read in commands
		else if(cmd == "coi")
		{
			ins >> coi;
			seen_coi = true;
		}
		else if(cmd == "eye")
		{
			ins >> eye;
			seen_eye = true;
		}
		else if(cmd == "vwidth")
		{
			ins >> viewport_u;
			seen_vwidth = true;
		}
		else if(cmd == "aspect")
		{
			ins >> aspect_ratio;
			seen_aspect = true;
		}
		else if(cmd == "xres")
		{
			ins >> num_x_pixels;
			seen_xres = true;
		}
		else if(cmd == "focal")
		{
			ins >> focal_length;
			seen_focal = true;
		}
		else if(cmd == "up")
		{
			ins >> up;
			seen_up = true;
		}
	}

	// Make sure we've seen every command at least once
	if(!(seen_coi && seen_eye && seen_vwidth && seen_aspect &&
		 seen_xres && seen_focal && seen_up))
		cerr << "Error: did not see a necessary view command in view block!" << endl;

    // Calculate the private parameters that depend on what was read in above
	viewport_v = viewport_u / aspect_ratio;
	num_y_pixels = (unsigned)gmRound( num_x_pixels / aspect_ratio );
	up.normalize();

	// Calculate u,v,w coordinate frame
	w = -( coi - eye );
	w.normalize();
	u = cross( up, w );
	v = cross( w, u );
}

/*****************************************************************************/
ostream& operator << ( ostream& os, const view_t& v )
{
    os << "\tCOI:            " << v.coi          << "\n";
    os << "\tEye:            " << v.eye          << "\n";
    os << "\tWidth x Height: " << v.viewport_u   << " x " << v.viewport_v << "\n";
    os << "\tAspect Ratio:   " << v.aspect_ratio << "\n";
    os << "\tX-res x Y-res:  " << v.num_x_pixels << " x " << v.num_y_pixels << "\n";
    os << "\tFocal:          " << v.focal_length << "\n";
    os << "\tUp:             " << v.up           << "\n";
    os << "\tU,V,W:          " << v.u << v.v << v.w << "\n";
    return os;
}
