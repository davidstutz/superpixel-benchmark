// SLIC.cpp: implementation of the SLIC class.
//
// Copyright (C) Radhakrishna Achanta 2012
// All rights reserved
// Email: firstname.lastname@epfl.ch
//////////////////////////////////////////////////////////////////////
#include <cfloat>
#include <cmath>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "SLIC.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SLIC::SLIC()
{
	m_lvec = NULL;
	m_avec = NULL;
	m_bvec = NULL;

    m_xvec = NULL;
    m_yvec = NULL;
    m_zvec = NULL;
    
	m_lvecvec = NULL;
	m_avecvec = NULL;
	m_bvecvec = NULL;
}

SLIC::~SLIC()
{
	if(m_lvec) delete [] m_lvec;
	if(m_avec) delete [] m_avec;
	if(m_bvec) delete [] m_bvec;

    if(m_xvec) delete [] m_xvec;
	if(m_yvec) delete [] m_yvec;
	if(m_zvec) delete [] m_zvec;
    
	if(m_lvecvec)
	{
		for( int d = 0; d < m_depth; d++ ) delete [] m_lvecvec[d];
		delete [] m_lvecvec;
	}
	if(m_avecvec)
	{
		for( int d = 0; d < m_depth; d++ ) delete [] m_avecvec[d];
		delete [] m_avecvec;
	}
	if(m_bvecvec)
	{
		for( int d = 0; d < m_depth; d++ ) delete [] m_bvecvec[d];
		delete [] m_bvecvec;
	}
}

//==============================================================================
///	RGB2XYZ
///
/// sRGB (D65 illuninant assumption) to XYZ conversion
//==============================================================================
void SLIC::RGB2XYZ(
	const int&		sR,
	const int&		sG,
	const int&		sB,
	float&			X,
	float&			Y,
	float&			Z)
{
	float R = sR/255.0;
	float G = sG/255.0;
	float B = sB/255.0;

	float r, g, b;

	if(R <= 0.04045)	r = R/12.92;
	else				r = pow((R+0.055)/1.055,2.4);
	if(G <= 0.04045)	g = G/12.92;
	else				g = pow((G+0.055)/1.055,2.4);
	if(B <= 0.04045)	b = B/12.92;
	else				b = pow((B+0.055)/1.055,2.4);

	X = r*0.4124564 + g*0.3575761 + b*0.1804375;
	Y = r*0.2126729 + g*0.7151522 + b*0.0721750;
	Z = r*0.0193339 + g*0.1191920 + b*0.9503041;
}

//===========================================================================
///	RGB2LAB
//===========================================================================
void SLIC::RGB2LAB(const int& sR, const int& sG, const int& sB, float& lval, float& aval, float& bval)
{
	//------------------------
	// sRGB to XYZ conversion
	//------------------------
	float X, Y, Z;
	RGB2XYZ(sR, sG, sB, X, Y, Z);

	//------------------------
	// XYZ to LAB conversion
	//------------------------
	float epsilon = 0.008856;	//actual CIE standard
	float kappa   = 903.3;		//actual CIE standard

	float Xr = 0.950456;	//reference white
	float Yr = 1.0;		//reference white
	float Zr = 1.088754;	//reference white

	float xr = X/Xr;
	float yr = Y/Yr;
	float zr = Z/Zr;

	float fx, fy, fz;
	if(xr > epsilon)	fx = pow(xr, 1.0/3.0);
	else				fx = (kappa*xr + 16.0)/116.0;
	if(yr > epsilon)	fy = pow(yr, 1.0/3.0);
	else				fy = (kappa*yr + 16.0)/116.0;
	if(zr > epsilon)	fz = pow(zr, 1.0/3.0);
	else				fz = (kappa*zr + 16.0)/116.0;

	lval = 116.0*fy-16.0;
	aval = 500.0*(fx-fy);
	bval = 200.0*(fy-fz);
}

//===========================================================================
///	DoRGBtoLABConversion
///
///	For whole image: overlaoded floating point version
//===========================================================================
void SLIC::DoRGBtoLABConversion(
	const unsigned int*&		ubuff,
	float*&					lvec,
	float*&					avec,
	float*&					bvec)
{
	int sz = m_width*m_height;
	lvec = new float[sz];
	avec = new float[sz];
	bvec = new float[sz];

	for( int j = 0; j < sz; j++ )
	{
		int r = (ubuff[j] >> 16) & 0xFF;
		int g = (ubuff[j] >>  8) & 0xFF;
		int b = (ubuff[j]      ) & 0xFF;

		RGB2LAB( r, g, b, lvec[j], avec[j], bvec[j] );
	}
}

//===========================================================================
///	DoRGBtoLABConversion
///
/// For whole volume
//===========================================================================
void SLIC::DoRGBtoLABConversion(
	unsigned int**&		ubuff,
	float**&					lvec,
	float**&					avec,
	float**&					bvec)
{
	int sz = m_width*m_height;
	for( int d = 0; d < m_depth; d++ )
	{
		for( int j = 0; j < sz; j++ )
		{
			int r = (ubuff[d][j] >> 16) & 0xFF;
			int g = (ubuff[d][j] >>  8) & 0xFF;
			int b = (ubuff[d][j]      ) & 0xFF;

			RGB2LAB( r, g, b, lvec[d][j], avec[d][j], bvec[d][j] );
		}
	}
}

//=================================================================================
/// DrawContoursAroundSegments
///
/// Internal contour drawing option exists. One only needs to comment the if
/// statement inside the loop that looks at neighbourhood.
//=================================================================================
void SLIC::DrawContoursAroundSegments(
	unsigned int*&			ubuff,
	int*&					labels,
	const int&				width,
	const int&				height,
	const unsigned int&				color )
{
	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

/*	int sz = width*height;

	vector<bool> istaken(sz, false);

	int mainindex(0);
	for( int j = 0; j < height; j++ )
	{
		for( int k = 0; k < width; k++ )
		{
			int np(0);
			for( int i = 0; i < 8; i++ )
			{
				int x = k + dx8[i];
				int y = j + dy8[i];

				if( (x >= 0 && x < width) && (y >= 0 && y < height) )
				{
					int index = y*width + x;

					if( false == istaken[index] )//comment this to obtain internal contours
					{
						if( labels[mainindex] != labels[index] ) np++;
					}
				}
			}
			if( np > 1 )//change to 2 or 3 for thinner lines
			{
				ubuff[mainindex] = color;
				istaken[mainindex] = true;
			}
			mainindex++;
		}
	}*/


	int sz = width*height;
	vector<bool> istaken(sz, false);
	vector<int> contourx(sz);vector<int> contoury(sz);
	int mainindex(0);int cind(0);
	for( int j = 0; j < height; j++ )
	{
		for( int k = 0; k < width; k++ )
		{
			int np(0);
			for( int i = 0; i < 8; i++ )
			{
				int x = k + dx8[i];
				int y = j + dy8[i];

				if( (x >= 0 && x < width) && (y >= 0 && y < height) )
				{
					int index = y*width + x;

					//if( false == istaken[index] )//comment this to obtain internal contours
					{
						if( labels[mainindex] != labels[index] ) np++;
					}
				}
			}
			if( np > 1 )
			{
				contourx[cind] = k;
				contoury[cind] = j;
				istaken[mainindex] = true;
				//img[mainindex] = color;
				cind++;
			}
			mainindex++;
		}
	}

	int numboundpix = cind;//int(contourx.size());
	for( int j = 0; j < numboundpix; j++ )
	{
		int ii = contoury[j]*width + contourx[j];
		ubuff[ii] = 0xffffff;

		for( int n = 0; n < 8; n++ )
		{
			int x = contourx[j] + dx8[n];
			int y = contoury[j] + dy8[n];
			if( (x >= 0 && x < width) && (y >= 0 && y < height) )
			{
				int ind = y*width + x;
				if(!istaken[ind]) ubuff[ind] = 0;
			}
		}
	}
}


