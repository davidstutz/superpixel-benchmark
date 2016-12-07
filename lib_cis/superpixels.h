#ifndef SUPERPIXELS_H
#define	SUPERPIXELS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <float.h>
#include <image.h>
#include <misc.h>
#include <pnmfile.h>
#include <matrix.h>
#include <misc.h>
#include <draw.h>
#include <errno.h>
#include "graph.h"
#include "energy.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace vlib;

#define NUM_COLORS 255
#define MULTIPLIER_VAR 1.5
#define sq(x) ((x)*(x))

typedef int Value;
typedef int TotalValue;
typedef int Var;

void check_error(int boolE, const char *error_message);
void PlaceSeeds(image<uchar> *I,int width,int height,
        int num_pixels, vector<int> &Seeds,int *numSeeds, int PATCH_SIZE);
void MoveSeedsFromEdges(image<uchar> *I,int width,int height,int num_pixels,
        vector<int> &Seeds, int numSeeds, int PATCH_SIZE);
int computeEnergy(vector<int> &labeling,int width,int height,int num_pixels,
        vector<Value> &horizWeights,vector<Value> &vertWeights,
        vector<Value> &diag1Weights,vector<Value> &diag2Weights,
        vector<int> &Seeds, image<uchar> *I,int TYPE);
void getBounds(int width,int height,vector<int> &Seeds,int *seedX,int *seedY,
        int *startX,int *startY,int *endX,int *endY,int label, int PATCH_SIZE);
void expandOnLabel(int label,int width,int height,int num_pixels,
        vector<int> &Seeds,int numSeeds, vector<int> &labeling,
        vector<Value> &horizWeights,vector<Value> &vertWeights,Value lambda,
        vector<Value> &diag1Weights,vector<Value> &diag2Weights,int PATCH_SIZE,
        vector<int> &changeMask, vector<int> &changeMaskNew,image<uchar> *I,
        int TYPE,float variance);
void initializeLabeling(vector<int> &labeling,int width,int height,
        vector<int> &Seeds,int numSeeds, int PATCH_SIZE);
float  computeImageVariance(image<uchar> *I,int width,int height);
void loadEdges(vector<Value> &weights,int num_pixels,int width,int height,
               Value lambda, vlib::image<unsigned char> *edges);
void computeWeights(vector<Value> &weights,int num_pixels,int width,
                    int height,Value lambda, float variance,
                    int incrX,int incrY, image<uchar> *I,
                    int TYPE, float sigma);
int  saveSegmentationColor(vector<int> &labeling,int width,int height,int num_pixels,
                           image<uchar> *I, int numSeeds, char *name);
void purturbSeeds(vector<int> &order,int numSeeds);
void check_input_arguments(int argc);

Value computeEnergyColor(vector<int> &labeling,int width,int height,int num_pixels,
                    vector<Value> &horizWeights,vector<Value> &vertWeights,
                    vector<Value> &diag1Weights,vector<Value> &diag2Weights,
                    vector<int> &Seeds, image<rgb> *I,int TYPE)
{
    TotalValue engSmooth = 0,engData = 0;
    float SQRT_2= 1/sqrt(2.0);


    if ( TYPE == 1 )
    {
        for ( int y = 0; y < height; y++ )
            for ( int x = 0; x < width; x++){
                int label = labeling[x+y*width];
                int seedY = Seeds[label]/width;
                int seedX = Seeds[label] - seedY*width;
                rgb seedColor = imRef(I,seedX,seedY);
                rgb color = imRef(I,x,y);
                
                int diff = abs(seedColor.r - color.r)
                        + abs(seedColor.g - color.g)
                        + abs (seedColor.g - color.g);
                
                int maxD = 15;
                if ( diff > maxD ) diff = maxD;
                engData = engData+diff;
            }
    }

    for ( int y = 1; y < height; y++ ){
        for (int  x = 0; x < width; x++ )
            if ( labeling[x+y*width] != labeling[x+(y-1)*width] )
                engSmooth = engSmooth + vertWeights[x+(y-1)*width];
        }

    for (int  y = 0; y < height; y++ ){
        for (int  x = 1; x < width; x++ )
            if ( labeling[x+y*width] != labeling[(x-1)+y*width] ){
                engSmooth = engSmooth + horizWeights[(x-1)+y*width];
            }
        }

    for ( int y = 1; y < height; y++ ){
        for (int  x = 1; x < width; x++ )
            if ( labeling[x+y*width] != labeling[x-1+(y-1)*width] )
                engSmooth = engSmooth + SQRT_2*diag1Weights[x-1+(y-1)*width];
        }

    for ( int y = 1; y < height; y++ ){
        for (int  x = 0; x < width-1; x++ )
            if ( labeling[x+y*width] != labeling[x+1+(y-1)*width] )
                engSmooth = engSmooth + SQRT_2*diag2Weights[x+1+(y-1)*width];
    }


    //printf("\nDeng %d ",engSmooth);
    return(engSmooth+engData);
}

