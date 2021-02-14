//*****************************************************************************
//    file: mray.cc
// created: 1-29-2005
//  author: J. Edward Swan II
//  major modifer: Greg Walker
// purpose: Highest level ray tracer routines.
//*****************************************************************************
#include <iostream>
#include <fstream>
#include "view.h"
#include "surface.h"
#include "scene.h"
#include "light.h"
#include "../libgm/gmUtils.h"
#include "../libgm/gmMat4.h"
#include <time.h>

using namespace std;

// Indicate frame construct in input file
#define FRAME_TAG1 "begin_frame"
#define FRAME_TAG2 "end_frame"

static view_t   view;	// Hold the view
static scene_t  scene;	// Hold the scene
static ofstream fout;	// Output file stream

unsigned int TOTAL_RAYS_CAST;
struct timezone tz;
struct timeval  startTime, endTime;

extern int optind;		// Stuff for getopt

gmVector3 vectorMult(gmVector3 op1, gmVector3 op2)
{
	gmVector3 out;
	out[0] = op1[0] * op2[0];
	out[1] = op1[1] * op2[1];
	out[2] = op1[2] * op2[2];
	return out;
}

double timeElapsed()
{
	double t1,t2;
	gettimeofday(&endTime,&tz);
	t1 =  (double)startTime.tv_sec + (double)startTime.tv_usec/(1000*1000);
	t2 =  (double)endTime.tv_sec + (double)endTime.tv_usec/(1000*1000);
	return t2-t1;
}

gmVector3 rayColor(ray_t rayCast);