//==============================================================================
///	DetectLabEdges
//==============================================================================
void SLIC::DetectLabEdges(
	const float*				lvec,
	const float*				avec,
	const float*				bvec,
	const int&					width,
	const int&					height,
	vector<float>&				edges)
{
	int sz = width*height;

	edges.resize(sz,0);
	for( int j = 1; j < height-1; j++ )
	{
		for( int k = 1; k < width-1; k++ )
		{
			int i = j*width+k;

			float dx = (lvec[i-1]-lvec[i+1])*(lvec[i-1]-lvec[i+1]) +
						(avec[i-1]-avec[i+1])*(avec[i-1]-avec[i+1]) +
						(bvec[i-1]-bvec[i+1])*(bvec[i-1]-bvec[i+1]);

			float dy = (lvec[i-width]-lvec[i+width])*(lvec[i-width]-lvec[i+width]) +
						(avec[i-width]-avec[i+width])*(avec[i-width]-avec[i+width]) +
						(bvec[i-width]-bvec[i+width])*(bvec[i-width]-bvec[i+width]);

			//edges[i] = fabs(dx) + fabs(dy);
			edges[i] = dx*dx + dy*dy;
		}
	}
}

//===========================================================================
///	PerturbSeeds
//===========================================================================
void SLIC::PerturbSeeds(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
	vector<float>&				kseedsx,
	vector<float>&				kseedsy,
        const vector<float>&                   edges)
{
	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};
	
	int numseeds = kseedsl.size();

	for( int n = 0; n < numseeds; n++ )
	{
		int ox = kseedsx[n];//original x
		int oy = kseedsy[n];//original y
		int oind = oy*m_width + ox;

		int storeind = oind;
		for( int i = 0; i < 8; i++ )
		{
			int nx = ox+dx8[i];//new x
			int ny = oy+dy8[i];//new y

			if( nx >= 0 && nx < m_width && ny >= 0 && ny < m_height)
			{
				int nind = ny*m_width + nx;
				if( edges[nind] < edges[storeind])
				{
					storeind = nind;
				}
			}
		}
		if(storeind != oind)
		{
			kseedsx[n] = storeind%m_width;
			kseedsy[n] = storeind/m_width;
			kseedsl[n] = m_lvec[storeind];
			kseedsa[n] = m_avec[storeind];
			kseedsb[n] = m_bvec[storeind];
		}
	}
}

//===========================================================================
///	PerturbSeeds
//===========================================================================
void SLIC::PerturbSeeds(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
    vector<float>&             kseedsox,
    vector<float>&             kseedsoy,
	vector<float>&				kseedsx,
	vector<float>&				kseedsy,
    vector<float>&				kseedsz,
        const vector<float>&                   edges)
{
	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};
	
	int numseeds = kseedsl.size();

	for( int n = 0; n < numseeds; n++ )
	{
		int ox = kseedsox[n];//original x
		int oy = kseedsoy[n];//original y
		int oind = oy*m_width + ox;

		int storeind = oind;
		for( int i = 0; i < 8; i++ )
		{
			int nx = ox+dx8[i];//new x
			int ny = oy+dy8[i];//new y

			if( nx >= 0 && nx < m_width && ny >= 0 && ny < m_height)
			{
				int nind = ny*m_width + nx;
				if( edges[nind] < edges[storeind])
				{
					storeind = nind;
				}
			}
		}
		if(storeind != oind)
		{
            int storex = storeind%m_width;
            int storey = storeind/m_width;
            
			kseedsox[n] = storex;
			kseedsoy[n] = storey;
            kseedsx[n] = m_xvec[storeind];
			kseedsy[n] = m_yvec[storeind];
            kseedsz[n] = m_zvec[storeind];
			kseedsl[n] = m_lvec[storeind];
			kseedsa[n] = m_avec[storeind];
			kseedsb[n] = m_bvec[storeind];
		}
	}
}

//===========================================================================
///	GetLABXYSeeds_ForGivenSeedProbability
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void SLIC::GetLABXYSeeds_ForGivenSeedProbabilities(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
	vector<float>&				kseedsx,
	vector<float>&				kseedsy,
    const float&                                variance,
    const unsigned short*                        depth,
        const int                               superpixels,
    const bool&					perturbseeds,
    const vector<float>&       edgemag)
{
    // Get max and min depth.
    float D = 0;
    for (int i = 0; i < m_width*m_height; i++)
    {
        D += depth[i];
    }
    
    // Compute probability map.
    float* p = new float[m_width*m_height];
    for (int x = 0; x < m_width; x++)
    {
        for (int y = 0; y < m_height; y++)
        {
            int i = y*m_width + x;
            
            p[i] = depth[i]/D;
        }
    }
    
    kseedsl.resize(superpixels);
    kseedsa.resize(superpixels);
    kseedsb.resize(superpixels);
    kseedsx.resize(superpixels);
    kseedsy.resize(superpixels);
    
    const float M = 1;
    const float q = 1/((float) (m_width*m_height));
    
    int S = 0;
    while (S < superpixels)
    {
        int i = std::rand () % m_width*m_height;
        float u = std::rand () / (float) RAND_MAX;
        
        if (u < depth[i]/(M*q))
        {
            int x = i%m_width;
            int y = i/m_width;
            
            assert(x < m_width);
            assert(y < m_height);
            
            kseedsl[S] = m_lvec[i];
            kseedsa[S]= m_avec[i];
            kseedsb[S] = m_bvec[i];
            
            kseedsx[S] = x;
            kseedsy[S] = y;
            
            // Adapt the probability according to depth.
            float g_variance = variance/depth[i];
            
            for (int xx = std::max(0, int(x - 3*g_variance)); 
                    xx < std::min(m_width - 1, int(x + 3*g_variance)); xx++)
            {
                for (int yy = std::max(0, int(y - 3*g_variance)); 
                        yy < std::min(m_height - 1, int(y + 3*g_variance)); yy++)
                {
                    int ii = yy*m_width + xx;
                    
                    float g = 1/(2*M_PI*g_variance) * std::exp(- ((x - xx)*(x - xx) + (y - yy)*(y - yy))/(2*g_variance*g_variance));
                    p[ii] = std::max(0.f, p[i] - g);
                    
                    // Ignore renormalizing.
                }
            }
            
            S++;
        }
    }
    
    assert (kseedsl.size () == superpixels);
    
	if(perturbseeds)
	{
		PerturbSeeds(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, edgemag);
	}
    
//    const int STEP = 20;
//    const bool hexgrid = false;
//	int numseeds(0);
//	int n(0);
//
//	//int xstrips = m_width/STEP;
//	//int ystrips = m_height/STEP;
//	int xstrips = (0.5+float(m_width)/float(STEP));
//	int ystrips = (0.5+float(m_height)/float(STEP));
//
//    int xerr = m_width  - STEP*xstrips;if(xerr < 0){xstrips--;xerr = m_width - STEP*xstrips;}
//    int yerr = m_height - STEP*ystrips;if(yerr < 0){ystrips--;yerr = m_height- STEP*ystrips;}
//
//	float xerrperstrip = float(xerr)/float(xstrips);
//	float yerrperstrip = float(yerr)/float(ystrips);
//
//	int xoff = STEP/2;
//	int yoff = STEP/2;
//	//-------------------------
//	numseeds = xstrips*ystrips;
//	//-------------------------
//	kseedsl.resize(numseeds);
//	kseedsa.resize(numseeds);
//	kseedsb.resize(numseeds);
//	kseedsx.resize(numseeds);
//	kseedsy.resize(numseeds);
//
//	for( int y = 0; y < ystrips; y++ )
//	{
//		int ye = y*yerrperstrip;
//		for( int x = 0; x < xstrips; x++ )
//		{
//			int xe = x*xerrperstrip;
//            int seedx = (x*STEP+xoff+xe);
//            if(hexgrid){ seedx = x*STEP+(xoff<<(y&0x1))+xe; seedx = min(m_width-1,seedx); }//for hex grid sampling
//            int seedy = (y*STEP+yoff+ye);
//            int i = seedy*m_width + seedx;
//			
//			kseedsl[n] = m_lvec[i];
//			kseedsa[n] = m_avec[i];
//			kseedsb[n] = m_bvec[i];
//            kseedsx[n] = seedx;
//            kseedsy[n] = seedy;
//			n++;
//		}
//	}
//
//	
//	if(perturbseeds)
//	{
//		PerturbSeeds(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, edgemag);
//	}
}

