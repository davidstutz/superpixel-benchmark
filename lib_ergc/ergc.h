#include <CImg.h>
#include <iostream>
#include <string>
#include <vector>
#include "Heap.h"
using namespace std;
using namespace cimg_library;

#define SWAP(a,b) float tmp = a; a = b; b = tmp
#define SWAPIF(a,b) if(a>b) { SWAP(a,b); }


//////////////////////////////////
// structures declaration
//////////////////////////////////
struct SP { // superpixel/supervoxel structure
  int xs,ys,zs; // seed coords
  int count;

  CImg<> meanColor;

  bool isalive;

  SP(int xs_, int ys_, int zs_, int nc_) {
    xs=xs_; ys=ys_; zs=zs_; count=0;
    meanColor.resize(1,1,1,nc_).fill(0);
    isalive=true;
  }
};
//////////////////////////////////
struct point2d{
  int x,y;
  point2d(int xx, int yy) {x=xx; y=yy;}
};
//////////////////////////////////
struct point3d{
  int x,y,z;
  point3d(int xx, int yy, int zz) {x=xx; y=yy; z=zz;}
};


//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
// prototypes of functions
float distance_float(float g1, float g2);
float distance_xy(int xa, int ya, int xb, int yb);
float distance_xyz(int xa, int ya, int za, int xb, int yb, int zb);

void fmm2d(CImg<> &D, CImg<int> &imLabels, CImg<int> &S, CImg<> &im, vector<SP*> &SPs, int m);
void fmm3d(CImg<> &Dist, CImg<int> &imLabels, CImg<int> &S, CImg<> &im, vector<SP*> &SVs, int m);

void addNewSeed(CImg<> &D, CImg<int> &imLabels, CImg<int> &S, CImg<> &im, vector<SP*> &SPs);

int placeSeedsOnCustomGrid2d(int W, int H, int dx, int dy, CImg<int> &outseeds);
int placeSeedsOnCustomGrid3d(int W, int H, int D, int dx, int dy, int dz, CImg<int> &outseeds);

int placeSeedsOnRegularGrid2d(int W, int H, int K, CImg<int> &outseeds);
int placeSeedsOnRegularGrid3d(int W, int H, int D, int K, CImg<int> &outseeds);
void perturbSeeds2d(CImg<int> &initialSeeds, CImg<> &perturbMap, CImg<int> &outputSeeds);
void perturbSeeds3d(CImg<int> &initialSeeds, CImg<> &perturbMap, CImg<int> &outputSeeds);

vector<SP*> initialize_superpixels(CImg<> &ims, CImg<int> &inlabels);
vector<SP*> initialize_regions(CImg<> &ims, CImg<int> &inlabels);
void initialize_images(CImg<int> &inlabels, CImg<> &Dist, CImg<int> &S);

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////



//////////////////////////////////
// distance functions 
//////////////////////////////////
float distance_float(float g1, float g2) {
  float d=(g1-g2)*(g1-g2);
  return d;
}
//////////////////////////////////
float distance_xy(int xa, int ya, int xb, int yb) {
  float d=(xa-xb)*(xa-xb);
  d+=(ya-yb)*(ya-yb);
  return d;
}
//////////////////////////////////
float distance_xyz(int xa, int ya, int za, int xb, int yb, int zb) {
  float d=(xa-xb)*(xa-xb);
  d+=(ya-yb)*(ya-yb);
  d+=(za-zb)*(za-zb);
  return d;
}