/*****************************************************************************/
static void frame_read(istream& ins)
{
    string cmd;    // Buffer to hold each command

    // Loop and read until we reach the end of the frame construct
    while(!ins.eof())
	{
		ins >> cmd;

		// Skip a comment line; ignore input until end-of-line
		if(cmd == "#")
			ins.ignore( numeric_limits<int>::max(), '\n' );
		
		// Skip a beginning frame tag
		else if(cmd == FRAME_TAG1);
		
		// Read in a view block
		else if(cmd == view.begin_tag())
			view.read(ins);

		// Read in a scene block
		else if(cmd == scene.begin_tag())
			scene.read(ins);
    }
}
/*****************************************************************************/
int main( int argc, char* argv[] )
{
	/** /
	gmVector3 axis(0,1,0);
	gmMatrix4 m = gmMatrix4::translate(4,2,-1);
	m = m * gmMatrix4::scale(.5,.5,.5);
	m = m * gmMatrix4::rotate(45, axis);
	axis.assign(1,0,0);
	m = m * gmMatrix4::rotate(45, axis);
	cout << m.transpose() << endl;
	exit(0);
	//*/

	bool	err = false;						// Invocation error?
	int	opt;									// Command line options
	bool	EchoInputLanguage = false;			// Show view and scene data
	bool	ShowPercentCompleteDisplay = true;	// Show progress meter
	bool	readFromFile = false;				// Whether to read from an input file (or cin)
	string	inputFile;							// Input filename
	string	outputFile;							// Output filename

	// Parse the user's command line options
	while((opt = getopt(argc, argv, "ecs")) != EOF)
	{
		switch(opt)
		{
			case 'e': EchoInputLanguage = true; break;
			case 'c': ShowPercentCompleteDisplay = false; break;
			default: err = true;
		}
	}

    	// Check for the output file and optional input file
	if(optind == argc)
	{
		cerr << "ERROR:  No output file specified\n";
		err = true;
	}
	else if((argc - optind) == 1)
	{
		// output file specified, no input file
		outputFile = argv[optind];
	}
	else if((argc - optind) == 2)
	{
		// input, output files specified
		inputFile    = argv[optind];
		outputFile   = argv[optind+1];
		readFromFile = true;
	}
	else
	{
		// Too many arguments
		cerr << "ERROR:  Unknown arguments.\n";
		err = true;
	}

	// Usage error
	if(err)
	{
		cerr << argv[0] << " [-ec] < infile > outfile" << endl;
		exit(1);
	}
	
	// Read in the data
	if(readFromFile)
	{
		// Read from file
		ifstream fin(inputFile.c_str());
		frame_read(fin);
		fin.close();
	}
	else
		// Read from CIN
		frame_read(cin);

	if(EchoInputLanguage)
		cout << "The view:\n" << view << endl << scene;

	// Open the output file for writing
	fout.open(outputFile.c_str());
	
	// PPM header informationo
	fout << "P3\n" << view.numXPixels() << " " << view.numYPixels() << "\n255\n";

	// Ray to be cast, origin = eye
	ray_t rayCast;
	rayCast.set_origin(view.getEye());
	int*** colors = new int**[view.numYPixels()];

	// Do some initial calculations before entering the ray-generation loop	
	int nx = view.numXPixels();
	int ny = view.numYPixels();

	gmVector3 u = view.getU();
	gmVector3 v = view.getV();
	gmVector3 w = view.getW();

	double vpuOnx  = view.VPWidth() / nx;
	double xOffset = (1 - nx) / 2.0;
	double vpvOny  = view.VPHeight() / ny;
	double yOffset = (1 - ny) / 2.0;
	gmVector3 sw   = -view.getFocalLen() * w;

	gmVector3 uxvpuOnx = vpuOnx * u;
	gmVector3 vxvpvOny = vpvOny * v;
	gmVector3 xComponent, yComponent;

	// Direection of the ray; origin is constant
	gmVector3 ray_dir;

	gmVector3  pcolor;		// Pixel color
	//int i = 0;
	//float i_pct = 0.00;
	//int j = 0;
	double xy = 100.0 / (ny * nx);
	double prog = 0.0;

	double tsSize = 1.0 / (scene.getSRateSqr() - 1);
	double timesteps[scene.getSRateSqr()];
	unsigned int tsIndex;

	TOTAL_RAYS_CAST = 0;
	if(ShowPercentCompleteDisplay)
	{
		cout << "\nRendering scene [ " << inputFile << " ]";
		cout << "\nRendering Progress (estimate):" << endl;
	}

	gettimeofday(&startTime,&tz);
	// Generate rays
	for(int y = 0; y < ny; y++)
	{
		colors[y] = new int*[view.numXPixels()];
		for(int x = 0; x < nx; x++)
		{
			for(unsigned int ii = 0; ii < scene.getSRateSqr(); ii++)
				timesteps[ii] = -1.0;
			for(unsigned int ii = 0; ii < scene.getSRateSqr(); ii++)
			{
				tsIndex = rand() % scene.getSRateSqr();
				while(timesteps[tsIndex] != -1.0)
				{
					tsIndex++;
					if(tsIndex == scene.getSRateSqr())
						tsIndex = 0;
				}
				timesteps[tsIndex] = (ii * tsSize) + (((double)rand() / ((double)(RAND_MAX)+(double)(1))) * tsSize);
			}

			colors[y][x] = new int[3];
			// Show the percent-complete thing
			if(ShowPercentCompleteDisplay && ((int)prog%2)==0)
			{
				prog+=xy;
				printf("   %3.1f percent - %.1f second", prog, timeElapsed());
				cout << "     \r";
				cout.flush();
			}
			else
				prog+=xy;

			pcolor[0] = 0;
			pcolor[1] = 0;
			pcolor[2] = 0;
			for(unsigned int p = 0; p < scene.getSRate(); p++)
			{
				for(unsigned int q = 0; q < scene.getSRate(); q++)
				{
					// Calculate ray direction
					double E1 = (double)rand() / ((double)(RAND_MAX)+(double)(1));
					double E2 = (double)rand() / ((double)(RAND_MAX)+(double)(1));
					if(scene.getSRate() > 1)
						ray_dir = 
							(uxvpuOnx * ((xOffset + x) + ((p + E1) / scene.getSRate()))) +
							(vxvpvOny * ((yOffset + y) + ((q + E2) / scene.getSRate()))) +
							sw;
					else
						ray_dir =
							(uxvpuOnx * (xOffset + x)) +
							(vxvpvOny * (yOffset + y)) +
							sw;
					// And set it
					ray_dir.normalize();
					rayCast.set_dir(ray_dir);
					rayCast.set_time(timesteps[(p * scene.getSRate()) + q]);

					pcolor += rayColor(rayCast);
				}
			}
			colors[y][x][0] = ((int)((pcolor[0]/(double)scene.getSRateSqr()) * 255));
			colors[y][x][1] = ((int)((pcolor[1]/(double)scene.getSRateSqr()) * 255));
			colors[y][x][2] = ((int)((pcolor[2]/(double)scene.getSRateSqr()) * 255));
		}
	}
	if(ShowPercentCompleteDisplay)
		cout << "   Rendering finished                   \n\nFile output progress:" << endl;

	double totalPX = ny * nx / 100.0;
	int yy = 0;
	for(int y = (view.numYPixels() - 1); y >= 0; y--)
	{
		for(unsigned int x = 0; x < view.numXPixels(); x++)
		{
			fout << colors[y][x][0] << " ";
			fout << colors[y][x][1] << " ";
			fout << colors[y][x][2] << " ";
			if(ShowPercentCompleteDisplay)
			{
				printf("   %3.1f\r", (((double)((yy * view.numYPixels()) + x)) / totalPX));
				//cout << "%\r";
				cout.flush();
			}
		}
		yy++;
	}
	if(ShowPercentCompleteDisplay)
		cout << "   File output finished\n" << endl;

	cout << "Total Rays Cast:  " << TOTAL_RAYS_CAST << endl;

	// Close the output file, exit
	fout.close();
	return 0;
}