//===========================================================================
///	GetLABXYSeeds_ForGivenStepSize
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void SLIC::GetLABXYSeeds_ForGivenStepSize(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
	vector<float>&				kseedsx,
	vector<float>&				kseedsy,
    const int&					STEP,
    const bool&					perturbseeds,
    const vector<float>&       edgemag)
{
    const bool hexgrid = false;
	int numseeds(0);
	int n(0);

	//int xstrips = m_width/STEP;
	//int ystrips = m_height/STEP;
	int xstrips = (0.5+float(m_width)/float(STEP));
	int ystrips = (0.5+float(m_height)/float(STEP));

    int xerr = m_width  - STEP*xstrips;if(xerr < 0){xstrips--;xerr = m_width - STEP*xstrips;}
    int yerr = m_height - STEP*ystrips;if(yerr < 0){ystrips--;yerr = m_height- STEP*ystrips;}

	float xerrperstrip = float(xerr)/float(xstrips);
	float yerrperstrip = float(yerr)/float(ystrips);

	int xoff = STEP/2;
	int yoff = STEP/2;
	//-------------------------
	numseeds = xstrips*ystrips;
	//-------------------------
	kseedsl.resize(numseeds);
	kseedsa.resize(numseeds);
	kseedsb.resize(numseeds);
	kseedsx.resize(numseeds);
	kseedsy.resize(numseeds);

	for( int y = 0; y < ystrips; y++ )
	{
		int ye = y*yerrperstrip;
		for( int x = 0; x < xstrips; x++ )
		{
			int xe = x*xerrperstrip;
            int seedx = (x*STEP+xoff+xe);
            if(hexgrid){ seedx = x*STEP+(xoff<<(y&0x1))+xe; seedx = min(m_width-1,seedx); }//for hex grid sampling
            int seedy = (y*STEP+yoff+ye);
            int i = seedy*m_width + seedx;
			
			kseedsl[n] = m_lvec[i];
			kseedsa[n] = m_avec[i];
			kseedsb[n] = m_bvec[i];
            kseedsx[n] = seedx;
            kseedsy[n] = seedy;
			n++;
		}
	}

	
	if(perturbseeds)
	{
		PerturbSeeds(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, edgemag);
	}
}

//===========================================================================
///	GetLABXYZSeeds_ForGivenStepSize
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void SLIC::GetLABXYZSeeds_ForGivenStepSize(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
    vector<float>&             kseedsox,
    vector<float>&             kseedsoy,
	vector<float>&				kseedsx,
	vector<float>&				kseedsy,
    vector<float>&             kseedsz,
    const int&					STEP,
    const bool&					perturbseeds,
    const vector<float>&       edgemag)
{
    const bool hexgrid = false;
	int numseeds(0);
	int n(0);

	//int xstrips = m_width/STEP;
	//int ystrips = m_height/STEP;
	int xstrips = (0.5+float(m_width)/float(STEP));
	int ystrips = (0.5+float(m_height)/float(STEP));

    int xerr = m_width  - STEP*xstrips;if(xerr < 0){xstrips--;xerr = m_width - STEP*xstrips;}
    int yerr = m_height - STEP*ystrips;if(yerr < 0){ystrips--;yerr = m_height- STEP*ystrips;}

	float xerrperstrip = float(xerr)/float(xstrips);
	float yerrperstrip = float(yerr)/float(ystrips);

	int xoff = STEP/2;
	int yoff = STEP/2;
	//-------------------------
	numseeds = xstrips*ystrips;
	//-------------------------
	kseedsl.resize(numseeds);
	kseedsa.resize(numseeds);
	kseedsb.resize(numseeds);
    kseedsox.resize(numseeds);
	kseedsoy.resize(numseeds);
	kseedsx.resize(numseeds);
	kseedsy.resize(numseeds);
    kseedsz.resize(numseeds);
    
	for( int y = 0; y < ystrips; y++ )
	{
		int ye = y*yerrperstrip;
		for( int x = 0; x < xstrips; x++ )
		{
			int xe = x*xerrperstrip;
            int seedx = (x*STEP+xoff+xe);
            if(hexgrid){ seedx = x*STEP+(xoff<<(y&0x1))+xe; seedx = min(m_width-1,seedx); }//for hex grid sampling
            int seedy = (y*STEP+yoff+ye);
            int i = seedy*m_width + seedx;
			
			kseedsl[n] = m_lvec[i];
			kseedsa[n] = m_avec[i];
			kseedsb[n] = m_bvec[i];
                        kseedsox[n] = seedx;
                        kseedsoy[n] = seedy;
                        kseedsx[n] = m_xvec[i];
                        kseedsy[n] = m_yvec[i];
                        kseedsz[n] = m_zvec[i];
            
                        n++;
		}
	}
    
	if(perturbseeds)
	{
		PerturbSeeds(kseedsl, kseedsa, kseedsb, kseedsox, kseedsoy, kseedsx, kseedsy, kseedsz, edgemag);
	}
}

//===========================================================================
///	GetKValues_LABXYZ
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void SLIC::GetKValues_LABXYZ(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
	vector<float>&				kseedsx,
	vector<float>&				kseedsy,
	vector<float>&				kseedsz,
        const int&				STEP)
{
//    const bool hexgrid = false;
	int numseeds(0);
	int n(0);

	int xstrips = (0.5+float(m_width)/float(STEP));
	int ystrips = (0.5+float(m_height)/float(STEP));
	int zstrips = (0.5+float(m_depth)/float(STEP));

    int xerr = m_width  - STEP*xstrips;if(xerr < 0){xstrips--;xerr = m_width - STEP*xstrips;}
    int yerr = m_height - STEP*ystrips;if(yerr < 0){ystrips--;yerr = m_height- STEP*ystrips;}
    int zerr = m_depth  - STEP*zstrips;if(zerr < 0){zstrips--;zerr = m_depth - STEP*zstrips;}

	float xerrperstrip = float(xerr)/float(xstrips);
	float yerrperstrip = float(yerr)/float(ystrips);
	float zerrperstrip = float(zerr)/float(zstrips);

	int xoff = STEP/2;
	int yoff = STEP/2;
	int zoff = STEP/2;
	//-------------------------
	numseeds = xstrips*ystrips*zstrips;
	//-------------------------
	kseedsl.resize(numseeds);
	kseedsa.resize(numseeds);
	kseedsb.resize(numseeds);
	kseedsx.resize(numseeds);
	kseedsy.resize(numseeds);
	kseedsz.resize(numseeds);

	for( int z = 0; z < zstrips; z++ )
	{
		int ze = z*zerrperstrip;
		int d = (z*STEP+zoff+ze);
		for( int y = 0; y < ystrips; y++ )
		{
			int ye = y*yerrperstrip;
			for( int x = 0; x < xstrips; x++ )
			{
				int xe = x*xerrperstrip;
				int i = (y*STEP+yoff+ye)*m_width + (x*STEP+xoff+xe);
				
				kseedsl[n] = m_lvecvec[d][i];
				kseedsa[n] = m_avecvec[d][i];
				kseedsb[n] = m_bvecvec[d][i];
				kseedsx[n] = (x*STEP+xoff+xe);
				kseedsy[n] = (y*STEP+yoff+ye);
				kseedsz[n] = d;
				n++;
			}
		}
	}
}