//////////////////////////////////
// fast marching functions 
//////////////////////////////////
void fmm2d(CImg<> &D, CImg<int> &imLabels, CImg<int> &S, CImg<> &im, vector<SP*> &SPs, int m) {
  /* states S:
   * -1: OK
   *  0: NB
   *  1: FA
   */
  int W=im.width();
  int H=im.height();

  float INF=100000;

  int v4x[] ={-1,0,1,0};
  int v4y[] ={0,1,0,-1};
  int x,y,xx,yy,k;
  float P,a1,a2,A1,delta;

  float Sz=(float)W*H/(float)SPs.size();

  //////////////////////////////
  // initialize heap
  HeapL<point2d*, float> tas(10000000);
  cimg_forXY(S,x,y)
    if(S(x,y)==0) {
      point2d* pt=new point2d(x,y);
      tas.Push(pt,D(x,y));
    }

  ////////////////////////////////
  // let's go
  bool ok=false;
  point2d *pt;
  CImg<> v;
  while(!ok) {
    // current point2d
    pt=tas.Pop();
    x=pt->x;
    y=pt->y;

    delete pt;

    if(S(x,y)!=-1) { // consider only non fixed point2d
      S(x,y)=-1; // fix it !

      // update the mean color of the SP
      int lab=imLabels(x,y);
      cimg_forC(im,c)
	SPs[lab]->meanColor(0,c) = SPs[lab]->meanColor(0,c) * SPs[lab]->count + im(x,y,0,c);
      SPs[lab]->count++;
      SPs[lab]->meanColor /= SPs[lab]->count;

      // go for the neighborhood investigation
      for(k=0;k<4;k++) {
	xx=x+v4x[k];
	yy=y+v4y[k];

	if((xx<W) && (xx>=0) && (yy>=0) && (yy<H)) {
	  P=0;
	  cimg_forC(im,c)
	    P += distance_float(SPs[lab]->meanColor(0,0,0,c), im(xx,yy,0,c));

	  if(m>0) {
	    float dxy=distance_xy(SPs[lab]->xs, SPs[lab]->ys, xx, yy);
	    P=sqrt(P*P + dxy*dxy*m*m/(Sz*Sz));
	  }
	  // compute its neighboring values
	  a1=INF;
	  if(xx<W-1)
	    a1=D(xx+1,yy);
	  if(xx>0)
	    a1=(a1<D(xx-1,yy))?a1:D(xx-1,yy);

	  a2=INF;
	  if(yy<H-1)
	    a2=D(xx,yy+1);
	  if(yy>0)
	    a2=(a2<D(xx,yy-1))?a2:D(xx,yy-1);
	
	  SWAPIF(a1,a2);

	  // update its distance
	  // now the equation is   (a-a1)^2+(a-a2)^2 = P, with a >= a2 >= a1.
	  A1=0;
	  if(P*P > (a2-a1)*(a2-a1) ) {
	    delta=2*P*P-(a2-a1)*(a2-a1);
	    A1 = (a1+a2+sqrt(delta))/2.0;
	  } else {
	    A1 = a1 + P;
	  }
	  if(S(xx,yy)==0) {
	    if(A1<D(xx,yy)) {
	      // update distance
	      D(xx,yy)=A1;
	      imLabels(xx,yy)=imLabels(x,y);
	      point2d* pt1=new point2d(xx,yy);
	      tas.Push(pt1,A1);
	    }
	  } else {
	    if(S(xx,yy)==1) {
	      // add new point2d
	      S(xx,yy)=0;
	      D(xx,yy)=A1;
	      imLabels(xx,yy)=imLabels(x,y);
	      point2d* pt1=new point2d(xx,yy);
	      tas.Push(pt1,A1);
	    }
	  }
	}
      }
    }
    if(tas.Empty()) ok=true;
  }
}
//////////////////////////////////
void fmm3d(CImg<> &Dist, CImg<int> &imLabels, CImg<int> &S, CImg<> &im, vector<SP*> &SVs, int m) {
  /* states S:
   * -1: OK
   *  0: NB
   *  1: FA
   */

  int W=im.width();
  int H=im.height();
  int D=im.depth();

  float INF=100000;

  int v6x[] ={-1,0,1,0,0,0};
  int v6y[] ={0,1,0,-1,0,0};
  int v6z[] ={0,0,0,0,-1,1};
  int x,y,z,xx,yy,zz,k;
  float P,a1,a2,a3,A1,delta;

  float Sz=(float)W*H*D/(float)SVs.size();

  //////////////////////////////
  // initialize heap
  HeapL<point3d*, float> tas(10000000);
  cimg_forXYZ(S,x,y,z)
    if(S(x,y,z)==0) {
      point3d* pt=new point3d(x,y,z);
      tas.Push(pt,Dist(x,y,z));
    }


  ////////////////////////////////
  // let's go
  bool ok=false;
  point3d *pt;
  CImg<> v;
  while(!ok) {
    // current point3d
    pt=tas.Pop();
    x=pt->x;
    y=pt->y;
    z=pt->z;

    delete pt;

    if(S(x,y,z)!=-1) { // consider only non fixed point3d
      S(x,y,z)=-1; // fix it !

      // update the mean color of the SV
      int lab=imLabels(x,y,z);
      cimg_forC(im,c)
	SVs[lab]->meanColor(0,0,0,c) = SVs[lab]->meanColor(0,0,0,c) * SVs[lab]->count + im(x,y,z,c);
      SVs[lab]->count++;
      SVs[lab]->meanColor /= SVs[lab]->count;

      // go for the neighborhood investigation
      for(k=0;k<6;k++) {
	xx=x+v6x[k];
	yy=y+v6y[k];
	zz=z+v6z[k];

	if((xx<W) && (xx>=0) && (yy>=0) && (yy<H) && (zz>=0) && (zz<D)) {
	  P=0;
	  cimg_forC(im,c)
	    P += distance_float(SVs[lab]->meanColor(0,0,0,c), im(xx,yy,zz,c));

	  if(m>0) {
	    float dxyz=distance_xyz(SVs[lab]->xs, SVs[lab]->ys, SVs[lab]->zs, xx, yy, zz);
	    P=sqrt(P*P + dxyz*dxyz*m*m/(Sz*Sz));
	  }
	  // compute its neighboring values
	  a1=INF;
	  if(xx<W-1)
	    a1=Dist(xx+1,yy,zz);
	  if(xx>0)
	    a1=(a1<Dist(xx-1,yy,zz))?a1:Dist(xx-1,yy,zz);

	  a2=INF;
	  if(yy<H-1)
	    a2=Dist(xx,yy+1,zz);
	  if(yy>0)
	    a2=(a2<Dist(xx,yy-1,zz))?a2:Dist(xx,yy-1,zz);

	  a3=INF;
	  if(zz<D-1)
	    a3=Dist(xx,yy,zz+1);
	  if(zz>0)
	    a3=(a3<Dist(xx,yy,zz-1))?a3:Dist(xx,yy,zz-1);
	  
	  SWAPIF(a2,a3);
	  SWAPIF(a1,a2);
	  SWAPIF(a2,a3);

	  // update its distance
	  // now the equation is   (a-a1)^2+(a-a2)^2+(a-a3)^2 - P^2 = 0, with a >= a3 >= a2 >= a1.
	  // =>    3*a^2 - 2*(a2+a1+a3)*a - P^2 + a1^2 + a3^2 + a2^2
	  // => delta = (a2+a1+a3)^2 - 3*(a1^2 + a3^2 + a2^2 - P^2)
	  A1=0;
	  delta=(a2+a1+a3)*(a2+a1+a3) - 3*(a1*a1 + a2*a2 + a3*a3 - P*P);
	  A1 = 0;
	  if( delta>=0 )
	    A1 = ( a2+a1+a3 + sqrt(delta) )/3.0;
	  if( A1<=a3 ) {
	    // at least a3 is too large, so we have
	    // a >= a2 >= a1  and  a<a3 so the equation is 
	    //		(a-a1)^2+(a-a2)^2 - P^2 = 0
	    //=> 2*a^2 - 2*(a1+a2)*a + a1^2+a2^2-P^2
	    // delta = (a2+a1)^2 - 2*(a1^2 + a2^2 - P^2)
	    delta = (a2+a1)*(a2+a1) - 2*(a1*a1 + a2*a2 - P*P);
	    A1 = 0;
	    if( delta>=0 )
	      A1 = 0.5 * ( a2+a1 +sqrt(delta) );
	    if( A1<=a2 )
	      A1 = a1 + P;
	  }

	  if(S(xx,yy,zz)==0) {
	    if(A1<Dist(xx,yy,zz)) {
	      // update distance
	      Dist(xx,yy,zz)=A1;
	      imLabels(xx,yy,zz)=imLabels(x,y,z);
	      point3d* pt1=new point3d(xx,yy,zz);
	      tas.Push(pt1,A1);
	    }
	  } else {
	    if(S(xx,yy,zz)==1) {
	      // add new point2d
	      S(xx,yy,zz)=0;
	      Dist(xx,yy,zz)=A1;
	      imLabels(xx,yy,zz)=imLabels(x,y,z);
	      point3d* pt1=new point3d(xx,yy,zz);
	      tas.Push(pt1,A1);
	    }
	  }
	}
      }
    }
    if(tas.Empty()) ok=true;
  }
}