bool refract(gmVector3 d, gmVector3 norm, double refrIndex, gmVector3& t)
{
	double denom = 1 - (1/refrIndex/refrIndex) * (1 - (dot(-d,norm) * dot(-d, norm)));
	if(denom <= 0)
		return false;	// total internal reflection

	t = (1/refrIndex) * (d + (norm * dot(-d, norm))) - (norm * sqrt(denom));
	return true;
}

gmVector3 doGlossOrBlur(gmVector3 r, double glossOrBlur, unsigned int rayNumber)
{
	gmVector3 u,v,w,t;
	double uoffset, voffset, rnd1, rnd2;
	w = r;
	t = w;
	w.normalize();
	if((t[0] < t[1]) && (t[0] < t[2]))
		t[0] = 1;
	else if((t[1] < t[0]) && (t[1] < t[2]))
		t[1] = 1;
	else
		t[2] = 1;
	//t.normalize();
	u = cross(t,w);
	u.normalize();
	v = cross(w,u);
	//v.normalize();

	//do
	//{
	rnd1 = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));// / scene.getSRate();
	rnd2 = ((double)rand() / ((double)(RAND_MAX)+(double)(1)));// / scene.getSRate();
	//}while(((rnd1 * rnd1) + (rnd2 * rnd2)) > 1);

	uoffset = (-(glossOrBlur / 2) + (rnd1 * glossOrBlur));// + (int)(rayNumber / scene.getSRateSqr());
	voffset = (-(glossOrBlur / 2) + (rnd2 * glossOrBlur));// + (rayNumber %  scene.getSRateSqr());

	r = r + (uoffset * u) + (voffset * v);
	r.normalize();
	return r;
}