//===========================================================================
///	PerformSuperpixelSLIC
///
///	Performs k mean segmentation. It is fast because it looks locally, not
/// over the entire image.
//===========================================================================
void SLIC::PerformSuperpixelSLIC(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
	vector<float>&				kseedsx,
	vector<float>&				kseedsy,
        int*&					klabels,
        const int&				STEP,
        const vector<float>&                   edgemag,
	const float&				M,
        const int                               iterations)
{
	int sz = m_width*m_height;
	const int numk = kseedsl.size();
	//----------------
	int offset = STEP;
        //if(STEP < 8) offset = STEP*1.5;//to prevent a crash due to a very small step size
	//----------------
	
	vector<float> clustersize(numk, 0);
	vector<float> inv(numk, 0);//to store 1/clustersize[k] values

	vector<float> sigmal(numk, 0);
	vector<float> sigmaa(numk, 0);
	vector<float> sigmab(numk, 0);
	vector<float> sigmax(numk, 0);
	vector<float> sigmay(numk, 0);
	vector<float> distvec(sz, DBL_MAX);
        
	float invwt = 1.0/((STEP/M)*(STEP/M));
        
	int x1, y1, x2, y2;
	float l, a, b;
	float dist;
	float distxy;
	for( int itr = 0; itr < iterations; itr++ )
	{
		distvec.assign(sz, DBL_MAX);
		for( int n = 0; n < numk; n++ )
		{
                        y1 = max(0.0f,			kseedsy[n]-offset);
                        y2 = min((float)m_height,	kseedsy[n]+offset);
                        x1 = max(0.0f,			kseedsx[n]-offset);
                        x2 = min((float)m_width,	kseedsx[n]+offset);


			for( int y = y1; y < y2; y++ )
			{
				for( int x = x1; x < x2; x++ )
				{
					int i = y*m_width + x;

					l = m_lvec[i];
					a = m_avec[i];
					b = m_bvec[i];

					dist =			(l - kseedsl[n])*(l - kseedsl[n]) +
								(a - kseedsa[n])*(a - kseedsa[n]) +
								(b - kseedsb[n])*(b - kseedsb[n]);

					distxy =		(x - kseedsx[n])*(x - kseedsx[n]) +
								(y - kseedsy[n])*(y - kseedsy[n]);
					
					//------------------------------------------------------------------------
					dist += distxy*invwt;//dist = sqrt(dist) + sqrt(distxy*invwt);//this is more exact
					//------------------------------------------------------------------------
					if( dist < distvec[i] )
					{
						distvec[i] = dist;
						klabels[i]  = n;
					}
				}
			}
		}
		//-----------------------------------------------------------------
		// Recalculate the centroid and store in the seed values
		//-----------------------------------------------------------------
		//instead of reassigning memory on each iteration, just reset.
	
		sigmal.assign(numk, 0);
		sigmaa.assign(numk, 0);
		sigmab.assign(numk, 0);
		sigmax.assign(numk, 0);
		sigmay.assign(numk, 0);
		clustersize.assign(numk, 0);
		//------------------------------------
		//edgesum.assign(numk, 0);
		//------------------------------------

		{int ind(0);
		for( int r = 0; r < m_height; r++ )
		{
			for( int c = 0; c < m_width; c++ )
			{
				sigmal[klabels[ind]] += m_lvec[c + m_width*r];
				sigmaa[klabels[ind]] += m_avec[c + m_width*r];
				sigmab[klabels[ind]] += m_bvec[c + m_width*r];
				sigmax[klabels[ind]] += c;
				sigmay[klabels[ind]] += r;
				//------------------------------------
				//edgesum[klabels[ind]] += edgemag[ind];
				//------------------------------------
				clustersize[klabels[ind]] += 1.0;
				ind++;
			}
		}}

		{for( int k = 0; k < numk; k++ )
		{
			if( clustersize[k] <= 0 ) clustersize[k] = 1;
			inv[k] = 1.0/clustersize[k];//computing inverse now to multiply, than divide later
		}}
		
		{for( int k = 0; k < numk; k++ )
		{
			kseedsl[k] = sigmal[k]*inv[k];
			kseedsa[k] = sigmaa[k]*inv[k];
			kseedsb[k] = sigmab[k]*inv[k];
			kseedsx[k] = sigmax[k]*inv[k];
			kseedsy[k] = sigmay[k]*inv[k];
			//------------------------------------
			//edgesum[k] *= inv[k];
			//------------------------------------
		}}
	}
}

//===========================================================================
///	Perform3DSupervoxelSLIC
///
///	Performs k mean segmentation. It is fast because it looks locally, not
/// over the entire image.
//===========================================================================
void SLIC::Perform3DSupervoxelSLIC(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
	vector<float>&				kseedsox,
	vector<float>&				kseedsoy,
        vector<float>&				kseedsx,
	vector<float>&				kseedsy,  
        vector<float>&				kseedsz,   
        int*&					klabels,
        const int&				STEP,
        const vector<float>&                   edgemag,
	const float&				M,
        const int                               iterations)
{
	int sz = m_width*m_height;
	const int numk = kseedsl.size();
	//----------------
	int offset = STEP;
        //if(STEP < 8) offset = STEP*1.5;//to prevent a crash due to a very small step size
	//----------------
	
	vector<float> clustersize(numk, 0);
	vector<float> inv(numk, 0);//to store 1/clustersize[k] values

	vector<float> sigmal(numk, 0);
	vector<float> sigmaa(numk, 0);
	vector<float> sigmab(numk, 0);
        vector<float> sigmaox(numk, 0);
	vector<float> sigmaoy(numk, 0);
	vector<float> sigmax(numk, 0);
	vector<float> sigmay(numk, 0);
        vector<float> sigmaz(numk, 0);
	vector<float> distvec(sz, DBL_MAX);

	float invwt = 1.0/((STEP/M)*(STEP/M));
    
	int x1, y1, x2, y2;
	float l, a, b;
	float dist;
//	float distxy;
        float distxyz;
        float widthx, widthy;
	for( int itr = 0; itr < iterations; itr++ )
	{
		distvec.assign(sz, DBL_MAX);
		for( int n = 0; n < numk; n++ )
		{
                        y1 = max(0.0f,			kseedsoy[n]-offset);
                        y2 = min((float)m_height,	kseedsoy[n]+offset);
                        x1 = max(0.0f,			kseedsox[n]-offset);
                        x2 = min((float)m_width,	kseedsox[n]+offset);
                        
//                        widthx = abs(m_xvec[y1*m_width + x1] - m_xvec[y1*m_width + x2 - 1]);
//                        widthy = abs(m_yvec[y1*m_width + x1] - m_yvec[(y2 - 1)*m_width + x1]);
//                        
//                        invwt = 1.0/((widthx/M)*(widthy/M));
                        
			for( int y = y1; y < y2; y++ )
			{
				for( int x = x1; x < x2; x++ )
				{
					int i = y*m_width + x;

					l = m_lvec[i];
					a = m_avec[i];
					b = m_bvec[i];

					dist =	(l - kseedsl[n])*(l - kseedsl[n]) +
						(a - kseedsa[n])*(a - kseedsa[n]) +
						(b - kseedsb[n])*(b - kseedsb[n]);
                    
//                    distxy =        (x - kseedsox[n])*(x - kseedsox[n]) + 
//                                    (y - kseedsoy[n])*(y - kseedsoy[n]) +
//                                    (m_zvec[i] - kseedsx[n])*(m_zvec[i] - kseedsx[n]);
//                    
//                    dist += distxy*invwt;
                    
					distxyz =	(m_xvec[i] - kseedsx[n])*(m_xvec[i] - kseedsx[n]) +
							(m_yvec[i] - kseedsy[n])*(m_yvec[i] - kseedsy[n]) +
                                                        (m_zvec[i] - kseedsz[n])*(m_zvec[i] - kseedsz[n]);
					
					//------------------------------------------------------------------------
					dist += distxyz*invwt;//dist = sqrt(dist) + sqrt(distxy*invwt);//this is more exact
					//------------------------------------------------------------------------
                    
                                        assert(i < sz);
                                        assert(!std::isinf(dist));
                                        
					if( dist < distvec[i])
					{
						distvec[i] = dist;
						klabels[i]  = n;
					}
				}
			}
		}
		//-----------------------------------------------------------------
		// Recalculate the centroid and store in the seed values
		//-----------------------------------------------------------------
		//instead of reassigning memory on each iteration, just reset.
	
		sigmal.assign(numk, 0);
		sigmaa.assign(numk, 0);
		sigmab.assign(numk, 0);
                sigmaox.assign(numk, 0);
		sigmaoy.assign(numk, 0);
		sigmax.assign(numk, 0);
		sigmay.assign(numk, 0);
                sigmaz.assign(numk, 0);
		clustersize.assign(numk, 0);
		//------------------------------------
		//edgesum.assign(numk, 0);
		//------------------------------------

		{int ind(0);
		for( int r = 0; r < m_height; r++ )
		{
			for( int c = 0; c < m_width; c++ )
			{
                                assert(ind < m_width*m_height);
                                assert(klabels[ind] >= 0);
                                assert(klabels[ind] < numk);
                                
                                sigmal[klabels[ind]] += m_lvec[ind];
                                sigmaa[klabels[ind]] += m_avec[ind];
                                sigmab[klabels[ind]] += m_bvec[ind];
                                sigmaox[klabels[ind]] += c;
                                sigmaoy[klabels[ind]] += r;
                                sigmax[klabels[ind]] += m_xvec[ind];
                                sigmay[klabels[ind]] += m_yvec[ind];
                                sigmaz[klabels[ind]] += m_zvec[ind];
                                //------------------------------------
                                //edgesum[klabels[ind]] += edgemag[ind];
                                //------------------------------------
                                clustersize[klabels[ind]] += 1.0;
                                ind++;
			}
		}}

		{for( int k = 0; k < numk; k++ )
		{
			if( clustersize[k] <= 0 ) clustersize[k] = 1;
			inv[k] = 1.0/clustersize[k];//computing inverse now to multiply, than divide later
		}}
		
		{for( int k = 0; k < numk; k++ )
		{
			kseedsl[k] = sigmal[k]*inv[k];
			kseedsa[k] = sigmaa[k]*inv[k];
			kseedsb[k] = sigmab[k]*inv[k];
                        kseedsox[k] = sigmaox[k]*inv[k];
			kseedsoy[k] = sigmaoy[k]*inv[k];
			kseedsx[k] = sigmax[k]*inv[k];
			kseedsy[k] = sigmay[k]*inv[k];
                        kseedsz[k] = sigmaz[k]*inv[k];
			//------------------------------------
			//edgesum[k] *= inv[k];
			//------------------------------------
		}}
	}
}