//////////////////////////////////
// refining functions
//////////////////////////////////
void addNewSeed(CImg<> &D, CImg<int> &imLabels, CImg<int> &S, CImg<> &im, vector<SP*> &SPs) {
  // 2d case
  int v4x[] ={-1,0,1,0};
  int v4y[] ={0,1,0,-1};  
  // 3d case
  int v6x[] ={-1,0,1,0,0,0};
  int v6y[] ={0,1,0,-1,0,0};
  int v6z[] ={0,0,0,0,1,-1};

  float INF=100000;
  float max=0;
  int xmax=-1, ymax=-1, zmax=0, xx, yy, zz, k;
  if(D.depth()==1) {
    cimg_for_insideXY(D,x,y,1) {
      if(D(x,y)>max) {
	max=D(x,y);
	xmax=x;
	ymax=y;
      }
    }
  } else {
    cimg_forXYZ(D,x,y,z) {
      if(D(x,y,z)>max) {
	max=D(x,y,z);
	xmax=x;
	ymax=y;
	zmax=z;
      }
    }
  }
  SP* new_SP=new SP(xmax,ymax,zmax,im.spectrum());
  SPs.push_back(new_SP);

  // find adjacent regions of the old region that contains (xmax,ymax,zmax)
  int oldRegion=imLabels(xmax,ymax,zmax);
  CImg<int> adjacentRegions; adjacentRegions.resize(SPs.size(),1).fill(0);
  adjacentRegions(SPs.size()-1)=1; // add indice of the new region
  adjacentRegions(oldRegion)=1; // add indice of the old region
  // look for all the indices that are adjacent to the old region
  if(im.depth()==1) { // 2d case
    cimg_for_insideXY(imLabels,x,y,1)
      if(imLabels(x,y)==oldRegion) {
	for(k=0;k<4;k++) {
	  xx=x+v4x[k];
	  yy=y+v4y[k];
	  if(imLabels(xx,yy)!=oldRegion)
	    adjacentRegions(imLabels(xx,yy))=1;
	}
      }
  } else { // 3d case
    cimg_for_insideXYZ(imLabels,x,y,z,1)
      if(imLabels(x,y,z)==oldRegion) {
	for(k=0;k<6;k++) {
	  xx=x+v6x[k];
	  yy=y+v6y[k];
	  zz=z+v6z[k];
	  if(imLabels(xx,yy,zz)!=oldRegion)
	    adjacentRegions(imLabels(xx,yy,zz))=1;
	}
      }
  }

  // compute new initial S, imLabels, D
  cimg_forXYZ(imLabels,x,y,z) {
    int lab=imLabels(x,y,z);
    if(adjacentRegions(lab)==0) { // not to be refined
      S(x,y,z)=-1;
    } else { // to refine
      S(x,y,z)=1;
      imLabels(x,y,z)=-1;
    }
  }
  for(unsigned int sp=0;sp<SPs.size();sp++) {
    if(adjacentRegions(sp)==1) { // re-initialization of adjacent SP
      S(SPs[sp]->xs,SPs[sp]->ys,SPs[sp]->zs)=0;
      imLabels(SPs[sp]->xs,SPs[sp]->ys,SPs[sp]->zs)=sp; // the label of a SP is its indice in the list SPs
      cimg_forC(im,c)
	SPs[sp]->meanColor(0,0,0,c) = im(SPs[sp]->xs,SPs[sp]->ys,SPs[sp]->zs,c);
      SPs[sp]->count=1;
      if(im.depth()==1) { // 2d case
	for(k=0;k<4;k++) {
	  xx=SPs[sp]->xs+v4x[k];
	  yy=SPs[sp]->ys+v4y[k];
	  if(im.containsXYZC(xx,yy)) {
	    cimg_forC(im,c)
	      SPs[sp]->meanColor(0,0,0,c) += im(xx,yy,0,c);
	    SPs[sp]->count++;
	  }
	}
      } else { // 3d case
	for(k=0;k<6;k++) {
	  xx=SPs[sp]->xs+v6x[k];
	  yy=SPs[sp]->ys+v6y[k];
	  zz=SPs[sp]->zs+v6z[k];
	  if(im.containsXYZC(xx,yy)) {
	    cimg_forC(im,c)
	      SPs[sp]->meanColor(0,0,0,c) += im(xx,yy,zz,c);
	    SPs[sp]->count++;
	  }
	}
      }
      SPs[sp]->meanColor /= SPs[sp]->count;
    }
  }
  // re-initialization of distance map
  cimg_forXYZ(imLabels,x,y,z) {
    if(S(x,y,z)!=-1)
      D(x,y,z)=INF;
    if(S(x,y,z)==0)
      D(x,y,z)=0;
  }
}