void expandOnLabelColor(int label,int width,int height,int num_pixels,
                   vector<int> &Seeds,int numSeeds, vector<int> &labeling,
                   vector<Value> &horizWeights,vector<Value> &vertWeights,Value lambda,
                   vector<Value> &diag1Weights,vector<Value> &diag2Weights,int PATCH_SIZE,
                   vector<int> &changeMask, vector<int> &changeMaskNew,image<rgb> *I,
                   int TYPE,float variance)
{
    int seedX,seedY,startX,startY,endX,endY,numVars,blockWidth;
    getBounds(width,height,Seeds,&seedX,&seedY,&startX,&startY,&endX,&endY,label,PATCH_SIZE);

    int somethingChanged = 0;

    for ( int y = startY; y <= endY; y++ )
        for ( int x = startX; x <= endX; x++ )
            if ( changeMask[x+y*width] == 1 )
            {
                somethingChanged = 1;
                break;
            }


    if ( somethingChanged == 0)
        return;

    blockWidth = endX-startX+1;
    numVars   = (endY-startY+1)*blockWidth;


    vector<Var> variables(numVars);
    Energy<int,int,int> *e = new Energy<int,int,int>(numVars,numVars*3);

    for ( int i = 0; i < numVars; i++ )
        variables[i] = e->add_variable();

    Value LARGE_WEIGHT = lambda*NUM_COLORS*8;

    // First fix the border to old labels, except the edges of the image
    for ( int y = startY; y <= endY; y++ ){
        if ( startX != 0 )
            e->add_term1(variables[(y-startY)*blockWidth],0,LARGE_WEIGHT); // variables[0 + (y - startY)*blockWidth]
        else if ( y == startY || y == endY)
            e->add_term1(variables[(y-startY)*blockWidth],0,LARGE_WEIGHT);

        if( endX != width -1 )
            e->add_term1(variables[(endX-startX)+(y-startY)*blockWidth],0,LARGE_WEIGHT);
        else if ( y == startY || y == endY)
            e->add_term1(variables[(endX-startX)+(y-startY)*blockWidth],0,LARGE_WEIGHT);
    }

    for ( int x = startX+1; x < endX; x++){
        if ( startY != 0 )
            e->add_term1(variables[(x-startX)],0,LARGE_WEIGHT);
        if ( endY != height - 1)
            e->add_term1(variables[(x-startX)+(endY-startY)*blockWidth],0,LARGE_WEIGHT);
    }

    // add links to center of the patch for color constant superpixels
    if ( TYPE == 1 )
    {
        rgb seedColor = imRef(I,seedX,seedY);

        for ( int y = startY+1; y < endY; y++ )
            for ( int x = startX+1; x < endX; x++){
                Value E00=0,E01=0,E10=LARGE_WEIGHT,E11=0;

                if (seedX != x && seedY != y)
                    e->add_term2(variables[(x-startX)+(y-startY)*blockWidth],
                             variables[(seedX-startX)+(seedY-startY)*blockWidth],E00,E01,E10,E11);

                rgb color = imRef(I,x,y);
                int diff = abs(seedColor.r - color.r)
                        + abs(seedColor.g - color.g)
                        + abs(seedColor.b - color.b);
                
                int maxD = (int) variance*MULTIPLIER_VAR;
                if ( diff > maxD ) diff = maxD;

                int oldLabel = labeling[x+y*width];
                int oldY = Seeds[oldLabel]/width;
                int oldX = Seeds[oldLabel]-oldY*width;
                rgb oldColor = imRef(I,oldX,oldY);
                
                int oldDiff = abs(color.r - oldColor.r)
                        + abs(color.g - oldColor.g)
                        + abs(color.b - oldColor.b);
                
                if ( oldDiff > maxD ) oldDiff = maxD;

                if ( oldDiff > diff)
                     e->add_term1(variables[(x-startX)+(y-startY)*blockWidth],oldDiff-diff,0);
                else e->add_term1(variables[(x-startX)+(y-startY)*blockWidth],0,diff-oldDiff);
            }
    }


    // First set up horizontal links
    for ( int y = startY; y <= endY; y++ )
        for ( int x = startX+1; x <=endX; x++){
            int oldLabelPix       = labeling[x+y*width];
            int oldLabelNeighbPix = labeling[x-1+y*width];
            Value E00,E01,E10,E11=0;

            if ( oldLabelPix != oldLabelNeighbPix )
                E00 = horizWeights[x-1+y*width];
            else E00 = 0;
            if ( oldLabelNeighbPix != label )
                E01 = horizWeights[x-1+y*width];
            else E01 = 0;
            if ( label != oldLabelPix )
                E10 = horizWeights[x-1+y*width];
            else E10 = 0;

            e->add_term2(variables[(x-startX)-1+(y-startY)*blockWidth],variables[(x-startX)+(y-startY)*blockWidth],E00,E01,E10,E11);
        }

    // Next set up vertical links
    for ( int y = startY+1; y <= endY; y++ )
        for ( int x = startX; x <=endX; x++){
            int oldLabelPix       = labeling[x+y*width];
            int oldLabelNeighbPix = labeling[x+(y-1)*width];
            Value E00,E01,E10,E11=0;

            if ( oldLabelPix != oldLabelNeighbPix )
                E00 = vertWeights[x+(y-1)*width];
            else E00 = 0;
            if ( oldLabelNeighbPix != label )
                E01 = vertWeights[x+(y-1)*width];
            else E01 = 0;
            if ( label != oldLabelPix )
                E10 = vertWeights[x+(y-1)*width];
            else E10 = 0;


            e->add_term2(variables[(x-startX)+(y-startY-1)*blockWidth],variables[(x-startX)+(y-startY)*blockWidth],E00,E01,E10,E11);
        }

    // Next set up diagonal links
    float SQRT_2= 1/sqrt(2.0);
    for ( int y = startY+1; y <= endY; y++ )
        for ( int x = startX+1; x <=endX; x++){
            int oldLabelPix       = labeling[x+y*width];
            int oldLabelNeighbPix = labeling[x-1+(y-1)*width];
            Value E00,E01,E10,E11=0;

            if ( oldLabelPix != oldLabelNeighbPix )
                E00 = SQRT_2*diag1Weights[x-1+(y-1)*width];
            else E00 = 0;
            if ( oldLabelNeighbPix != label )
                E01 = SQRT_2*diag1Weights[x-1+(y-1)*width];
            else E01 = 0;
            if ( label != oldLabelPix )
                E10 = SQRT_2*diag1Weights[x-1+(y-1)*width];
            else E10 = 0;

            e->add_term2(variables[(x-startX)-1+(y-startY-1)*blockWidth],variables[(x-startX)+(y-startY)*blockWidth],E00,E01,E10,E11);
        }

    // More diagonal links
    for ( int y = startY+1; y <= endY; y++ )
        for ( int x = startX; x <=endX-1; x++){
            int oldLabelPix       = labeling[x+y*width];
            int oldLabelNeighbPix = labeling[(x+1)+(y-1)*width];
            Value E00,E01,E10,E11=0;

            if ( oldLabelPix != oldLabelNeighbPix )
                E00 = SQRT_2*diag2Weights[(x+1)+(y-1)*width];
            else E00 = 0;
            if ( oldLabelNeighbPix != label )
                E01 = SQRT_2*diag2Weights[(x+1)+(y-1)*width];
            else E01 = 0;
            if ( label != oldLabelPix )
                E10 = SQRT_2*diag2Weights[(x+1)+(y-1)*width];
            else E10 = 0;

            e->add_term2(variables[(x-startX+1)+(y-startY-1)*blockWidth],variables[(x-startX)+(y-startY)*blockWidth],E00,E01,E10,E11);
        }


    e->minimize();

    for ( int y = startY; y <= endY; y++ )
        for ( int x = startX; x <= endX; x++){
            if ( e->get_var(variables[(x-startX)+(y-startY)*blockWidth]) != 0 )
            {
                if ( labeling[x+y*width] != label ){
                    labeling[x+y*width] = label;
                    changeMaskNew[x+y*width] = 1;
                    changeMask[x+y*width] = 1;
                }
            }
        }


    delete e;
}