//===========================================================================
///	PerformSupervoxelSLIC
///
///	Performs k mean segmentation. It is fast because it searches locally, not
/// over the entire image.
//===========================================================================
void SLIC::PerformSupervoxelSLIC(
	vector<float>&				kseedsl,
	vector<float>&				kseedsa,
	vector<float>&				kseedsb,
	vector<float>&				kseedsx,
	vector<float>&				kseedsy,
	vector<float>&				kseedsz,
        int**&					klabels,
        const int&				STEP,
	const float&				compactness)
{
	int sz = m_width*m_height;
	const int numk = kseedsl.size();
        //int numitr(0);

	//----------------
	int offset = STEP;
        //if(STEP < 8) offset = STEP*1.5;//to prevent a crash due to a very small step size
	//----------------

	vector<float> clustersize(numk, 0);
	vector<float> inv(numk, 0);//to store 1/clustersize[k] values

	vector<float> sigmal(numk, 0);
	vector<float> sigmaa(numk, 0);
	vector<float> sigmab(numk, 0);
	vector<float> sigmax(numk, 0);
	vector<float> sigmay(numk, 0);
	vector<float> sigmaz(numk, 0);

	vector< float > initfloat(sz, DBL_MAX);
	vector< vector<float> > distvec(m_depth, initfloat);
	//vector<float> distvec(sz, DBL_MAX);

	float invwt = 1.0/((STEP/compactness)*(STEP/compactness));//compactness = 20.0 is usually good.

	int x1, y1, x2, y2, z1, z2;
	float l, a, b;
	float dist;
	float distxyz;
	for( int itr = 0; itr < 5; itr++ )
	{
		distvec.assign(m_depth, initfloat);
		for( int n = 0; n < numk; n++ )
		{
                        y1 = max(0.0f,			kseedsy[n]-offset);
                        y2 = min((float)m_height,	kseedsy[n]+offset);
                        x1 = max(0.0f,			kseedsx[n]-offset);
                        x2 = min((float)m_width,	kseedsx[n]+offset);
                        z1 = max(0.0f,			kseedsz[n]-offset);
                        z2 = min((float)m_depth,	kseedsz[n]+offset);


			for( int z = z1; z < z2; z++ )
			{
				for( int y = y1; y < y2; y++ )
				{
					for( int x = x1; x < x2; x++ )
					{
						int i = y*m_width + x;

						l = m_lvecvec[z][i];
						a = m_avecvec[z][i];
						b = m_bvecvec[z][i];

						dist =			(l - kseedsl[n])*(l - kseedsl[n]) +
										(a - kseedsa[n])*(a - kseedsa[n]) +
										(b - kseedsb[n])*(b - kseedsb[n]);

						distxyz =		(x - kseedsx[n])*(x - kseedsx[n]) +
										(y - kseedsy[n])*(y - kseedsy[n]) +
										(z - kseedsz[n])*(z - kseedsz[n]);
						//------------------------------------------------------------------------
						dist += distxyz*invwt;
						//------------------------------------------------------------------------
						if( dist < distvec[z][i] )
						{
							distvec[z][i] = dist;
							klabels[z][i]  = n;
						}
					}
				}
			}
		}
		//-----------------------------------------------------------------
		// Recalculate the centroid and store in the seed values
		//-----------------------------------------------------------------
		//instead of reassigning memory on each iteration, just reset.
	
		sigmal.assign(numk, 0);
		sigmaa.assign(numk, 0);
		sigmab.assign(numk, 0);
		sigmax.assign(numk, 0);
		sigmay.assign(numk, 0);
		sigmaz.assign(numk, 0);
		clustersize.assign(numk, 0);

		for( int d = 0; d < m_depth; d++  )
		{
			int ind(0);
			for( int r = 0; r < m_height; r++ )
			{
				for( int c = 0; c < m_width; c++ )
				{
					sigmal[klabels[d][ind]] += m_lvecvec[d][ind];
					sigmaa[klabels[d][ind]] += m_avecvec[d][ind];
					sigmab[klabels[d][ind]] += m_bvecvec[d][ind];
					sigmax[klabels[d][ind]] += c;
					sigmay[klabels[d][ind]] += r;
					sigmaz[klabels[d][ind]] += d;

					clustersize[klabels[d][ind]] += 1.0;
					ind++;
				}
			}
		}

		{for( int k = 0; k < numk; k++ )
		{
			if( clustersize[k] <= 0 ) clustersize[k] = 1;
			inv[k] = 1.0/clustersize[k];//computing inverse now to multiply, than divide later
		}}
		
		{for( int k = 0; k < numk; k++ )
		{
			kseedsl[k] = sigmal[k]*inv[k];
			kseedsa[k] = sigmaa[k]*inv[k];
			kseedsb[k] = sigmab[k]*inv[k];
			kseedsx[k] = sigmax[k]*inv[k];
			kseedsy[k] = sigmay[k]*inv[k];
			kseedsz[k] = sigmaz[k]*inv[k];
		}}
	}
}


//===========================================================================
///	SaveSuperpixelLabels
///
///	Save labels in raster scan order.
//===========================================================================
void SLIC::SaveSuperpixelLabels(
	const int*&					labels,
	const int&					width,
	const int&					height,
	const string&				filename,
	const string&				path) 
{
#ifdef WINDOWS
        char fname[256];
        char extn[256];
        _splitpath(filename.c_str(), NULL, NULL, fname, extn);
        string temp = fname;
        string finalpath = path + temp + string(".dat");
#else
        string nameandextension = filename;
        size_t pos = filename.find_last_of("/");
        if(pos != string::npos)//if a slash is found, then take the filename with extension
        {
            nameandextension = filename.substr(pos+1);
        }
        string newname = nameandextension.replace(nameandextension.rfind(".")+1, 3, "dat");//find the position of the dot and replace the 3 characters following it.
        string finalpath = path+newname;
#endif

        int sz = width*height;
	ofstream outfile;
	outfile.open(finalpath.c_str(), ios::binary);
	for( int i = 0; i < sz; i++ )
	{
		outfile.write((const char*)&labels[i], sizeof(int));
	}
	outfile.close();
}