//////////////////////////////////
// seeds location initialization functions
//////////////////////////////////
int placeSeedsOnCustomGrid2d(int W, int H, int dx, int dy, CImg<int> &outseeds) {
  outseeds.resize(W,H).fill(-1);

  int xoff=dx/2;
  int yoff=dy/2;

  int x=xoff,y;
  int c=0;
  while(x<W) {
    y=yoff;
    while(y<H) {
      outseeds(x,y)=c++;
      y += dy;
    }
    x += dx;
  }
  return c;
}

int placeSeedsOnCustomGrid3d(int W, int H, int D, int dx, int dy, int dz, CImg<int> &outseeds) {
  outseeds.resize(W,H,D).fill(-1);

  int xoff=dx/2;
  int yoff=dy/2;
  int zoff=dz/2;

  int x=xoff,y,z;
  int c=0;
  while(x<W) {
    y=yoff;
    while(y<H) {
      z=zoff;
      while(z<D) {
	outseeds(x,y,z)=c++;
	z += dz;
      }
      y += dy;
    }
    x += dx;
  }
  return c;
}


int placeSeedsOnRegularGrid2d(int W, int H, int K, CImg<int> &outseeds) {
  outseeds.resize(W,H).fill(-1);

  int superpixelsize=0.5+double(W*H)/double(K);
  int STEP=sqrt(double(superpixelsize))+0.5;

  int xstrips = (0.5+double(W)/double(STEP));
  int ystrips = (0.5+double(H)/double(STEP));

  int xerr = W - STEP*xstrips;if(xerr < 0){xstrips--;xerr = W - STEP*xstrips;}
  int yerr = H - STEP*ystrips;if(yerr < 0){ystrips--;yerr = H - STEP*ystrips;}

  double xerrperstrip = double(xerr)/double(xstrips);
  double yerrperstrip = double(yerr)/double(ystrips);

  int xoff = STEP/2;
  int yoff = STEP/2;

  int n=0;
  for( int y = 0; y < ystrips; y++ ) {
    int ye = y*yerrperstrip;
    for( int x = 0; x < xstrips; x++ ) {
      int xe = x*xerrperstrip;
      outseeds(x*STEP+xoff+xe,y*STEP+yoff+ye)=n;
      n++;
    }
  }
  return n;
}

