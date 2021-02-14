//*****************************************************************************
//    file: hit.h
// created: 1-30-2005
//  author: J. Edward Swan II
// purpose: Hold needed information when a ray hits a surface
//*****************************************************************************

#ifndef HIT_H
#define HIT_H

#include <iostream>
#include "../libgm/gmVec3.h"

class hit_t 
{
	public:
		double    t;	// Parametric ray distance where hit occurs
		gmVector3 r;	// Intersection ray
		gmVector3 n;	// Normal at point of intersection
};

#endif