//===========================================================================
///	SaveSupervoxelLabels
///
///	Save labels in raster scan order.
//===========================================================================
void SLIC::SaveSupervoxelLabels(
	const int**&				labels,
	const int&					width,
	const int&					height,
	const int&					depth,
	const string&				filename,
	const string&				path) 
{
#ifdef WINDOWS
        char fname[256];
        char extn[256];
        _splitpath(filename.c_str(), NULL, NULL, fname, extn);
        string temp = fname;
        string finalpath = path + temp + string(".dat");
#else
        string nameandextension = filename;
        size_t pos = filename.find_last_of("/");
        if(pos != string::npos)//if a slash is found, then take the filename with extension
        {
            nameandextension = filename.substr(pos+1);
        }
        string newname = nameandextension.replace(nameandextension.rfind(".")+1, 3, "dat");//find the position of the dot and replace the 3 characters following it.
        string finalpath = path+newname;
#endif

        int sz = width*height;
	ofstream outfile;
	outfile.open(finalpath.c_str(), ios::binary);
	for( int d = 0; d < depth; d++ )
	{
		for( int i = 0; i < sz; i++ )
		{
			outfile.write((const char*)&labels[d][i], sizeof(int));
		}
	}
	outfile.close();
}

//===========================================================================
///	EnforceLabelConnectivity
///
///		1. finding an adjacent label for each new component at the start
///		2. if a certain component is too small, assigning the previously found
///		    adjacent label to this component, and not incrementing the label.
//===========================================================================
void SLIC::EnforceLabelConnectivity(
	const int*					labels,//input labels that need to be corrected to remove stray labels
	const int					width,
	const int					height,
	int*&						nlabels,//new labels
	int&						numlabels,//the number of labels changes in the end if segments are removed
	const int&					K) //the number of superpixels desired by the user
{
//	const int dx8[8] = {-1, -1,  0,  1, 1, 1, 0, -1};
//	const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1,  1};

	const int dx4[4] = {-1,  0,  1,  0};
	const int dy4[4] = { 0, -1,  0,  1};

	const int sz = width*height;
	const int SUPSZ = sz/K;
	//nlabels.resize(sz, -1);
	for( int i = 0; i < sz; i++ ) nlabels[i] = -1;
	int label(0);
	int* xvec = new int[sz];
	int* yvec = new int[sz];
	int oindex(0);
	int adjlabel(0);//adjacent label
	for( int j = 0; j < height; j++ )
	{
		for( int k = 0; k < width; k++ )
		{
			if( 0 > nlabels[oindex] )
			{
				nlabels[oindex] = label;
				//--------------------
				// Start a new segment
				//--------------------
				xvec[0] = k;
				yvec[0] = j;
				//-------------------------------------------------------
				// Quickly find an adjacent label for use later if needed
				//-------------------------------------------------------
				{for( int n = 0; n < 4; n++ )
				{
					int x = xvec[0] + dx4[n];
					int y = yvec[0] + dy4[n];
					if( (x >= 0 && x < width) && (y >= 0 && y < height) )
					{
						int nindex = y*width + x;
						if(nlabels[nindex] >= 0) adjlabel = nlabels[nindex];
					}
				}}

				int count(1);
				for( int c = 0; c < count; c++ )
				{
					for( int n = 0; n < 4; n++ )
					{
						int x = xvec[c] + dx4[n];
						int y = yvec[c] + dy4[n];

						if( (x >= 0 && x < width) && (y >= 0 && y < height) )
						{
							int nindex = y*width + x;

							if( 0 > nlabels[nindex] && labels[oindex] == labels[nindex] )
							{
								xvec[count] = x;
								yvec[count] = y;
								nlabels[nindex] = label;
								count++;
							}
						}

					}
				}
				//-------------------------------------------------------
				// If segment size is less then a limit, assign an
				// adjacent label found before, and decrement label count.
				//-------------------------------------------------------
				if(count <= SUPSZ >> 2)
				{
					for( int c = 0; c < count; c++ )
					{
						int ind = yvec[c]*width+xvec[c];
						nlabels[ind] = adjlabel;
					}
					label--;
				}
				label++;
			}
			oindex++;
		}
	}
	numlabels = label;

	if(xvec) delete [] xvec;
	if(yvec) delete [] yvec;
}


//===========================================================================
///	RelabelStraySupervoxels
//===========================================================================
void SLIC::EnforceSupervoxelLabelConnectivity(
	int**&						labels,//input - previous labels, output - new labels
	const int&					width,
	const int&					height,
	const int&					depth,
	int&						numlabels,
	const int&					STEP)
{
	const int dx10[10] = {-1,  0,  1,  0, -1,  1,  1, -1,  0, 0};
	const int dy10[10] = { 0, -1,  0,  1, -1, -1,  1,  1,  0, 0};
	const int dz10[10] = { 0,  0,  0,  0,  0,  0,  0,  0, -1, 1};

	int sz = width*height;
	const int SUPSZ = STEP*STEP*STEP;

	int adjlabel(0);//adjacent label
        int* xvec = new int[SUPSZ*10];//a large enough size
        int* yvec = new int[SUPSZ*10];//a large enough size
        int* zvec = new int[SUPSZ*10];//a large enough size
	//------------------
	// memory allocation
	//------------------
	int** nlabels = new int*[depth];
	{for( int d = 0; d < depth; d++ )
	{
		nlabels[d] = new int[sz];
		for( int i = 0; i < sz; i++ ) nlabels[d][i] = -1;
	}}
	//------------------
	// labeling
	//------------------
	int lab(0);
	{for( int d = 0; d < depth; d++ )
	{
		int i(0);
		for( int h = 0; h < height; h++ )
		{
			for( int w = 0; w < width; w++ )
			{
				if(nlabels[d][i] < 0)
				{
					nlabels[d][i] = lab;
					//-------------------------------------------------------
					// Quickly find an adjacent label for use later if needed
					//-------------------------------------------------------
					{for( int n = 0; n < 10; n++ )
					{
						int x = w + dx10[n];
						int y = h + dy10[n];
						int z = d + dz10[n];
						if( (x >= 0 && x < width) && (y >= 0 && y < height) && (z >= 0 && z < depth) )
						{
							int nindex = y*width + x;
							if(nlabels[z][nindex] >= 0)
							{
								adjlabel = nlabels[z][nindex];
							}
						}
					}}
					
					xvec[0] = w; yvec[0] = h; zvec[0] = d;
					int count(1);
					for( int c = 0; c < count; c++ )
					{
						for( int n = 0; n < 10; n++ )
						{
							int x = xvec[c] + dx10[n];
							int y = yvec[c] + dy10[n];
							int z = zvec[c] + dz10[n];

							if( (x >= 0 && x < width) && (y >= 0 && y < height) && (z >= 0 && z < depth))
							{
								int nindex = y*width + x;

								if( 0 > nlabels[z][nindex] && labels[d][i] == labels[z][nindex] )
								{
									xvec[count] = x;
									yvec[count] = y;
									zvec[count] = z;
									nlabels[z][nindex] = lab;
									count++;
								}
							}

						}
					}
					//-------------------------------------------------------
					// If segment size is less then a limit, assign an
					// adjacent label found before, and decrement label count.
					//-------------------------------------------------------
					if(count <= (SUPSZ >> 2))//this threshold can be changed according to needs
					{
						for( int c = 0; c < count; c++ )
						{
							int ind = yvec[c]*width+xvec[c];
							nlabels[zvec[c]][ind] = adjlabel;
						}
						lab--;
					}
					//--------------------------------------------------------
					lab++;
				}
				i++;
			}
		}
	}}
	//------------------
	// mem de-allocation
	//------------------
	{for( int d = 0; d < depth; d++ )
	{
		for( int i = 0; i < sz; i++ ) labels[d][i] = nlabels[d][i];
	}}
	{for( int d = 0; d < depth; d++ )
	{
		delete [] nlabels[d];
	}}
	delete [] nlabels;
	//------------------
	if(xvec) delete [] xvec;
	if(yvec) delete [] yvec;
	if(zvec) delete [] zvec;
	//------------------
	numlabels = lab;
	//------------------
}