int placeSeedsOnRegularGrid3d(int W, int H, int D, int K, CImg<int> &outseeds) {
  outseeds.resize(W,H,D).fill(-1);

  int superpixelsize=0.5+double(W*H*D)/double(K);
  int STEP=sqrt(double(superpixelsize))+0.5;
  STEP=pow(double(superpixelsize),1./3);

  int xstrips = (0.5+double(W)/double(STEP));
  int ystrips = (0.5+double(H)/double(STEP));
  int zstrips = (0.5+double(D)/double(STEP));

  int xerr = W - STEP*xstrips;if(xerr < 0){xstrips--;xerr = W - STEP*xstrips;}
  int yerr = H - STEP*ystrips;if(yerr < 0){ystrips--;yerr = H - STEP*ystrips;}
  int zerr = D - STEP*zstrips;if(zerr < 0){zstrips--;zerr = D - STEP*zstrips;}

  double xerrperstrip = double(xerr)/double(xstrips);
  double yerrperstrip = double(yerr)/double(ystrips);
  double zerrperstrip = double(zerr)/double(zstrips);

  int xoff = STEP/2;
  int yoff = STEP/2;
  int zoff = STEP/2;

  if(STEP>W/2.) xoff=(int)(W/2.);
  if(STEP>H/2.) yoff=(int)(H/2.);
  if(STEP>D/2.) zoff=(int)(D/2.);

  int n=0;
  for( int z = 0; z < zstrips; z++ ) {
    int ze = z*zerrperstrip;
    for( int y = 0; y < ystrips; y++ ) {
      int ye = y*yerrperstrip;
      for( int x = 0; x < xstrips; x++ ) {
	int xe = x*xerrperstrip;
	outseeds(x*STEP+xoff+xe,y*STEP+yoff+ye,z*STEP+zoff+ze)=n;
	n++;
      }
    }
  }
  return n;
}
// perturb seeds according to a perturbMap (gradient map for example)
void perturbSeeds2d(CImg<int> &initialSeeds, CImg<> &perturbMap, CImg<int> &outputSeeds) {
  // only works for grid seeds
  int v8x []={-1, 0, 1, 0, -1, -1, 1, 1};
  int v8y []={0 ,-1, 0, 1, -1, 1, 1, -1};

  int W=initialSeeds.width();
  int H=initialSeeds.height();

  outputSeeds.resize(W,H).fill(-1);

  float v=0;
  int xx,yy,k,lab,xmin,ymin;
  cimg_forXY(initialSeeds,x,y) {
    if(initialSeeds(x,y)!=-1) {
      v=perturbMap(x,y);
      xmin=x;
      ymin=y;
      lab=initialSeeds(x,y);
      for(k=0;k<8;k++) {
	xx=x+v8x[k];
	yy=y+v8y[k];
	if((xx>=0) && (xx<W))
	  if((yy>=0) && (yy<H))
	    if(perturbMap(xx,yy)<v) {
	      v=perturbMap(xx,yy);
	      xmin=xx;
	      ymin=yy;
	    }
      }
      outputSeeds(xmin,ymin)=lab;
    }
  }
}
void perturbSeeds3d(CImg<int> &initialSeeds, CImg<> &perturbMap, CImg<int> &outputSeeds) {
  // only works for grid seeds
  int v26x []={-1, 0, 1, 0, -1, -1, 1, 1,      -1,-1,-1,0,0,0,1,1,1,            -1,-1,-1,0,0,0,1,1,1};
  int v26y []={0 ,-1, 0, 1, -1, 1, 1, -1,      -1,0,1,-1,0,1,-1,0,1,            -1,0,1,-1,0,1,-1,0,1};
  int v26z []={0,0,0,0,0,0,0,0,                -1,-1,-1,-1,-1,-1,-1,-1,-1,          1,1,1,1,1,1,1,1, 1};

  int W=initialSeeds.width();
  int H=initialSeeds.height();
  int D=initialSeeds.depth();

  outputSeeds.resize(W,H,D).fill(-1);

  float v=0;
  int xx,yy,zz,k,lab,xmin,ymin,zmin;
  cimg_forXYZ(initialSeeds,x,y,z) {
    if(initialSeeds(x,y,z)!=-1) {
      v=perturbMap(x,y,z);
      xmin=x;
      ymin=y;
      zmin=z;
      lab=initialSeeds(x,y,z);
      for(k=0;k<26;k++) {
	xx=x+v26x[k];
	yy=y+v26y[k];
	zz=z+v26z[k];
	if((xx>=0) && (xx<W))
	  if((yy>=0) && (yy<H))
	    if((zz>=0) && (zz<D))
	      if(perturbMap(xx,yy,zz)<v) {
		v=perturbMap(xx,yy,zz);
		xmin=xx;
		ymin=yy;
		zmin=zz;
	      }
      }
      outputSeeds(xmin,ymin,zmin)=lab;
    }
  }
}


