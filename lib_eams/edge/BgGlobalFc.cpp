/////////////////////////////////////////////////////////////////////////////
// Name:        BgGlobalFc.cpp
// Purpose:     global functions
// Author:      Bogdan Georgescu
// Modified by:
// Created:     06/22/2000
// Copyright:   (c) Bogdan Georgescu
// Version:     v0.1
/////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "BgDefaults.h"
// !!! only MSV
#include <sys/timeb.h>
#include <time.h>

double bgSign(double x)
{
   if (x>=0)
      return 1.0;
   return -1.0;
}

double factorial(double num)
{
   if (num==0 || num==1)
      return 1;
   return (num * factorial(num - 1));
}

// return 0 - error
// return 1 - one real solution
// return 3 - three real solutions
int bgSolveCubic(double a, double b, double c, double d, double& s1, double& s2, double& s3)
{
   double p, q;
   double r, s, t, z;

   // convert to canonical form
   r = b/a;
   s = c/a;
   t = d/a;
   p = s-(r*r)/3.0;
   q = (2*r*r*r)/27.0-(r*s)/3.0+t;

   double D, phi, R;
   R = bgSign(q)*sqrt(fabs(p)/3.0);
   D = pow(p/3.0,3)+pow(q/2,2);

   if (p<0)
   {
      if (D<=0)
      {
         phi = acos(q/(2*R*R*R));
         s1 = -2*R*cos(phi/3)-r/3;
         s2 = -2*R*cos(phi/3+2*PI/3)-r/3;
         s3 = -2*R*cos(phi/3+4*PI/3)-r/3;
         return 3;
      }
      else
      {
         z = q/(2*R*R*R);
         phi = log(z+sqrt(z*z-1));
         s1 = -2*R*cosh(phi/3)-r/3;
         return 1;
      }
   }
   else
   {
      z = q/(2*R*R*R);
      phi = log(z+sqrt(z*z+1));
      s1 = -2*R*sinh(phi/3)-r/3;
      return 1;
   }

   return 0;
}



inline int bgRound(double in_x)
{
  return int(floor(in_x + 0.5));
}

inline int bgRoundSign(double in_x)
{
   if (in_x>=0)
   {
      return ((int) (in_x + 0.5));
   }
   else
   {
      return ((int) (in_x - 0.5));
   }
}

void bgSort(double* ra, int nVec)
{
   unsigned long n, l, ir, i, j;
   n = nVec;
   double rra;
   
   if (n<2)
      return;

   l = (n>>1)+1;
   ir = n;
   for (;;)
   {
      if (l>1)
      {
         rra = ra[(--l)-1];
      }
      else
      {
         rra = ra[ir-1];
         ra[ir-1] = ra[1-1];
         if (--ir==1)
         {
            ra[1-1] = rra;
            break;
         }
      }
      i = l;
      j = l+l;
      while (j<=ir)
      {
         if (j<ir && ra[j-1]<ra[j+1-1])
            j++;
         if (rra<ra[j-1])
         {
            ra[i-1] = ra[j-1];
            i = j;
            j <<= 1;
         }
         else
            j = ir+1;
      }
      ra[i-1] = rra;
   }
}

// rank in 0-1 range, 0 min, 1 max
// inplace sort vec
double bgMedian(double* vec, int nVec, double rank)
{
   bgSort(vec, nVec);
   int krank = int(floor(rank*nVec));
   if (rank == 1)
      krank = nVec-1;
   return vec[krank];
}

double bgMedianToSigmaGaussian(double med)
{
  return med * 1.482602219;
}