void computeWeightsColor(vector<Value> &weights,int num_pixels,int width,
                    int height,Value lambda, float variance,
                    int incrX,int incrY, image<rgb> *I,
                    int TYPE, float sigma)
{

//    float sigma = 2.0f;

    int startX=0, startY=0;

    if ( incrX != 0 ) startX  = abs(incrX);

    if (incrY != 0 ) startY = abs(incrY);

    Value smallPenalty;
    if ( TYPE == 1 )
        smallPenalty = (MULTIPLIER_VAR*variance)/8+1;
    else smallPenalty  = 1;


    for ( int y = startY; y < height; y++ )
        for ( int x = startX; x < width; x++){
            rgb color = imRef(I,x,y);
            rgb color1 = imRef(I,x+incrX,y+incrY);
            int difference = sq(abs(color.r - color1.r) + abs(color.g - color1.g) + abs(color.b - color1.b));
                weights[(x+incrX)+(y+incrY)*width] = (Value) (lambda*exp((-difference/(sigma*sq(variance))))+smallPenalty);
        }


    //image<uchar> *e = new image<uchar>(width,height);

    //for ( int y = 0; y < height; y++ )
    //    for (int  x = 0; x < width; x++ ){
    //        imRef(e,x,y) = weights[x+y*width];
    //    }

    //savePGM(e,name);

}

float  computeImageVarianceColor(image<rgb> *I,int width,int height)
{
    float v = (float) 0.0;
    int total = 0;

    for ( int y = 1; y < height; y++ )
        for ( int x = 1; x < width ; x++ ){
            rgb color = imRef(I,x,y);
            rgb color1 = imRef(I,x-1,y);
            float diff1 = abs(color.r - color1.r)
                    + abs(color.g - color1.g)
                    + abs(color.b - color1.b);
            
            color = imRef(I,x,y);
            color1 = imRef(I,x,y-1);
            float diff2 = abs(color.r - color1.r)
                    + abs(color.g - color1.g)
                    + abs(color.b - color1.b);
            
            v = v + diff1 + diff2;
            total = total + 2;
    }

    return( v/total);
}

#endif	/* SUPERPIXELS_H */