//////////////////////////////////
// superpixels initialization functions (for grid seeds)
//////////////////////////////////
vector<SP*> initialize_superpixels(CImg<> &ims, CImg<int> &inlabels) {
  // 2d case
  int v4x[] ={-1,0,1,0};
  int v4y[] ={0,1,0,-1};
  // 3d case
  int v6x[] ={-1,0,1,0,0,0};
  int v6y[] ={0,1,0,-1,0,0};
  int v6z[] ={0,0,0,0,1,-1};

  // we create the vector of superpixels/supervoxels
  vector<SP*> SPs;
  int nseeds=inlabels.max()+1;
  for(int k=0;k<nseeds;k++) {
    SP* sp=new SP(-1,-1,0,ims.spectrum());
    SPs.push_back(sp);
  }

  int lab,k,xx,yy,zz;
  if(ims.depth()==1) { // 2d case
    cimg_forXY(inlabels,x,y) {
      lab=inlabels(x,y);
      if(lab!=-1) { // seed
	SPs[lab]->xs=x;
	SPs[lab]->ys=y;
	cimg_forC(ims,c)
	  SPs[lab]->meanColor(0,0,0,c) = ims(x,y,0,c);
	SPs[lab]->count++;
	for(k=0;k<4;k++) { // initialization with neighbors too
	  xx=x+v4x[k];
	  yy=y+v4y[k];
	  if(ims.containsXYZC(xx,yy))
	    if(inlabels(xx,yy)==-1) {
	      cimg_forC(ims,c)
		SPs[lab]->meanColor(0,0,0,c) += ims(xx,yy,0,c);
	      SPs[lab]->count++;
	    }
	}
	SPs[lab]->meanColor /= SPs[lab]->count;
      }
    }
  } else { // 3d case
    cimg_forXYZ(inlabels,x,y,z) {
      lab=inlabels(x,y,z);
      if(lab!=-1) { // seed
	SPs[lab]->xs=x;
	SPs[lab]->ys=y;
	SPs[lab]->zs=z;
	cimg_forC(ims,c)
	  SPs[lab]->meanColor(0,0,0,c) = ims(x,y,z,c);
	SPs[lab]->count++;
	for(k=0;k<6;k++) { // initialization with neighbors too
	  xx=x+v6x[k];
	  yy=y+v6y[k];
	  zz=z+v6z[k];
	  if(ims.containsXYZC(xx,yy,zz))
	    if(inlabels(xx,yy,zz)==-1) {
	      cimg_forC(ims,c)
		SPs[lab]->meanColor(0,0,0,c) += ims(xx,yy,zz,c);
	      SPs[lab]->count++;
	    }
	}
	SPs[lab]->meanColor /= SPs[lab]->count;
      }
    }
  }
  return SPs;
}

