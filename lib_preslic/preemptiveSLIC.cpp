/*
 * PreemptiveSLIC
 * Copyright (C) 2014  Peer Neubert, peer.neubert@etit.tu-chemnitz.de
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * 
 * ====================================================================== 
 * 
 * This code is based on the original SLIC code by Radhakrishna Achanta [EPFL].
 * He kindly permits the publication of this code under GPL. This is the preamble
 * of his orginal code
 * 
 * // SLIC.cpp: implementation of the SLIC class.
 * //
 * // Copyright (C) Radhakrishna Achanta 2012
 * // All rights reserved
 * // Email: firstname.lastname@epfl.ch
 * 
 * ====================================================================== 
 * 
 * 
 * 
 */

#include <cfloat>
#include <cmath>
#include <iostream>
#include <fstream>
#include "preemptiveSLIC.h"


using namespace std;
using namespace cv;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PreemptiveSLIC::PreemptiveSLIC()
{
	m_lvec = NULL;
	m_avec = NULL;
	m_bvec = NULL;
}

PreemptiveSLIC::~PreemptiveSLIC()
{
	if(m_lvec) delete [] m_lvec;
	if(m_avec) delete [] m_avec;
	if(m_bvec) delete [] m_bvec;
}




//==============================================================================
///	DetectLabEdges
//==============================================================================
void PreemptiveSLIC::DetectLabEdges(
	const double*				lvec,
	const double*				avec,
	const double*				bvec,
	const int&					width,
	const int&					height,
	vector<double>&				edges)
{
	int sz = width*height;

	edges.resize(sz,0);
	for( int j = 1; j < height-1; j++ )
	{
		for( int k = 1; k < width-1; k++ )
		{
			int i = j*width+k;

			double dx = (lvec[i-1]-lvec[i+1])*(lvec[i-1]-lvec[i+1]) +
						(avec[i-1]-avec[i+1])*(avec[i-1]-avec[i+1]) +
						(bvec[i-1]-bvec[i+1])*(bvec[i-1]-bvec[i+1]);

			double dy = (lvec[i-width]-lvec[i+width])*(lvec[i-width]-lvec[i+width]) +
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
void PreemptiveSLIC::PerturbSeeds(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
        const vector<double>&                   edges)
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
///	GetLABXYSeeds_ForGivenStepSize
///
/// The k seed values are taken as uniform spatial pixel samples.
//===========================================================================
void PreemptiveSLIC::GetLABXYSeeds_ForGivenStepSize(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
    const int&					STEP,
    const bool&					perturbseeds,
    const vector<double>&       edgemag)
{
    const bool hexgrid = false;
	int numseeds(0);
	int n(0);

	int xstrips = (0.5+double(m_width)/double(STEP));
	int ystrips = (0.5+double(m_height)/double(STEP));

    int xerr = m_width  - STEP*xstrips;if(xerr < 0){xstrips--;xerr = m_width - STEP*xstrips;}
    int yerr = m_height - STEP*ystrips;if(yerr < 0){ystrips--;yerr = m_height- STEP*ystrips;}

	double xerrperstrip = double(xerr)/double(xstrips);
	double yerrperstrip = double(yerr)/double(ystrips);

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
	
    m_w_seed = xstrips;
    m_h_seed = ystrips;
    
	if(perturbseeds)
	{
		PerturbSeeds(kseedsl, kseedsa, kseedsb, kseedsx, kseedsy, edgemag);
	}
}


//===========================================================================
///	PerformSuperpixelSLIC
///
///	Performs k mean segmentation. It is fast because it looks locally, not
/// over the entire image.
//===========================================================================
void PreemptiveSLIC::PerformSuperpixelSLIC(
	vector<double>&				kseedsl,
	vector<double>&				kseedsa,
	vector<double>&				kseedsb,
	vector<double>&				kseedsx,
	vector<double>&				kseedsy,
        int*&					klabels,
        const int&				STEP,
        const vector<double>&                   edgemag,
	const double&				M)
{
	int sz = m_width*m_height;
	const int numk = kseedsl.size();
	//----------------
	int offset = STEP;
        //if(STEP < 8) offset = STEP*1.5;//to prevent a crash due to a very small step size
	//----------------
	
	vector<double> clustersize(numk, 0);
	vector<double> inv(numk, 0);//to store 1/clustersize[k] values

	vector<double> sigmal(numk, 0);
	vector<double> sigmaa(numk, 0);
	vector<double> sigmab(numk, 0);
	vector<double> sigmax(numk, 0);
	vector<double> sigmay(numk, 0);
	vector<double> distvec(sz, DBL_MAX);
    vector<unsigned int> changes(numk, 0);

	double invwt = 1.0/((STEP/M)*(STEP/M));

    
	int x1, y1, x2, y2;
	double l, a, b;
	double dist;
	double distxy;
    double dl, da, db, dx, dy;
    unsigned int callCount=0;
    
    // main iteration loop 
	for( int itr = 0; itr < 10; itr++ )
	{
		distvec.assign(sz, DBL_MAX);
        
        // iterate over each cluster
		for( int n = 0; n < numk; n++ )
		{
            y1 = max(0.0,			kseedsy[n]-offset);
            y2 = min((double)m_height,	kseedsy[n]+offset);
            x1 = max(0.0,			kseedsx[n]-offset);
            x2 = min((double)m_width,	kseedsx[n]+offset);

            // iterate over each pixel in the cluster area and update pixel label and distance
			for( int y = y1; y < y2; y++ )
			{
				for( int x = x1; x < x2; x++ )
				{
                    callCount++;
					int i = y*m_width + x;
                    
                    dl = m_lvec[i]-kseedsl[n];
                    da = m_avec[i]-kseedsa[n];
                    db = m_bvec[i]-kseedsb[n];
                    dx = x-kseedsx[n];
                    dy = y-kseedsy[n];
                                        
					dist = dl*dl + da*da + db*db;
                    distxy = dx*dx + dy*dy;
					
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

        // for each pixel: update data of assigned cluster
		int ind(0);
		for( int r = 0; r < m_height; r++ )
		{
			for( int c = 0; c < m_width; c++ )
			{
       if(klabels[ind]>=0 && klabels[ind]<numk)
       {
          sigmal[klabels[ind]] += m_lvec[ind];
          sigmaa[klabels[ind]] += m_avec[ind];
          sigmab[klabels[ind]] += m_bvec[ind];
          sigmax[klabels[ind]] += c;
          sigmay[klabels[ind]] += r;
          //------------------------------------
          //edgesum[klabels[ind]] += edgemag[ind];
          //------------------------------------
          clustersize[klabels[ind]] += 1.0;
       }
				ind++;
			}
		}

        
        // iterate over cluster and compute inverse of size
		for( int k = 0; k < numk; k++ )
		{
			if( clustersize[k] <= 0 ) clustersize[k] = 1;
			inv[k] = 1.0/clustersize[k];//computing inverse now to multiply, than divide later
		}		
        
        
        // iterate over cluster and update
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
/// PerformSuperpixelSLIC
///
/// Performs k mean segmentation. It is fast because it looks locally, not
/// over the entire image.
//===========================================================================
void PreemptiveSLIC::PerformSuperpixelSLIC_preemptive(
    vector<double>&             kseedsl,
    vector<double>&             kseedsa,
    vector<double>&             kseedsb,
    vector<double>&             kseedsx,
    vector<double>&             kseedsy,
        int*&                   klabels,
    const double&               M,
    const int&                  maxIter)
{
 
    double preemptive_thresh_iteration = 0.01;
    double preemptive_thresh_segment = 0.01;
    double minChanges = preemptive_thresh_segment*9*m_sx*m_sy; // m_sx*_m_sy is cluster size, 9 clusters are in the neighborhoud
    
    int sz = m_width*m_height;
    const int numk = kseedsl.size();
    
    vector<double> clustersize(numk, 0);
    vector<double> inv(numk, 0);//to store 1/clustersize[k] values

    vector<double> sigmal(numk, 0);
    vector<double> sigmaa(numk, 0);
    vector<double> sigmab(numk, 0);
    vector<double> sigmax(numk, 0);
    vector<double> sigmay(numk, 0);
    vector<double> distvec(sz, DBL_MAX);
    vector<unsigned int> changes(numk, 0);
    
    double invwt = 1.0/((m_sx/M)*(m_sy/M));
    
    int x1, y1, x2, y2;
    double l, a, b;
    double dist;
    double distxy;
    double dl, da, db, dx, dy;
    unsigned int callCount=0;
    
    std::vector<int> klabels_new(sz, 1);
    
    // assign each pixel to the nearest cluster
    for( int s = 0; s < sz; s++ )
    {
      klabels[s] = 0;
      klabels_new[s] = 1;
    }
    
    vector<unsigned int> nChangesVec(sz, sz);
    
    int nChanges = sz;
    int nSkippedClusters;

    // main iteration loop 
    vector<double> dist_n((2*m_sx+1)*(2*m_sy+1));
    for( int itr = 0; itr < maxIter; itr++ )
    {      
      
        if(nChanges<preemptive_thresh_iteration*sz)
          break;

        nChanges=0;
        nSkippedClusters = 0;
//         distvec.assign(sz, DBL_MAX); // TODO CHANGED: uncommented
                
        // for each cluster
        for( int n = 0; n < numk; n++ )
        {
          // skip if there are too few changes in the area of this cluster
          if(nChangesVec[n]<minChanges)
          {
            nSkippedClusters++;
            continue;
          }
          nChangesVec[n]=0;
          
          
          y1 = max(0.0,           kseedsy[n]-m_sy);
          y2 = min((double)m_height,  kseedsy[n]+m_sy);
          x1 = max(0.0,           kseedsx[n]-m_sx);
          x2 = min((double)m_width,   kseedsx[n]+m_sx);

          // for each pixel in the area of the cluster, update label and distance
          int i=0;          
          
          double kseedsl_n = kseedsl[n];
          double kseedsa_n = kseedsa[n];
          double kseedsb_n = kseedsb[n];
          
          double kseedsy_n = kseedsy[n];
          double kseedsx_n = kseedsx[n];          
          
          int dist_n_idx=0;
          
          // compute distance for each near pixel
          for( int y = y1; y < y2; y++ )
          {
            i = y*m_width + x1;
            
            dy = y-kseedsy_n;
            
            for( int x = x1; x < x2; x++ )
            {
                callCount++;                
                
                dl = m_lvec[i]-kseedsl_n;
                da = m_avec[i]-kseedsa_n;
                db = m_bvec[i]-kseedsb_n;
                dx = x-kseedsx_n;
                                    
                dist = dl*dl + da*da + db*db;
                distxy = dx*dx + dy*dy;
                
                //------------------------------------------------------------------------
                dist += distxy*invwt;//dist = sqrt(dist) + sqrt(distxy*invwt);//this is more exact
                
                dist_n[dist_n_idx] = dist;
                dist_n_idx++;
                //------------------------------------------------------------------------
                i++;
            }
          }
          
          // check for each near pixel, if distance is smaller than the prior best
          dist_n_idx=0;
          for( int y = y1; y < y2; y++ )
          {
            i = y*m_width + x1;
            for( int x = x1; x < x2; x++ )
            {
              if( dist_n[dist_n_idx] < distvec[i] ) 
              {
                distvec[i] = dist_n[dist_n_idx];
                klabels_new[i]  = n;
              }
              dist_n_idx++;
              i++;
            }
          }
        }  // for each cluster
        
 


        //-----------------------------------------------------------------
        // Collect number of changes per cluster
        //-----------------------------------------------------------------
        int ind(0);

        // for each pixel
        int x_seed, y_seed;
        for( int r = 0; r < m_height; r++ )
        {
            for( int c = 0; c < m_width; c++ )
            {
              
              // if the label has changed
              if(klabels_new[ind]!=klabels[ind])
              {
                
                  // update label
                  klabels[ind] = klabels_new[ind];
                  
                  // increase change counter
                  nChanges++;
  
                  nChangesVec[ klabels[ind] ]++;
                  
                  // TODO This will potentially not work for non grid like seeds
                  // Dirty fix for allowing integer valued m_sx and m_sy
                  // David Stutz <david.stutz@rwth-aachen.de>
                  x_seed = int(c-m_pixel_offset)/int(m_sx + 1);
                  y_seed = int(r-m_pixel_offset)/int(m_sy + 1);
                  
                  // center
                  nChangesVec[ y_seed*m_nx + x_seed ]++; // center
                    
                  // 4 neighborhoud
                  if(x_seed>0)          nChangesVec[ y_seed*m_nx + x_seed - 1]++; // left
                  if(x_seed<m_nx-1)     nChangesVec[ y_seed*m_nx + x_seed + 1]++; // right
                  if(y_seed>0)          nChangesVec[ (y_seed-1)*m_nx + x_seed]++; // top
                  if(y_seed<m_ny-1)     nChangesVec[ (y_seed+1)*m_nx + x_seed]++; // bottom
                    
                  // rest of 8 neighborhoud
                  if(x_seed<m_nx-1 &&  y_seed>0)       nChangesVec[ (y_seed-1)*m_nx + x_seed + 1]++; // 
                  if(x_seed<m_nx-1 &&  y_seed<m_ny-1)  nChangesVec[ (y_seed+1)*m_nx + x_seed + 1]++; // 
                  if(x_seed>0 && y_seed>0)             nChangesVec[ (y_seed-1)*m_nx + x_seed - 1]++; // 
                  if(x_seed>0 && y_seed<m_ny-1)        nChangesVec[ (y_seed+1)*m_nx + x_seed - 1]++; // 
                    
              }
              ind++;
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
        
        ind=0;
        for( int r = 0; r < m_height; r++ )
        {
          for( int c = 0; c < m_width; c++ )
          {
              sigmal[klabels[ind]] += m_lvec[ind];
              sigmaa[klabels[ind]] += m_avec[ind];
              sigmab[klabels[ind]] += m_bvec[ind];
              sigmax[klabels[ind]] += c;
              sigmay[klabels[ind]] += r;
              //------------------------------------
              //edgesum[klabels[ind]] += edgemag[ind];
              //------------------------------------
              clustersize[klabels[ind]] += 1.0;                 

           
            ind++;
          }            
        }
        
        // iterate over cluster and compute inverse of size
        {for( int k = 0; k < numk; k++ )
        {
            if( clustersize[k] <= 0 ) clustersize[k] = 1;
            inv[k] = 1.0/clustersize[k];//computing inverse now to multiply, than divide later
        }}      

        // iterate over cluster and update
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
        
    } // main loop
}

//===========================================================================
///	EnforceLabelConnectivity
///
///		1. finding an adjacent label for each new component at the start
///		2. if a certain component is too small, assigning the previously found
///		    adjacent label to this component, and not incrementing the label.
//===========================================================================
void PreemptiveSLIC::EnforceLabelConnectivity(
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

/*
Append a single seed to vectors kseedsx, kseedsy, kseedsl, kseedsa,  kseedsb
The seed is located at (x,y) and should cover (x-sx/2,  x+sx/2), (y-sy/2
*/
inline void appendSeed(const Mat& I,const double x, const double y, const double sx, const double sy, vector<double>& kseedsx, vector<double>& kseedsy, vector<double>& kseedsl, vector<double>& kseedsa,  vector<double>& kseedsb)
{
  // position
  kseedsx.push_back(x);
  kseedsy.push_back(y);
  
  // color values
  Vec3b val = I.at<cv::Vec3b>(int(y),int(x));
  //Scalar val = sum( I(Rect(x-sx/2, y-sy/2, sx, sy) ) );
  
  kseedsl.push_back((double)val[0]);
  kseedsa.push_back((double)val[1]);
  kseedsb.push_back((double)val[2]);
}


/*

Initialize seed points

seed coordinates (row major order): 
  seed_idx = seed_y * m_nx + seed_x

to find seed_x and seed_y for a pixel (x,y)
  int seed_x = (x-m_pixel_offset)/m_sx;
  int seed_y = (y-m_pixel_offset)/m_sy;

pixel_offset is -0.51, 
  used to shift pixel coordinates to/from centers of discrete pixel areas
  with -0.5, there are problme at very few positions
   
*/
void PreemptiveSLIC::initSeeds(const Mat& I,
                        const int n,
                        vector<double>& kseedsx, 
                        vector<double>& kseedsy, 
                        vector<double>& kseedsl, 
                        vector<double>& kseedsa,  
                        vector<double>& kseedsb, 
                        int* klabels, 
                        bool init_labels_flag,
                        Mat& seeds)
{
  double h = I.rows;
  double w = I.cols;
  double ny_d = sqrt( n*h/w );
  m_nx = n/ny_d;
  m_ny = round(ny_d);
  
  m_sx = int(w/m_nx);
  m_sy = int(h/m_ny);
  
  m_pixel_offset = -0.5; // to shift and reshift pixel coordinates in the pixel centers
  double cur_x;;
  double cur_y = m_sy/2 + m_pixel_offset;
  int w_int = I.cols;
  
  // if there are no seeds given, initialize in a grid, otherwise use the seeds
  if(seeds.empty())
  {
  
    if(init_labels_flag)
    {
      int label = 0;
      for(int i=0; i<m_ny;i++)
      {
        cur_x = m_sx/2 + m_pixel_offset;
        for(int j=0; j<m_nx; j++)
        {      
          // append a new seed
          appendSeed(I, cur_x, cur_y, m_sx, m_sy, kseedsx, kseedsy, kseedsl, kseedsa,  kseedsb);

          // set initial labels
          for(int ii=round(cur_y-m_pixel_offset-m_sy/2); ii<round(cur_y-m_pixel_offset+m_sy/2); ii++)
            for(int jj=round(cur_x-m_pixel_offset-m_sx/2); jj<round(cur_x-m_pixel_offset+m_sx/2); jj++)
              klabels[ ii*w_int+jj ] = label;

          cur_x += m_sx; 
          label++;
        }    
        cur_y += m_sy;   
      }
    }
    else // do not init labels
    {
      for(int i=0; i<m_ny;i++)
      {
        cur_x = m_sx/2 + m_pixel_offset;
        for(int j=0; j<m_nx; j++)
        {      
          // append a new seed
          appendSeed(I, cur_x, cur_y, m_sx, m_sy, kseedsx, kseedsy, kseedsl, kseedsa,  kseedsb);

          cur_x += m_sx;        
        }    
        cur_y += m_sy;   
      }    
    }
  }
  else // use seeds
  {
    if( seeds.type()!=CV_32FC1)
      cout << "Warning; own_SLIC: seeds matrix should be of type CV_32FC1="<<CV_32FC1<<" but is "<<seeds.type()<<endl;
    for(int i=0; i<seeds.cols; i++)
    {
      // append a new seed TODO (x,y) or (y,x) ?
      appendSeed(I, floor(seeds.at<float>(1,i)), floor(seeds.at<float>(0,i)), m_sx, m_sy, kseedsx, kseedsy, kseedsl, kseedsa,  kseedsb);
    }

  }
  
  // initialize labels
  // -- alternative verison with extra loops for initialisation
//   m_pixel_offset = -0.51; 
//   int h_int = I.rows;
//   int w_int = I.cols;
//   for(int i=0; i<h_int; i++)
//   {
//     int seed_y = (i-m_pixel_offset)/m_sy;
//     for(int j=0;j<w_int;j++)
//     {
//       int seed_x = (j-m_pixel_offset)/m_sx;
//       klabels[ i*w_int+j ] = seed_y * m_nx + seed_x;    
// 
//     }
//   }

}

void PreemptiveSLIC::initSeedsStep(const Mat& I,
                        const int region_size,
                        vector<double>& kseedsx, 
                        vector<double>& kseedsy, 
                        vector<double>& kseedsl, 
                        vector<double>& kseedsa,  
                        vector<double>& kseedsb, 
                        int* klabels, 
                        bool init_labels_flag,
                        Mat& seeds)
{
  double h = I.rows;
  double w = I.cols;
//  double ny_d = sqrt( n*h/w );
//  m_nx = n/ny_d;
//  m_ny = round(ny_d);
  
//  m_sx = w/m_nx;
//  m_sy = h/m_ny;
  
  m_sx = region_size;
  m_sy = region_size;
  
  m_nx = w/m_sx;
  m_ny = h/m_sy;
  
  m_pixel_offset = -0.5; // to shift and reshift pixel coordinates in the pixel centers
  double cur_x;;
  double cur_y = m_sy/2 + m_pixel_offset;
  int w_int = I.cols;
  
  // if there are no seeds given, initialize in a grid, otherwise use the seeds
  if(seeds.empty())
  {
  
    if(init_labels_flag)
    {
      int label = 0;
      for(int i=0; i<m_ny;i++)
      {
        cur_x = m_sx/2 + m_pixel_offset;
        for(int j=0; j<m_nx; j++)
        {      
          // append a new seed
          appendSeed(I, cur_x, cur_y, m_sx, m_sy, kseedsx, kseedsy, kseedsl, kseedsa,  kseedsb);

          // set initial labels
          for(int ii=round(cur_y-m_pixel_offset-m_sy/2); ii<round(cur_y-m_pixel_offset+m_sy/2); ii++)
            for(int jj=round(cur_x-m_pixel_offset-m_sx/2); jj<round(cur_x-m_pixel_offset+m_sx/2); jj++)
              klabels[ ii*w_int+jj ] = label;

          cur_x += m_sx; 
          label++;
        }    
        cur_y += m_sy;   
      }
    }
    else // do not init labels
    {
      for(int i=0; i<m_ny;i++)
      {
        cur_x = m_sx/2 + m_pixel_offset;
        for(int j=0; j<m_nx; j++)
        {      
          // append a new seed
          appendSeed(I, cur_x, cur_y, m_sx, m_sy, kseedsx, kseedsy, kseedsl, kseedsa,  kseedsb);

          cur_x += m_sx;        
        }    
        cur_y += m_sy;   
      }    
    }
  }
  else // use seeds
  {
    if( seeds.type()!=CV_32FC1)
      cout << "Warning; own_SLIC: seeds matrix should be of type CV_32FC1="<<CV_32FC1<<" but is "<<seeds.type()<<endl;
    for(int i=0; i<seeds.cols; i++)
    {
      // append a new seed TODO (x,y) or (y,x) ?
      appendSeed(I, floor(seeds.at<float>(1,i)), floor(seeds.at<float>(0,i)), m_sx, m_sy, kseedsx, kseedsy, kseedsl, kseedsa,  kseedsb);
    }

  }
  
  
  
  // initialize labels
  // -- alternative verison with extra loops for initialisation
//   m_pixel_offset = -0.51; 
//   int h_int = I.rows;
//   int w_int = I.cols;
//   for(int i=0; i<h_int; i++)
//   {
//     int seed_y = (i-m_pixel_offset)/m_sy;
//     for(int j=0;j<w_int;j++)
//     {
//       int seed_x = (j-m_pixel_offset)/m_sx;
//       klabels[ i*w_int+j ] = seed_y * m_nx + seed_x;    
// 
//     }
//   }

}

void PreemptiveSLIC::preemptiveSLIC(const Mat& I_rgb, const int k, const double compactness, int*& klabels, Mat& seeds)
{
   
  const int sz = I_rgb.cols*I_rgb.rows;
  const int superpixelsize = 0.5+double(sz)/double(k);
  
  m_width  = I_rgb.cols;
  m_height = I_rgb.rows;
  
  m_kseedsl.clear();
  m_kseedsa.clear();
  m_kseedsb.clear();
  m_kseedsx.clear();
  m_kseedsy.clear();

  klabels = new int[sz];


  Mat I_lab;
  cvtColor(I_rgb, I_lab, CV_BGR2Lab); // I_lab is CV_8UC3
  
  // fill lab vectors
  m_lvec = new double[sz];
  m_avec = new double[sz];
  m_bvec = new double[sz];
  
  for(int y=0; y<I_lab.rows;y++)
  {
    for(int x=0;x<I_lab.cols;x++)
    {
      int idx = y*I_lab.cols+x; // row major (this is what is used in the other functions!)
      //int idx = x*I_lab.rows+y; // column major (this is the right order!)
      m_lvec[idx] = (double)I_lab.at<cv::Vec3b>(y,x)[0];
      m_avec[idx] = (double)I_lab.at<cv::Vec3b>(y,x)[1];
      m_bvec[idx] = (double)I_lab.at<cv::Vec3b>(y,x)[2];
      
    }
  }
  
  //--------------------------------------------------
  bool perturbseeds(false);//perturb seeds is not absolutely necessary, one can set this flag to false
  vector<double> edgemag(0);
  if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
  
  initSeeds(I_lab, k, m_kseedsx, m_kseedsy, m_kseedsl, m_kseedsa,  m_kseedsb, klabels, 0 /*init labels*/, seeds);
  PerformSuperpixelSLIC_preemptive(m_kseedsl, m_kseedsa, m_kseedsb, m_kseedsx, m_kseedsy, klabels, compactness, 10);
  
  int numlabels = m_kseedsl.size();
  int* nlabels = new int[sz];
  EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, double(sz)/double(m_sx*m_sy));

  int* swapV = klabels;
  klabels = nlabels;
  nlabels = swapV;  
  
  if(nlabels) delete [] nlabels;
  
}


void PreemptiveSLIC::preemptiveSLIC(const Mat& I_rgb, const int region_size, const double compactness, bool perturbseeds, int iterations, bool rgb, int*& klabels, Mat& seeds)
{
   
  const int sz = I_rgb.cols*I_rgb.rows;
//  const int superpixelsize = 0.5+double(sz)/double(k);
  
  m_width  = I_rgb.cols;
  m_height = I_rgb.rows;
  
  m_kseedsl.clear();
  m_kseedsa.clear();
  m_kseedsb.clear();
  m_kseedsx.clear();
  m_kseedsy.clear();

  klabels = new int[sz];


  Mat I_lab;
  if (!rgb)
  {
    cvtColor(I_rgb, I_lab, CV_BGR2Lab);
  }
  else
  {
    I_lab = I_rgb.clone();
  }
  
   // I_lab is CV_8UC3
  
  // fill lab vectors
  m_lvec = new double[sz];
  m_avec = new double[sz];
  m_bvec = new double[sz];
  
  for(int y=0; y<I_lab.rows;y++)
  {
    for(int x=0;x<I_lab.cols;x++)
    {
      int idx = y*I_lab.cols+x; // row major (this is what is used in the other functions!)
      //int idx = x*I_lab.rows+y; // column major (this is the right order!)
      m_lvec[idx] = (double)I_lab.at<cv::Vec3b>(y,x)[0];
      m_avec[idx] = (double)I_lab.at<cv::Vec3b>(y,x)[1];
      m_bvec[idx] = (double)I_lab.at<cv::Vec3b>(y,x)[2];
      
    }
  }
  
  //--------------------------------------------------
//  bool perturbseeds(false);//perturb seeds is not absolutely necessary, one can set this flag to false
  vector<double> edgemag(0);
  if(perturbseeds) DetectLabEdges(m_lvec, m_avec, m_bvec, m_width, m_height, edgemag);
  
  initSeedsStep(I_lab, region_size, m_kseedsx, m_kseedsy, m_kseedsl, m_kseedsa,  m_kseedsb, klabels, 0 /*init labels*/, seeds);
  PerformSuperpixelSLIC_preemptive(m_kseedsl, m_kseedsa, m_kseedsb, m_kseedsx, m_kseedsy, klabels, compactness, iterations);
  
  int numlabels = m_kseedsl.size();
  int* nlabels = new int[sz];
  EnforceLabelConnectivity(klabels, m_width, m_height, nlabels, numlabels, double(sz)/double(m_sx*m_sy));

  int* swapV = klabels;
  klabels = nlabels;
  nlabels = swapV;  
  
  if(nlabels) delete [] nlabels;
  
}