int write_pgm_image(const char *outfilename, unsigned char *image, int rows,
    int cols, char *comment, int maxval)
{
   FILE *fp;

   /***************************************************************************
   * Open the output image file for writing if a filename was given. If no
   * filename was provided, set fp to write to standard output.
   ***************************************************************************/

   if(outfilename == NULL) fp = stdout;
   else{
      if((fp = fopen(outfilename, "wb")) == NULL){
         fprintf(stderr, "Error writing the file %s in write_pgm_image().\n",
            outfilename);
         return(0);
      }
   }

   /***************************************************************************
   * Write the header information to the PGM file.
   ***************************************************************************/

   fprintf(fp, "P5\n%d %d\n", cols, rows);
   if(comment != NULL)
      if(strlen(comment) <= 70) fprintf(fp, "# %s\n", comment);
   fprintf(fp, "%d\n", maxval);

   /***************************************************************************
   * Write the image data to the file.
   ***************************************************************************/

   if(rows != fwrite(image, cols, rows, fp)){
      fprintf(stderr, "Error writing the image data in write_pgm_image().\n");
      if(fp != stdout) fclose(fp);
      return(0);
   }

   if(fp != stdout) fclose(fp);
   return(1);
}


void write_MATLAB_ASCII(char *filename, float *data, int rows, int cols)
{
	FILE *fp	= fopen(filename, "wb");
	int i,j;
	for(i = 0; i < rows; i++)
	{
		for(j = 0; j < cols-1; j++)
		{
			fprintf(fp, "%10.6f ", data[i*rows+j]);
		}
		fprintf(fp, "%10.6f\n", data[i*rows+cols-1]);
	}
	fclose(fp);
}

/*
struct _timeb timestart;
struct _timeb timeend;
void timer_start()
{
   _ftime( &timestart );
   bgLog("timer start...\n");
}
void timer_stop()
{
   _ftime( &timeend );

   unsigned long seconds;
   unsigned long milliseconds;
   seconds = timeend.time - timestart.time;
   long msdif;
   msdif = timeend.millitm - timestart.millitm;
   if (msdif > 0)
      milliseconds = msdif;
   else
   {
      seconds--;
      milliseconds = (timeend.millitm + 1000) - timestart.millitm;
   }

   bgLog("timer stop, elapsed %d.%d seconds.\n", seconds, milliseconds);
}
*/
time_t timestart;
time_t timeend;
void timer_start()
{
   timestart = clock();
   bgLog("timer start...\n");
}
void timer_stop()
{
   timeend = clock();
   unsigned long seconds, milliseconds;
   seconds = (timeend-timestart)/CLOCKS_PER_SEC;
   milliseconds = ((100*(timeend-timestart))/CLOCKS_PER_SEC) - 100*seconds;
   bgLog("timer stop, elapsed %d.%d seconds.\n", seconds, milliseconds);
}


/********************************************/
/*				Image Sampling				*/
/********************************************/