// for given seeds image
// we assume that regions are numbered in [0-n] without "holes" in numbering
// we assume background is labeled -1
vector<SP*> initialize_regions(CImg<> &ims, CImg<int> &inlabels) {
  // we create the vector of superpixels/supervoxels
  vector<SP*> SPs;
  int nseeds=inlabels.max()+1;
  for(int k=0;k<nseeds;k++) {
    SP* sp=new SP(-1,-1,0,ims.spectrum());
    SPs.push_back(sp);
  }
  cimg_forXYZ(ims,x,y,z) {
    int lab=inlabels(x,y,z);
    if(lab!=-1) {
      // xs,ys,zs not taken into account (maybe in a future release..)
      cimg_forC(ims,c)
	SPs[lab]->meanColor(0,0,0,c) += ims(x,y,z,c);
      SPs[lab]->count++;
    }
  }
  for(unsigned int sp=0;sp<SPs.size();sp++)
    if(SPs[sp]->count!=0)
      SPs[sp]->meanColor /= SPs[sp]->count;
  return SPs;
}


//////////////////////////////////
// function that initializes distances and states images
//////////////////////////////////
void initialize_images(CImg<int> &inlabels, CImg<> &Dist, CImg<int> &S) {
  Dist.resize(inlabels.width(),inlabels.height(),inlabels.depth()).fill(1000000);
  S.resize(inlabels.width(),inlabels.height(),inlabels.depth()).fill(1);
  cimg_forXYZ(inlabels,x,y,z)
    if(inlabels(x,y,z)!=-1) {
      Dist(x,y,z)=0;
      S(x,y,z)=0;
    }
}

//////////////////////////////////
// gradient norm function
//////////////////////////////////
CImg<> compute_gradient(CImg<> &im) {
  CImg<> gradient;
  gradient.resize(im.width(),im.height(),im.depth()).fill(0);
  CImgList<> grads=im.get_gradient();
  for(unsigned int k=0;k<grads.size();k++) {
    grads(k).pow(2);
    cimg_forC(im,c)
      gradient+=grads(k).get_channel(c);
  }
  gradient.sqrt();
  return gradient;
}