//===========================================================================
///	DoSuperpixelSegmentation_ForGivenSuperpixelSize
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void SLIC::DoDepthSuperpixelSegmentation_ForGivenSeedProbabilities(
        const unsigned int*                             ubuff,
        const unsigned short*                           depth,
	const int					width,
	const int					height,
	int*&						klabels,
	int&						numlabels,
        const float                                     variance,
        const int                                       superpixels,
        const float&                                   compactness,
        const bool&                                     perturbseeds,
        const int                                       iterations,
        const int                                       color)
{
    //--------------------------------------------------
    const int superpixelsize = 0.5+float(width*height)/float(superpixels);
    //--------------------------------------------------
    
    //------------------------------------------------
    const int STEP = sqrt(float(superpixelsize))+0.5;
    //------------------------------------------------
        vector<float> kseedsl(0);
	vector<float> kseedsa(0);
	vector<float> kseedsb(0);
	vector<float> kseedsx(0);
	vector<float> kseedsy(0);

	//--------------------------------------------------
	m_width  = width;
	m_height = height;
	int sz = m_width*m_height;
	//klabels.resize( sz, -1 );
	//--------------------------------------------------
	klabels = new int[sz];
	for( int s = 0; s < sz; s++ ) klabels[s] = -1;
    //--------------------------------------------------
    if(color > 0)//LAB, the default option
    {
        DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);
    }
    else//RGB
    {
        m_lvec = new float[sz]; m_avec = new float[sz]; m_bvec = new float[sz];
        for( int i = 0; i < sz; i++ )
        {
                m_lvec[i] = ubuff[i] >> 16 & 0xff;
                m_avec[i] = ubuff[i] >>  8 & 0xff;
                m_bvec[i] = ubuff[i]       & 0xff;
        }
    }
	//--------------------------------------------------
	vector<float> edgemag(0);
	if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
	GetLABXYSeeds_ForGivenSeedProbabilities(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, variance, depth, superpixels, perturbseeds, edgemag);

	PerformSuperpixelSLIC(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, edgemag, compactness, iterations);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, float(sz)/float(STEP*STEP));
	{for(int i = 0; i < sz; i++ ) klabels[i] = nlabels[i];}
	if(nlabels) delete [] nlabels;
}

//===========================================================================
///	DoSuperpixelSegmentation_ForGivenSuperpixelSize
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void SLIC::DoSuperpixelSegmentation_ForGivenSuperpixelSize(
        const unsigned int*                             ubuff,
	const int					width,
	const int					height,
	int*&						klabels,
	int&						numlabels,
        const int&					superpixelsize,
        const float&                                   compactness,
        const bool&                                     perturbseeds,
        const int                                       iterations,
        const int                                       color)
{
    //------------------------------------------------
    const int STEP = sqrt(float(superpixelsize))+0.5;
    //------------------------------------------------
	vector<float> kseedsl(0);
	vector<float> kseedsa(0);
	vector<float> kseedsb(0);
	vector<float> kseedsx(0);
	vector<float> kseedsy(0);

	//--------------------------------------------------
	m_width  = width;
	m_height = height;
	int sz = m_width*m_height;
	//klabels.resize( sz, -1 );
	//--------------------------------------------------
	klabels = new int[sz];
	for( int s = 0; s < sz; s++ ) klabels[s] = -1;
    //--------------------------------------------------
    if(color > 0)//LAB, the default option
    {
        DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);
    }
    else//RGB
    {
        m_lvec = new float[sz]; m_avec = new float[sz]; m_bvec = new float[sz];
        for( int i = 0; i < sz; i++ )
        {
                m_lvec[i] = ubuff[i] >> 16 & 0xff;
                m_avec[i] = ubuff[i] >>  8 & 0xff;
                m_bvec[i] = ubuff[i]       & 0xff;
        }
    }
	//--------------------------------------------------
	vector<float> edgemag(0);
	if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
	GetLABXYSeeds_ForGivenStepSize(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, STEP, perturbseeds, edgemag);

	PerformSuperpixelSLIC(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, edgemag, compactness, iterations);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, float(sz)/float(STEP*STEP));
	{for(int i = 0; i < sz; i++ ) klabels[i] = nlabels[i];}
	if(nlabels) delete [] nlabels;
}

//===========================================================================
///	DoSuperpixelSegmentation_ForGivenSuperpixelSize
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void SLIC::DoSuperpixelSegmentation_ForGivenSuperpixelStep(
        const unsigned int*                             ubuff,
	const int					width,
	const int					height,
	int*&						klabels,
	int&						numlabels,
        const int&					superpixelstep,
        const float&                                   compactness,
        const bool&                                     perturbseeds,
        const int                                       iterations,
        const int                                       color)
{
    //------------------------------------------------
    const int STEP = superpixelstep;
    //------------------------------------------------
	vector<float> kseedsl(0);
	vector<float> kseedsa(0);
	vector<float> kseedsb(0);
	vector<float> kseedsx(0);
	vector<float> kseedsy(0);

	//--------------------------------------------------
	m_width  = width;
	m_height = height;
	int sz = m_width*m_height;
	//klabels.resize( sz, -1 );
	//--------------------------------------------------
	klabels = new int[sz];
	for( int s = 0; s < sz; s++ ) klabels[s] = -1;
    //--------------------------------------------------
    if(color > 0)//LAB, the default option
    {
        DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);
    }
    else//RGB
    {
        m_lvec = new float[sz]; m_avec = new float[sz]; m_bvec = new float[sz];
        for( int i = 0; i < sz; i++ )
        {
                m_lvec[i] = ubuff[i] >> 16 & 0xff;
                m_avec[i] = ubuff[i] >>  8 & 0xff;
                m_bvec[i] = ubuff[i]       & 0xff;
        }
    }
	//--------------------------------------------------
	vector<float> edgemag(0);
	if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
	GetLABXYSeeds_ForGivenStepSize(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, STEP, perturbseeds, edgemag);

	PerformSuperpixelSLIC(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, klabels, STEP, edgemag, compactness, iterations);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, float(sz)/float(STEP*STEP));
	{for(int i = 0; i < sz; i++ ) klabels[i] = nlabels[i];}
	if(nlabels) delete [] nlabels;
}

//===========================================================================
///	Do3DSupervixelSegmentation_ForGivenSuperpixelSize
///
/// This is the extension of SLIC to 3D supervoxels. In addition to the color
/// image, the input includes the x, y and z coordinates in the world
/// coordinate system.
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void SLIC::Do3DSupervoxelSegmentation_ForGivenSupervoxelSize(
        const unsigned int*                             ubuff,
        const float*                                   x,
        const float*                                   y,
        const float*                                   z,
	const int					width,
	const int					height,
	int*&						klabels,
	int&						numlabels,
        const int&					superpixelsize,
        const float&                                   compactness,
        const bool&                                     perturbseeds,
        const int                                       iterations,
        const int                                       color)
{
    //------------------------------------------------
    const int STEP = sqrt(float(superpixelsize))+0.5;
    //------------------------------------------------
	vector<float> kseedsl(0);
	vector<float> kseedsa(0);
	vector<float> kseedsb(0);
    vector<float> kseedsox(0);
	vector<float> kseedsoy(0);
	vector<float> kseedsx(0);
	vector<float> kseedsy(0);
    vector<float> kseedsz(0);
    
	//--------------------------------------------------
	m_width  = width;
	m_height = height;
	int sz = m_width*m_height;
	//klabels.resize( sz, -1 );
	//--------------------------------------------------
	klabels = new int[sz];
	for( int s = 0; s < sz; s++ ) klabels[s] = -1;
    //--------------------------------------------------
    if(color > 0)//LAB, the default option
    {
        DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);
    }
    else//RGB
    {
        m_lvec = new float[sz]; m_avec = new float[sz]; m_bvec = new float[sz];
        for( int i = 0; i < sz; i++ )
        {
                m_lvec[i] = ubuff[i] >> 16 & 0xff;
                m_avec[i] = ubuff[i] >>  8 & 0xff;
                m_bvec[i] = ubuff[i]       & 0xff;
        }
    }
    
    m_xvec = new float[sz];
    m_yvec = new float[sz];
    m_zvec = new float[sz];
    for( int i = 0; i < sz; i++ )
    {
        m_xvec[i] = x[i];
        m_yvec[i] = y[i];
        m_zvec[i] = z[i];
    }
    
	//--------------------------------------------------
	vector<float> edgemag(0);
	if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
	GetLABXYZSeeds_ForGivenStepSize(kseedsl, kseedsa, kseedsb, kseedsox, kseedsoy, kseedsx, kseedsy, kseedsz, STEP, perturbseeds, edgemag);

	Perform3DSupervoxelSLIC(kseedsl, kseedsa, kseedsb, kseedsox, kseedsoy, kseedsx, kseedsy, kseedsz, klabels, STEP, edgemag, compactness, iterations);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, float(sz)/float(STEP*STEP));
	{for(int i = 0; i < sz; i++ ) klabels[i] = nlabels[i];}
	if(nlabels) delete [] nlabels;
}