//zooms into an image
//pre : 
//		- dest is the pre-allocated destination color image
//		- src is the (w x h) source color image
//		- zconst is an integer zoom constant from that is >= 1
//		- interpolate is a flag that determines if interpolation
//		  should occur
//post: the src image has been zoomed by (zconst)x and stored into dest
void bgZoomIn(unsigned char **dest, unsigned char *src, int w, int h, int zconst, bool interpolate)
{

	//if dest or src is NULL or zconst is < 1 exit
	if((!(*dest))||(!src)||(zconst < 1))
		return;

	//if zconst = 1 then copy the image contents and return
	if(zconst == 1)
	{
		memcpy(*dest, src, 3*w*h*sizeof(unsigned char));
		return;
	}

	//calculate new image dimension
	w	*= zconst;
	h	*= zconst;

	//copy image data from source image to destination image

	//******************************************************//

	unsigned char *dptr	= (*dest);

	int i, j, x, y, dp, offset, factor, index;
	index = 0;
	//zconst does not divide evenly into image width
	if(factor = w%zconst)
	{
		for(j=0; j<(h-zconst); j+=zconst)
		{
			for(i=0; i<(w-zconst); i+=zconst)
			{
				dp	= 3*(j*w+i);
				for(y=0; y<zconst; y++)
				{
					for(x=0; x<zconst; x++)
					{
						offset	= 3*(y*w+x);
						dptr[dp+offset  ]	= src[index  ];
						dptr[dp+offset+1]	= src[index+1];
						dptr[dp+offset+2]	= src[index+2];
					}
				}

				//next data point
				index	+= 3;
			}

			dp	= 3*(j*w+i);
			for(y=0; y<(zconst-factor-1); y++)
			{
				for(x=0; x<(zconst-factor-1); x++)
				{
					offset	= 3*(y*w+x);
					dptr[dp+offset  ]	= src[index  ];
					dptr[dp+offset+1]	= src[index+1];
					dptr[dp+offset+2]	= src[index+2];
				}
			}

			//next data point
			index	+= 3;
		}
	}
	//zconst does divide evenly into image width
	else
	{
		for(j=0; j<(h-zconst); j+=zconst)
		{
			for(i=0; i<(w); i+=zconst)
			{
				dp	= 3*(j*w+i);
				for(y=0; y<zconst; y++)
				{
					for(x=0; x<zconst; x++)
					{
						offset	= 3*(y*w+x);
						dptr[dp+offset  ]	= src[index  ];
						dptr[dp+offset+1]	= src[index+1];
						dptr[dp+offset+2]	= src[index+2];
					}
				}

				//next data point
				index	+= 3;
			}
		}
	}

	//consider last row of image
	factor	= h%zconst;
	dp		= 3*(h-zconst)*(w);
	for(i=0; i<(w); i+=zconst)
	{
		for(y=0; y<(zconst-factor); y++)
		{
			for(x=0; x<(zconst-factor); x++)
			{
				offset	= 3*(y*w+x);
				dptr[dp+offset  ]	= src[index  ];
				dptr[dp+offset+1]	= src[index+1];
				dptr[dp+offset+2]	= src[index+2];
			}
		}
		//next data point
		dp		+= zconst*3;
		index	+= 3;
	}

	//******************************************************//

	//done.
	return;

}

//zooms out of an image
//pre : 
//		- dest is the pre-allocated destination color image
//		- src is the (w x h) source color image
//		- zconst is an integer zoom constant from that is >= 1
//		- interpolate is a flag that determines if interpolation
//		  should occur
//post: the src image has been zoomed by (1/zconst)x and stored into dest
void bgZoomOut(unsigned char **dest, unsigned char *src, int w, int h, int zconst, bool interpolate)
{

	//if dest or src is NULL or zconst is <= 1 exit
	if((!(*dest))||(!src)||(zconst <= 1))
		return;

	//copy image data from source image to destination image

	//******************************************************//

	unsigned char *dptr	= (*dest);

	int i, j, dp, index;
	index = 0;
	for(j=0; j<h; j+=zconst)
	{
		for(i=0; i<w; i+=zconst)
		{
			dp	= 3*(j*w+i);
			dptr[index  ]	= src[dp  ];
			dptr[index+1]	= src[dp+1];
			dptr[index+2]	= src[dp+2];
		
			//next data point
			index	+= 3;
		}
	}

	//******************************************************//

	//done.
	return;

}

/************************************/
/*       Filename Manipulation      */
/************************************/

//adds an extension (label) to a filename
void BgAddExtension(char **filename, char *label)
{
	//allocate memory for new filename
	char *new_filename	= new char [strlen(*filename) + strlen(label) + 1], ext[5];

	//copy filename
	strcpy(new_filename, *filename);

	//get extension of filename (e.g. '.txt')
	char *pdest = strchr(new_filename, '.');
	strcpy(ext, pdest);

	//place filename label at the end of the filename
	//followed by extension...
	strcpy(pdest, label);
	strcat(new_filename, ext);

	//delete old filename and replace it with new one...
	delete *filename;
	(*filename)	= new_filename;

	//done.
	return;
}