gmVector3 rayColor(ray_t rayCast)
{
	//std::cout << rayCast << std::endl;
	TOTAL_RAYS_CAST++;

	// Pixel color
	gmVector3 pcolor = scene.getBGColor();
	gmVector3 refCol;
	gmVector3 reflectivity;
	ray_t     reflect;

	ray_t refractRay;

	surface_t  *s,*s2;		// Surface to test for ray intersection
	hit_t      hitInfo;		// Surface/ray intersection information
	double     dist = -1;	// Distance to furthest surface

	Light*    l;					// Light being examined/used
	gmVector3 dir2light;			// Direction from intersection to light
	gmVector3 intersectionPoint;	// Point of intersection
	bool      obs;					// Path to light obstructed?
	ray_t     sToL;					// Ray, for testing obstruction
	hit_t     hitHolder;			// Holds some stuff
	double    dotP;					// Holds dot product stuff
	gmVector3 r;					// Reflection vector
	gmMatrix4 T;					// Time transformation matrix

	bool hitSomething = false;		// Hit anything?

	// Check for intersection with all surfaces in the scene
	for(unsigned int surfaceIndex = 0; surfaceIndex < scene.numSurfaces(); surfaceIndex++)
	{
		s = scene.fetchSurface(surfaceIndex);

		// If there's an intersection at this color, check the
		// distance.  If this intersection is closer than all
		// previous intersections, store this surface's color
		// for this pixel
		if(s->intersect(rayCast, gmEPSILON, numeric_limits<double>::max(), hitInfo))
		{
			hitSomething = true;
            if(hitInfo.t < dist || dist == -1)
			{
				hitInfo.r = rayCast.getOrigin() + (hitInfo.t * rayCast.getDir());

				if(scene.lightingEnabled())
				{
					pcolor = s->getColor();

					// ambient light
                    pcolor = vectorMult(s->getAmbient(), pcolor);//s->getColor());

					/**/
					sToL.set_origin(hitInfo.r);
					for(unsigned int lightIndex = 0; lightIndex < scene.numLights(); lightIndex++)
					{
						l = scene.fetchLight(lightIndex);

						if(l->isPositional())
							dir2light = l->getDirPos() - hitInfo.r;
						else
							dir2light = -l->getDirPos();
						sToL.set_dir(dir2light);
						sToL.set_time(rayCast.getTime());

						obs = false;
						for(unsigned int i = 0; i < scene.numSurfaces(); i++)
						{
							s2 = scene.fetchSurface(i);
							if(s2->intersect(sToL, gmEPSILON, dir2light.length(), hitHolder))
							{
								obs = true;
								break;
							}
						}

					if(s->doRefract())//rayCast.bounce() && 
					{
                        gmVector3 k;
						gmVector3 refr = s->getRefractE();
						gmVector3 t;
						double c, R0, R;
						refractRay.set_origin(hitInfo.r + (rayCast.getDir() * gmEPSILON));
						refractRay.set_time(rayCast.getTime());

						//std::cout << "REFRACT: " << rayCast << std::endl;
						if(dot(rayCast.getDir(), hitInfo.n) < 0) // comes from outside
						{
							refract(rayCast.getDir(), hitInfo.n, s->getRefractI(), t);
							c = -dot(rayCast.getDir(), hitInfo.n);
							R0  = ((s->getRefractI() - 1)*(s->getRefractI() - 1));
							R0 /= ((s->getRefractI() + 1)*(s->getRefractI() + 1));
							R = R0 + (1 - R0) * pow((1 - c),5);

							if(s->doBlur())
								t = doGlossOrBlur(t, s->getBlur(), 0);

							refractRay.set_dir(t);
							refractRay.setGen(rayCast.gen());
							//std::cout << "REFRACT: " << refractRay << std::endl;
							refCol = rayColor(refractRay);
							if(refCol[0] < 0)
								refCol *= -1;

							gmClamp(refCol[0],0,1);
							gmClamp(refCol[1],0,1);
							gmClamp(refCol[2],0,1);

							//std::cout << R << " | " << (1-R) << std::endl;
							pcolor = (R * pcolor) + ((1 - R) * refCol);
							gmClamp(pcolor[0],0,1);
							gmClamp(pcolor[1],0,1);
							gmClamp(pcolor[2],0,1);
						}
						else	// comes from inside
						{
							k[0] = exp(refr[0] * hitInfo.t);
							k[1] = exp(refr[1] * hitInfo.t);
							k[2] = exp(refr[2] * hitInfo.t);
							if(!refract(rayCast.getDir(), -hitInfo.n, 1/s->getRefractI(), t))
								pcolor = vectorMult(k, pcolor);
							else
							{
								c = dot(t, hitInfo.n);
								R0  = ((s->getRefractI() - 1)*(s->getRefractI() - 1));
								R0 /= ((s->getRefractI() + 1)*(s->getRefractI() + 1));
								R = R0 + (1 - R0) * pow((1 - c),5);

								if(s->doBlur())
									t = doGlossOrBlur(t, s->getBlur(), 0);
								refractRay.set_dir(t);
								refractRay.setGen(rayCast.gen());
								//std::cout << "REFRACT: " << refractRay << std::endl;
								refCol = rayColor(refractRay);
								if(refCol[0] < 0)
									refCol *= -1;

								gmClamp(refCol[0],0,1);
								gmClamp(refCol[1],0,1);
								gmClamp(refCol[2],0,1);

								pcolor = vectorMult(k,(R * pcolor) + ((1 - R) * refCol));
								gmClamp(pcolor[0],0,1);
								gmClamp(pcolor[1],0,1);
								gmClamp(pcolor[2],0,1);
							}
						}
					}

						if(!obs)
						{
							// diffuse light
							dir2light.normalize();
							dotP = dot(dir2light,hitInfo.n);
							dotP = (dotP < 0) ? 0 : (dotP > 1) ? 1 : dotP;
							
							pcolor += (vectorMult(s->getColor(), l->getColor()) * dotP);
                            
							// specular highlight
							if(s->doSpecular() > 0)
							{
								r = (hitInfo.n * 2 * dotP) - dir2light;
								dotP = dot(-rayCast.getDir(), r);
								dotP = (dotP < 0) ? 0 : (dotP > 1) ? 1 : dotP;
								pcolor += (l->getColor() * pow(dotP, s->getPhong()));
							}
						}
					}//*/

					reflectivity = s->getReflect();
					
					//rayCast.setGen(rayCast.gen() - 1);
					if(rayCast.bounce() && s->doReflect())
					{
						/**/
						dotP = dot(-rayCast.getDir(), hitInfo.n);
						r = rayCast.getDir() + (2 * dotP * hitInfo.n);
                        r.normalize();
						reflect.set_origin(hitInfo.r);
						reflect.setGen(rayCast.gen());
						reflect.set_time(rayCast.getTime());

						if(s->doGloss())
							r = doGlossOrBlur(r, s->getGloss(), 0);
						reflect.set_dir(r);
						refCol = rayColor(reflect);

						if(refCol[0] >= 0)
						{
							gmClamp(refCol[0], 0, 1);
							gmClamp(refCol[1], 0, 1);
							gmClamp(refCol[2], 0, 1);

							pcolor[0] = (pcolor[0] * (1 - reflectivity[0])) + (refCol[0] * reflectivity[0]);
							pcolor[1] = (pcolor[1] * (1 - reflectivity[1])) + (refCol[1] * reflectivity[1]);
							pcolor[2] = (pcolor[2] * (1 - reflectivity[2])) + (refCol[2] * reflectivity[2]);

							gmClamp(pcolor[0],0,1);
							gmClamp(pcolor[1],0,1);
							gmClamp(pcolor[2],0,1);
						}
						//*/
					}					
				}
				else
					pcolor = s->getColor();
				dist = hitInfo.t;
			}
		}
	}
	gmClamp(pcolor[0],0,1);
	gmClamp(pcolor[1],0,1);
	gmClamp(pcolor[2],0,1);

	if(!hitSomething && rayCast.gen() > 0)
	{
		//pcolor.assign(-1,-1,-1);
		if(pcolor[0] == 0)
			pcolor[0] = 0.0001;
		pcolor *= -1;
	}
	return pcolor;
}