//===========================================================================
///	Do3DSupervixelSegmentation_ForGivenSuperpixelSize
///
/// This is the extension of SLIC to 3D supervoxels. In addition to the color
/// image, the input includes the x, y and z coordinates in the world
/// coordinate system.
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void SLIC::Do3DSupervoxelSegmentation_ForGivenSupervoxelStep(
        const unsigned int*                             ubuff,
        const float*                                   x,
        const float*                                   y,
        const float*                                   z,
	const int					width,
	const int					height,
	int*&						klabels,
	int&						numlabels,
        const int&					superpixelstep,
        const float&                                   compactness,
        const bool&                                     perturbseeds,
        const int                                       iterations,
        const int                                       color)
{
    //------------------------------------------------
    const int STEP = superpixelstep;
    //------------------------------------------------
	vector<float> kseedsl(0);
	vector<float> kseedsa(0);
	vector<float> kseedsb(0);
    vector<float> kseedsox(0);
	vector<float> kseedsoy(0);
	vector<float> kseedsx(0);
	vector<float> kseedsy(0);
    vector<float> kseedsz(0);
    
	//--------------------------------------------------
	m_width  = width;
	m_height = height;
	int sz = m_width*m_height;
	//klabels.resize( sz, -1 );
	//--------------------------------------------------
	klabels = new int[sz];
	for( int s = 0; s < sz; s++ ) klabels[s] = -1;
    //--------------------------------------------------
    if(color > 0)//LAB, the default option
    {
        DoRGBtoLABConversion(ubuff, m_lvec, m_avec, m_bvec);
    }
    else//RGB
    {
        m_lvec = new float[sz]; m_avec = new float[sz]; m_bvec = new float[sz];
        for( int i = 0; i < sz; i++ )
        {
                m_lvec[i] = ubuff[i] >> 16 & 0xff;
                m_avec[i] = ubuff[i] >>  8 & 0xff;
                m_bvec[i] = ubuff[i]       & 0xff;
        }
    }
    
    m_xvec = new float[sz];
    m_yvec = new float[sz];
    m_zvec = new float[sz];
    for( int i = 0; i < sz; i++ )
    {
        m_xvec[i] = x[i];
        m_yvec[i] = y[i];
        m_zvec[i] = z[i];
    }
    
	//--------------------------------------------------
	vector<float> edgemag(0);
	if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
	GetLABXYZSeeds_ForGivenStepSize(kseedsl, kseedsa, kseedsb, kseedsox, kseedsoy, kseedsx, kseedsy, kseedsz, STEP, perturbseeds, edgemag);

	Perform3DSupervoxelSLIC(kseedsl, kseedsa, kseedsb, kseedsox, kseedsoy, kseedsx, kseedsy, kseedsz, klabels, STEP, edgemag, compactness, iterations);
	numlabels = kseedsl.size();

	int* nlabels = new int[sz];
	EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, float(sz)/float(STEP*STEP));
	{for(int i = 0; i < sz; i++ ) klabels[i] = nlabels[i];}
	if(nlabels) delete [] nlabels;
}

//===========================================================================
///	DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void SLIC::DoSuperpixelSegmentation_ForGivenNumberOfSuperpixels(
        const unsigned int*                             ubuff,
	const int					width,
	const int					height,
	int*&						klabels,
	int&						numlabels,
	const int&					K,//required number of superpixels
        const float&                                   compactness,
        const bool&                                     perturbseeds,
        const int                                       iterations,
        const int                                       color)
{
    const int superpixelsize = 0.5+float(width*height)/float(K);
    DoSuperpixelSegmentation_ForGivenSuperpixelSize(ubuff,width,height,klabels,numlabels,superpixelsize,compactness,perturbseeds,iterations,color);
}

//===========================================================================
///	Do3DSupervoxelSegmentation_ForGivenNumberOfSupervoxels
///
/// This is the extension of SLIC to 3D supervoxels. In addition to the color
/// image, the input includes the x, y and z coordinates in the world
/// coordinate system.
///
/// The input parameter ubuff conains RGB values in a 32-bit unsigned integers
/// as follows:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// superpixels more compact while a smaller value would make them more uneven.
///
/// The labels can be saved if needed using SaveSuperpixelLabels()
//===========================================================================
void SLIC::Do3DSupervoxelSegmentation_ForGivenNumberOfSupervoxels(
        const unsigned int*                             ubuff,
        const float*                                   x,
        const float*                                   y,
        const float*                                   z,
	const int					width,
	const int					height,
	int*&						klabels,
	int&						numlabels,
	const int&					K,//required number of superpixels
        const float&                                   compactness,
        const bool&                                     perturbseeds,
        const int                                       iterations,
        const int                                       color)
{
    const int superpixelsize = 0.5+float(width*height)/float(K);
    Do3DSupervoxelSegmentation_ForGivenSupervoxelSize(ubuff,x,y,z,width,height,klabels,numlabels,superpixelsize,compactness,perturbseeds,iterations,color);
}

//===========================================================================
///	DoSupervoxelSegmentation
///
/// There is option to save the labels if needed.
///
/// The input parameter ubuffvec holds all the video frames. It is a
/// 2-dimensional array. The first dimension is depth and the second dimension
/// is pixel location in a frame. For example, to access a pixel in the 3rd
/// frame (i.e. depth index 2), in the 4th row (i.e. height index 3) on the
/// 37th column (i.e. width index 36), you would write:
///
/// unsigned int the_pixel_i_want = ubuffvec[2][3*width + 36]
///
/// In addition, here is how the RGB values are contained in a 32-bit unsigned
/// integer:
///
/// [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]  [1 1 1 1 1 1 1 1]
///
///        Nothing              R                 G                  B
///
/// The RGB values are accessed from (and packed into) the unsigned integers
/// using bitwise operators as can be seen in the function DoRGBtoLABConversion().
///
/// compactness value depends on the input pixels values. For instance, if
/// the input is greyscale with values ranging from 0-100, then a compactness
/// value of 20.0 would give good results. A greater value will make the
/// supervoxels more compact while a smaller value would make them more uneven.
//===========================================================================
void SLIC::DoSupervoxelSegmentation(
	unsigned int**&				ubuffvec,
	const int&					width,
	const int&					height,
	const int&					depth,
	int**&						klabels,
	int&						numlabels,
    const int&					supervoxelsize,
    const float&               compactness)
{
    //---------------------------------------------------------
    const int STEP = 0.5 + pow(float(supervoxelsize),1.0/3.0);
    //---------------------------------------------------------
	vector<float> kseedsl(0);
	vector<float> kseedsa(0);
	vector<float> kseedsb(0);
	vector<float> kseedsx(0);
	vector<float> kseedsy(0);
	vector<float> kseedsz(0);

	//--------------------------------------------------
	m_width  = width;
	m_height = height;
	m_depth  = depth;
	int sz = m_width*m_height;
	
	//--------------------------------------------------
        //klabels = new int*[depth];
	m_lvecvec = new float*[depth];
	m_avecvec = new float*[depth];
	m_bvecvec = new float*[depth];
	for( int d = 0; d < depth; d++ )
	{
                //klabels[d] = new int[sz];
		m_lvecvec[d] = new float[sz];
		m_avecvec[d] = new float[sz];
		m_bvecvec[d] = new float[sz];
		for( int s = 0; s < sz; s++ )
		{
			klabels[d][s] = -1;
		}
	}
	
	DoRGBtoLABConversion(ubuffvec, m_lvecvec, m_avecvec, m_bvecvec);

	GetKValues_LABXYZ(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, kseedsz, STEP);

	PerformSupervoxelSLIC(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, kseedsz, klabels, STEP, compactness);

	EnforceSupervoxelLabelConnectivity(klabels, width, height, depth, numlabels, STEP);
}

