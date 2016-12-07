/////////////////////////////////////////////////////////////////////////////
// Name:        BGDefaults.h
// Purpose:     Defines some program constants
// Author:      Bogdan Georgescu
// Modified by:
// Created:     06/22/2000
// Copyright:   (c) Bogdan Georgescu
// Version:     v0.1
/////////////////////////////////////////////////////////////////////////////

#define PI 3.1415926535
#define GTRESH PI/6
#define GTRESH_2 PI/4
#define RTRESH 7.0
#define RTRESH_2 20
#define DF_SIGMA 1.0
#define DF_HIGH 0.9
#define DF_LOW 0.5
#define DF_MINN 5

#define FC_ELLIPSE 0
#define FC_VERT_LINE 1
#define FC_HORIZ_LINE 2
#define FC_LINE 3
#define FC_SQUARE_BOX 4
#define FC_CUSTOM 5

// Line detection
#define NE_TRESH 5
#define MIN_DIST 30
#define DMAX 9
#define DMAX2 DMAX*DMAX
#define DMIN2 (DMAX-2)*(DMAX-2)
#define ALPHA_MAX 160.0*PI/180.0

#define MIN_LINE_REZID 1.2
//#define MIN_LINE_INLIER 7
#define MIN_LINE_INLIER 15

#define M_FRM_NO_POINT -10000000

#define AUGMENT_MIN_CORN 25
#define AUGMENT_LINE_STEP 10

extern double bgSign(double);
extern int bgSolveCubic(double, double, double, double, double&, double&, double&);
extern inline int bgRound(double);
extern inline int bgRoundSign(double);
extern double bgMedian(double*, int, double);
extern inline double bgMedianToSigmaGaussian(double);
extern void bgSort(double*, int);

//image sampling functions
extern void bgZoomIn(unsigned char**, unsigned char*, int, int, int, bool);
extern void bgZoomOut(unsigned char**, unsigned char*, int, int, int, bool);

//file extension function
extern void BgAddExtension(char**, char*);

extern void bgLog(const char*, ...);
extern void bgLogFile(const char*, ...);
extern int write_pgm_image(const char *outfilename, unsigned char *image, int rows,
    int cols, char *comment, int maxval);
extern void write_MATLAB_ASCII(char *filename, float *data, int rows, int cols);
extern "C" int lmdif(int m, int n,double x[], double fvec[] , double ftol, double xtol, double gtol, int maxfev,
						 double epsfcn,double diag[],int mode, double factor, int nprint, int *info, int *nfev,
						 double fjac[],int ldfjac, int ipvt[], double qtf[], double wa1[], double wa2[] ,
						 double wa3[], double wa4[]);
extern void timer_start();
extern void timer_stop();
