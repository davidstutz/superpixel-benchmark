/*
 * fMOG.cpp
 *
 *  Created on: 02 Aðu 2011
 *      Author: Tuðhan Marpuç
 */


#include <iostream>
#include "cMOG.h"
#include "cv.h"

using namespace cv;
using namespace std;
//constructor
MOG::MOG(int _nMod, float _BGR, int _imHeight, int _imWidth)
{
	// learning rates
	alfaMean   = 0.01;
	alfaVar    = 1.0/5000.0;
	alfaWeight = 1.0/5000.0;

	alfaMean2   = 0.01;
	alfaVar2    = 1.0/5000.0;
	alfaWeight2 = 1.0/5000.0;

	alfaMeanInit   = 0.01;//????????
	alfaVarInit    = 0.004;//???????
	alfaWeightInit = 0.004;

	oneMinusAlfaMean       = 1 - alfaMean;
	oneMinusAlfaVar        = 1 - alfaVar;
	oneMinusAlfaWeight     = 1 - alfaWeight;
	oneMinusAlfaMeanInit   = 1 - alfaMeanInit;
	oneMinusAlfaVarInit    = 1 - alfaVarInit;
	oneMinusAlfaWeightInit = 1 - alfaWeightInit;

	// history
	history = 1.0/alfaWeight;
	nframes = 0;

	// for new mode
	lowWeight     = 0.005;
	lowWeightInit = 0.25;
	highVar       = 100;

	// variance limits
	minVar = 4;
	maxVar = 144;

	// mode number, background ratio, image sizes
	nMod     = _nMod;
	BGR      = _BGR;
	imHeight = _imHeight;
	imWidth  = _imWidth;
	foreground     = cvCreateImage(cvSize(imWidth,imHeight),8,1);
	modNumberImage = cvCreateImage(cvSize(imWidth,imHeight),8,1);
	minAcceptedWeight = 0.0045;

	// initialize mean,variance and weight
	int i,j;
	int modelLength = imHeight*imWidth*nMod;
	modelMean   = new float[modelLength];
	modelVar    = new float[modelLength];
	modelWeight = new float[modelLength];
	//init them all
	for(i = 0; i < modelLength; i++)
	{
		modelMean[i]   = 301;
		modelVar[i]    = 144;
		modelWeight[i] = 0;
	}

	//calculate Gaussian look up table
	varLength   = (int)maxVar+1;
	diffLength  = 302;
	int tableLenght = diffLength*varLength;
	float ftmp,ftmp2,ftmp3;
	gaussianLUT = new float[tableLenght];
	for(i = 0; i < diffLength;i++)
	{
		for(j = 0; j < varLength; j++)
		{
			if(j!=0)
			{
				//if(i+j*diffLength == 20881)
					//ftmp=0.0;
				ftmp  = (-1.0/2.0)*pow((float)i,2)/(float)j;
				ftmp2 = (1.0/(sqrt(2.0*CV_PI*(float)j)));
				ftmp3 = ftmp2 * exp(ftmp);
				gaussianLUT[i + j*diffLength] = ftmp3;
				//cout<<ftmp3<<endl;
			}
			else
				gaussianLUT[i+j*diffLength] = 0;
		}
	}
	//cout<<gaussianLUT[20881]<<endl;

	// for the version of 4, keeps the number of mods used for each pixel
	modNumber = new int[imHeight*imWidth];
	for(i = 0; i < imHeight*imWidth; i++)
	{
		modNumber[i] = 1;
	}

}

//update model and detect foreground pixels
void MOG::getForeGround(IplImage* img)
{
	//int testInd = 50 + imWidth*50;

	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0;
	float gprop[5], *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		modelIndEnd = modelIndStart + nMod;
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];

		//get probability of current intensity value belong to Gaussians
		maxProp = -1.0;
		for(i = modelIndStart,j=0; i < modelIndEnd; i++,j++)
		{
			gprop[j] = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];
			if(gprop[j] > maxProp) {maxProp = gprop[j];maxPropInd=i;}
		}

		//check match
		dist = modelMean[maxPropInd] - (float)gpixel;
		if(pow(dist,2) < 12.25*(modelVar[maxPropInd] +12))
				match = 1;

		//foreground decision and model updates
		if(match)
		{
			//calculate weight over std
			for(j=0,i = modelIndStart; i < modelIndEnd;i++,j++)
				WOS[j] = modelWeight[i]/sqrt(modelVar[i]);


			// sort WOS in descending order
			bool swapped = true;
			j = 0;
			int ind[] = {0,1,2,3,4};
			while (swapped)
			{
				swapped = false;
				j++;
				for (int i = 0; i < nMod - j; i++)
				{
					  if (WOS[i] < WOS[i + 1])
					  {
							ftmp = WOS[i];
							WOS[i] = WOS[i + 1];
							WOS[i + 1] = ftmp;

							ntmp = ind[i];
							ind[i] = ind[i + 1];
							ind[i + 1] = ntmp;

							swapped = true;
					  }
				}
			}

			//foreground decision
			currentRatio = 0;
			for(i = 0; i < nMod ;i++)
			{
				modelInddum = modelIndStart + ind[i];
				currentRatio = currentRatio + modelWeight[modelInddum];
				if (maxPropInd == modelInddum)
				{
					foreground->imageData[pixelInd] = 0;
					break;
				}
				if(currentRatio > BGR)
				{
					foreground->imageData[pixelInd] = 255;
					break;
				}
			}



			//update of gaussians in case of match
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] = modelWeight[i] * oneMinusAlfaWeight;

			modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeight;
			modelMean[maxPropInd]   = oneMinusAlfaMean*modelMean[maxPropInd] + alfaMean*(float)gpixel;
			modelVar[maxPropInd]    = oneMinusAlfaVar*modelVar[maxPropInd] + alfaVar*pow((float)gpixel-modelMean[maxPropInd],2);

			if(modelVar[maxPropInd] > maxVar)
				modelVar[maxPropInd] = maxVar;

			if(modelVar[maxPropInd] < minVar)
				modelVar[maxPropInd] = minVar;
		}
		else // not match
		{
			// if not match it is foreground
			foreground->imageData[pixelInd] = 255;
			//find minimum weighted mode and replace it
			minWeight = modelWeight[modelIndStart]; minWeightInd = modelIndStart;
			for(i = modelIndStart+1; i < modelIndEnd; i++)
				if(modelWeight[i] < minWeight)
				{	minWeight = modelWeight[i];	minWeightInd = i;	}

			modelWeight[minWeightInd] = lowWeight;
			modelMean[minWeightInd]   = (float)gpixel;
			modelVar[minWeightInd]    = highVar;

			//normalize weight
			weightSum = 0;
			for(i = modelIndStart; i < modelIndEnd; i++)
				weightSum += modelWeight[i];
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] /= weightSum;
		}

		/*if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			for(i=0; i < 5; i++)
				cout<<gprop[i]<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}*/


	}
}





void MOG::getForeGround2(IplImage* img)
{
	//int testInd = 160 + imWidth*424;

	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0;
	float gprop[5], *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		modelIndEnd = modelIndStart + nMod;
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];

		//get probability of current intensity value belong to Gaussians
		maxProp = -1.0;
		for(i = modelIndStart,j=0; i < modelIndEnd; i++,j++)
		{
			//ntmp = (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i];
			//ftmp = gaussianLUT[ ntmp ];
			gprop[j] = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];
			//ftmp = (-1.0/2.0)*pow((float)gpixel-modelMean[i],2)/modelVar[i];
			//gprop[j] = modelWeight[i]*(1/(sqrt(2*CV_PI*modelVar[i]))) * exp(ftmp);
			if(gprop[j] > maxProp) {maxProp = gprop[j];maxPropInd=i;}
		}

		//check match
		dist = modelMean[maxPropInd] - (float)gpixel;
		if(pow(dist,2) < 12.25*(modelVar[maxPropInd] + 12))
				match = 1;

		//foreground decision and model updates
		if(match)
		{
			//calculate weight over std
			for(j=0,i = modelIndStart; i < modelIndEnd;i++,j++)
				WOS[j] = modelWeight[i];///sqrt(modelVar[i]);


			// sort WOS in descending order
			bool swapped = true;
			j = 0;
			int ind[] = {0,1,2,3,4};
			while (swapped)
			{
				swapped = false;
				j++;
				for (int i = 0; i < nMod - j; i++)
				{
					  if (WOS[i] < WOS[i + 1])
					  {
							ftmp = WOS[i];
							WOS[i] = WOS[i + 1];
							WOS[i + 1] = ftmp;

							ntmp = ind[i];
							ind[i] = ind[i + 1];
							ind[i + 1] = ntmp;

							swapped = true;
					  }
				}
			}

			//foreground decision
			currentRatio = 0;
			for(i = 0; i < nMod ;i++)
			{
				modelInddum = modelIndStart + ind[i];
				currentRatio = currentRatio + modelWeight[modelInddum];
				if (maxPropInd == modelInddum)
				{
					foreground->imageData[pixelInd] = 0;
					break;
				}
				if(currentRatio > BGR)
				{
					foreground->imageData[pixelInd] = 255;
					break;
				}
			}



			//update of gaussians in case of match
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] = modelWeight[i] * oneMinusAlfaWeight;

			modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeight;
			modelMean[maxPropInd]   = oneMinusAlfaMean*modelMean[maxPropInd] + alfaMean*(float)gpixel;
			modelVar[maxPropInd]    = oneMinusAlfaVar*modelVar[maxPropInd] + alfaVar*pow((float)gpixel-modelMean[maxPropInd],2);

			if(modelVar[maxPropInd] > maxVar)
				modelVar[maxPropInd] = maxVar;

			if(modelVar[maxPropInd] < minVar)
				modelVar[maxPropInd] = minVar;
		}
		else // not match
		{
			// if not match it is foreground
			foreground->imageData[pixelInd] = 255;
			//find minimum weighted mode and replace it
			minWeight = modelWeight[modelIndStart]; minWeightInd = modelIndStart;
			for(i = modelIndStart+1; i < modelIndEnd; i++)
				if(modelWeight[i] < minWeight)
				{	minWeight = modelWeight[i];	minWeightInd = i;	}

			modelWeight[minWeightInd] = lowWeight;
			modelMean[minWeightInd]   = (float)gpixel;
			modelVar[minWeightInd]    = highVar;

			//normalize weight
			weightSum = 0;
			for(i = modelIndStart; i < modelIndEnd; i++)
				weightSum += modelWeight[i];
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] /= weightSum;
		}

		/*if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			for(i=0; i < 5; i++)
				cout<<gprop[i]<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}*/


	}
}





void MOG::getForeGround3(IplImage* img)
{
	//int testInd = 160 + imWidth*424;

	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0;
	float gprop[5], *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		modelIndEnd = modelIndStart + nMod;
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];

		//get probability of current intensity value belong to Gaussians
		maxProp = -1.0;
		for(i = modelIndStart,j=0; i < modelIndEnd; i++,j++)
		{
			//ntmp = (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i];
			//ftmp = gaussianLUT[ ntmp ];
			gprop[j] = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];
			//ftmp = (-1.0/2.0)*pow((float)gpixel-modelMean[i],2)/modelVar[i];
			//gprop[j] = modelWeight[i]*(1/(sqrt(2*CV_PI*modelVar[i]))) * exp(ftmp);
			if(gprop[j] > maxProp) {maxProp = gprop[j];maxPropInd=i;}
		}

		//check match
		dist = modelMean[maxPropInd] - (float)gpixel;
		if(pow(dist,2) < 12.25*(modelVar[maxPropInd] + 12))
				match = 1;

		//foreground decision and model updates
		if(match)
		{


			//foreground decision
			currentRatio = 0;
			for(i = modelIndStart; i < modelIndEnd ;i++)
			{
				currentRatio = currentRatio + modelWeight[i];
				if (maxPropInd == i)
				{
					foreground->imageData[pixelInd] = 0;
					break;
				}
				if(currentRatio > BGR)
				{
					foreground->imageData[pixelInd] = 255;
					break;
				}
			}



			//update of gaussians in case of match
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] = modelWeight[i] * oneMinusAlfaWeight;


			modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeight;
			modelMean[maxPropInd]   = oneMinusAlfaMean*modelMean[maxPropInd] + alfaMean*(float)gpixel;
			modelVar[maxPropInd]    = oneMinusAlfaVar*modelVar[maxPropInd] + alfaVar*pow((float)gpixel-modelMean[maxPropInd],2);

			if(modelVar[maxPropInd] > maxVar)
				modelVar[maxPropInd] = maxVar;

			if(modelVar[maxPropInd] < minVar)
				modelVar[maxPropInd] = minVar;
			//keep weight order
			if(maxPropInd != modelIndStart)
			{
				if(modelWeight[maxPropInd] > modelWeight[maxPropInd-1])
				{
					ftmp = modelWeight[maxPropInd];
					modelWeight[maxPropInd]   = modelWeight[maxPropInd-1];
					modelWeight[maxPropInd-1] = ftmp;

					ftmp = modelMean[maxPropInd];
					modelMean[maxPropInd]   = modelMean[maxPropInd-1];
					modelMean[maxPropInd-1] = ftmp;

					ftmp = modelVar[maxPropInd];
					modelVar[maxPropInd]   = modelVar[maxPropInd-1];
					modelVar[maxPropInd-1] = ftmp;
				}
			}
		}
		else // not match
		{
			// if not match it is foreground
			foreground->imageData[pixelInd] = 255;
			//find minimum weighted mode and replace it
			minWeight = modelWeight[modelIndStart]; minWeightInd = modelIndStart;
			for(i = modelIndStart+1; i < modelIndEnd; i++)
				if(modelWeight[i] < minWeight)
				{	minWeight = modelWeight[i];	minWeightInd = i;	}

			modelWeight[minWeightInd] = lowWeight;
			modelMean[minWeightInd]   = (float)gpixel;
			modelVar[minWeightInd]    = highVar;

			//normalize weight
			weightSum = 0;
			for(i = modelIndStart; i < modelIndEnd; i++)
				weightSum += modelWeight[i];
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] /= weightSum;
		}

		/*if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			for(i=0; i < 5; i++)
				cout<<gprop[i]<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}*/


	}
}





void MOG::getForeGround4(IplImage* img)
{
	//int testInd = 518 + imWidth*18;

	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0,nModVary;
	float gprop, *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		nModVary = modNumber[pixelInd];
		modelIndEnd = modelIndStart + nModVary;
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];

		//get probability of current intensity value belong to Gaussians
		maxProp = -1.0;
		for(i = modelIndStart,j=0; i < modelIndEnd; i++,j++)
		{
			gprop = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];
			if(gprop > maxProp) {maxProp = gprop;maxPropInd=i;}
		}

		//check match
		dist = modelMean[maxPropInd] - (float)gpixel;
		if(/*pow(dist,2)*/dist*dist < 16*(modelVar[maxPropInd] ))
				match = 1;

		//foreground decision and model updates
		if(match)
		{


			//foreground decision
			currentRatio = 0;
			for(i = modelIndStart; i < modelIndEnd ;i++)
			{
				currentRatio = currentRatio + modelWeight[i];
				if (maxPropInd == i)
				{
					foreground->imageData[pixelInd] = 0;
					break;
				}
				if(currentRatio > BGR)
				{
					foreground->imageData[pixelInd] = 255;
					break;
				}
			}



			//update of gaussians in case of match
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] = modelWeight[i] * oneMinusAlfaWeight;


			modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeight;
			modelMean[maxPropInd]   = oneMinusAlfaMean*modelMean[maxPropInd] + alfaMean*(float)gpixel;
			ftmp = (float)gpixel-modelMean[maxPropInd];
			modelVar[maxPropInd]    = oneMinusAlfaVar*modelVar[maxPropInd] + alfaVar*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

			if(modelVar[maxPropInd] > maxVar)
				modelVar[maxPropInd] = maxVar-4;

			if(modelVar[maxPropInd] < minVar)
				modelVar[maxPropInd] = minVar+2;
			//keep weight order
			if(maxPropInd != modelIndStart)
			{
				if(modelWeight[maxPropInd] > modelWeight[maxPropInd-1])
				{
					ftmp = modelWeight[maxPropInd];
					modelWeight[maxPropInd]   = modelWeight[maxPropInd-1];
					modelWeight[maxPropInd-1] = ftmp;

					ftmp = modelMean[maxPropInd];
					modelMean[maxPropInd]   = modelMean[maxPropInd-1];
					modelMean[maxPropInd-1] = ftmp;

					ftmp = modelVar[maxPropInd];
					modelVar[maxPropInd]   = modelVar[maxPropInd-1];
					modelVar[maxPropInd-1] = ftmp;
				}
			}
			//eliminate low weighted mod
			if(modelWeight[modelIndEnd-1] < minAcceptedWeight)
				modNumber[pixelInd] -=1;

		}
		else // not match
		{
			// if not match it is foreground
			foreground->imageData[pixelInd] = 255;
			//check free slot
			if(nModVary != nMod)
			{
				modNumber[pixelInd] += 1;
				modelWeight[modelIndEnd] = lowWeight;
				modelMean[modelIndEnd]   = (float)gpixel;
				modelVar[modelIndEnd]    = highVar;
				modelIndEnd += 1;
			}
			else
			{
				ntmp = modelIndEnd -1;
				modelWeight[ntmp] = lowWeight;
				modelMean[ntmp]   = (float)gpixel;
				modelVar[ntmp]    = highVar;
			}
			//normalize weight
			weightSum = 0;
			for(i = modelIndStart; i < modelIndEnd; i++)
				weightSum += modelWeight[i];
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] /= weightSum;
		}

		/*if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			for(i=0; i < modelIndEnd-modelIndStart; i++)
				cout<<gprop[i]<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}*/
		//modNumberImage->imageData[pixelInd] = (unsigned char)(modNumber[pixelInd]-1)*63;

	}
}





void MOG::getForeGround5(IplImage* img)
{
	int fps = 25;
	nframes += 25/fps;
	if(nframes>1e4)
		nframes = history+1;

	if(nframes*2 > history)
		{alfaWeight2 = 1.0/history;    	oneMinusAlfaWeight=1-alfaWeight;}
	else
		{alfaWeight2 = 1.0/(nframes*2);	oneMinusAlfaWeight=1-alfaWeight;}

	if (1)
	{
		alfaVar    = 1.0 - exp(log(1-alfaVar2)*25/fps);
		alfaWeight = 1.0 - exp(log(1-alfaWeight2)*25/fps);
		alfaMean   = 1.0 - exp(log(1-alfaMean2)*25/fps);

		oneMinusAlfaVar    = 1 - alfaVar;
		oneMinusAlfaWeight = 1 - alfaWeight;
		oneMinusAlfaMean   = 1 - alfaMean;
	}


	int testInd = 522 + imWidth*85;

	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0,nModVary;
	float gprop, *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		nModVary = modNumber[pixelInd];
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];
		if(nModVary == 1)
		{
			//get probability of current intensity value belong to Gaussians
			maxPropInd = modelIndStart;modelIndEnd = modelIndStart + nModVary;
			//check match
			dist = modelMean[maxPropInd] - (float)gpixel;
			if(/*pow(dist,2)*/dist*dist < 49.0*(modelVar[maxPropInd] ))
					match = 1;

			//foreground decision and model updates
			if(match)
			{
				//foreground decision
				foreground->imageData[pixelInd] = 0;

				//update of gaussians in case of match
				modelMean[maxPropInd]   = oneMinusAlfaMean*modelMean[maxPropInd] + alfaMean*(float)gpixel;
				ftmp = (float)gpixel-modelMean[maxPropInd];
				modelVar[maxPropInd]    = oneMinusAlfaVar*modelVar[maxPropInd] + alfaVar*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

				// check variance limit
				if(modelVar[maxPropInd] > maxVar)
					modelVar[maxPropInd] = maxVar-4;

				if(modelVar[maxPropInd] < minVar)
					modelVar[maxPropInd] = minVar+2;
			}
			else // not match
			{
				// if not match it is foreground
				foreground->imageData[pixelInd] = 255;

				//free slot already available
				modNumber[pixelInd] += 1;
				ntmp = modelIndStart + 1;
				modelWeight[ntmp] = lowWeight;
				modelMean[ntmp]   = (float)gpixel;
				modelVar[ntmp]    = highVar;

				//normalize weight
				weightSum = modelWeight[modelIndStart] + lowWeight;
				for(i = modelIndStart; i <= ntmp; i++)
					modelWeight[i] /= weightSum;
			}
		}
		else
		{
			// calculate end index
			modelIndEnd = modelIndStart + nModVary;
			//get probability of current intensity value belong to Gaussians
			maxProp = -1.0;
			for(i = modelIndStart; i < modelIndEnd; i++)
			{
				gprop = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];
				if(gprop > maxProp) {maxProp = gprop;maxPropInd=i;}
			}

			//check match
			dist = modelMean[maxPropInd] - (float)gpixel;
			if(/*pow(dist,2)*/dist*dist < 49*(modelVar[maxPropInd] ))
					match = 1;

			//foreground decision and model updates
			if(match)
			{
				//foreground decision
				currentRatio = 0;
				for(i = modelIndStart; i < maxPropInd ;i++)
				{
					currentRatio = currentRatio + modelWeight[i];
				}

				if(currentRatio > BGR)
					foreground->imageData[pixelInd] = 255;
				else
					foreground->imageData[pixelInd] = 0;


				//update of gaussians in case of match
				for(i = modelIndStart; i < modelIndEnd; i++)
					modelWeight[i] = modelWeight[i] * oneMinusAlfaWeight;


				modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeight;
				modelMean[maxPropInd]   = oneMinusAlfaMean*modelMean[maxPropInd] + alfaMean*(float)gpixel;
				ftmp = (float)gpixel-modelMean[maxPropInd];
				modelVar[maxPropInd]    = oneMinusAlfaVar*modelVar[maxPropInd] + alfaVar*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

				if(modelVar[maxPropInd] > maxVar)
					modelVar[maxPropInd] = maxVar-4;

				if(modelVar[maxPropInd] < minVar)
					modelVar[maxPropInd] = minVar+2;
				//keep weight order
				if(maxPropInd != modelIndStart)
				{
					if(modelWeight[maxPropInd] > modelWeight[maxPropInd-1])
					{
						ftmp = modelWeight[maxPropInd];
						modelWeight[maxPropInd]   = modelWeight[maxPropInd-1];
						modelWeight[maxPropInd-1] = ftmp;

						ftmp = modelMean[maxPropInd];
						modelMean[maxPropInd]   = modelMean[maxPropInd-1];
						modelMean[maxPropInd-1] = ftmp;

						ftmp = modelVar[maxPropInd];
						modelVar[maxPropInd]   = modelVar[maxPropInd-1];
						modelVar[maxPropInd-1] = ftmp;
					}
				}
				//eliminate low weighted mod
				if(modelWeight[modelIndEnd-1] < minAcceptedWeight)
					modNumber[pixelInd] -=1;

			}
			else // not match
			{
				// if not match it is foreground
				foreground->imageData[pixelInd] = 255;
				//check free slot
				if(nModVary != nMod)
				{
					modNumber[pixelInd] += 1;
					modelWeight[modelIndEnd] = lowWeight;
					modelMean[modelIndEnd]   = (float)gpixel;
					modelVar[modelIndEnd]    = highVar;
					modelIndEnd += 1;
				}
				else
				{
					ntmp = modelIndEnd -1;
					modelWeight[ntmp] = lowWeight;
					modelMean[ntmp]   = (float)gpixel;
					modelVar[ntmp]    = highVar;
				}
				//normalize weight
				weightSum = 0;
				for(i = modelIndStart; i < modelIndEnd; i++)
					weightSum += modelWeight[i];
				for(i = modelIndStart; i < modelIndEnd; i++)
					modelWeight[i] /= weightSum;
			}
		}
/*
		if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			//for(i=0; i < modelIndEnd-modelIndStart; i++)
			if(foreground->imageData[pixelInd] == 255)
				cout<<"255"<<endl;
			else
				cout<<"0"<<endl;
			cout<<alfaWeight<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}
*/
		/*
		if(match)
			modNumberImage->imageData[pixelInd] = (unsigned char)(maxPropInd-modelIndStart)*63;//(modNumber[pixelInd]-1)*63;
		else
		*/
			modNumberImage->imageData[pixelInd] = (unsigned char)(modNumber[pixelInd]-1)*63;

	}
}





void MOG::init5(IplImage* img)
{
	nframes++;
	int testInd = 522 + imWidth*85;

	int i,j,ntmp, modelIndStart, modelIndEnd, pixelInd, modelLength, maxPropInd=0, nModVary;
	float gprop, dist, currentRatio, maxProp, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		nModVary = modNumber[pixelInd];
		//reset match and get the pixel value
		match = 0;
		//cout << pixelInd << " - " << (int)(img->imageData[pixelInd]) << "\n";
		gpixel = (unsigned char)img->imageData[pixelInd];
		if(nModVary == 1)
		{
			//get probability of current intensity value belong to Gaussians
			maxPropInd = modelIndStart;modelIndEnd = modelIndStart + nModVary;
			//check match
			dist = modelMean[maxPropInd] - (float)gpixel;
			//if(/*pow(dist,2)*/dist*dist < 16*(modelVar[maxPropInd] ))
			if(/*pow(dist,2)*/dist*dist < 12.25*(modelVar[maxPropInd] + 12 ))
					match = 1;

			//foreground decision and model updates
			foreground->imageData[pixelInd] = 0;
			if(match)
			{
				//foreground decision
				//foreground->imageData[pixelInd] = 0;

				//update of gaussians in case of match
				modelWeight[maxPropInd]   +=1;
				modelMean[maxPropInd]   = oneMinusAlfaMeanInit*modelMean[maxPropInd] + alfaMeanInit*(float)gpixel;
				ftmp = (float)gpixel-modelMean[maxPropInd];
				modelVar[maxPropInd]    = oneMinusAlfaVarInit*modelVar[maxPropInd] + alfaVarInit*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

				// check variance limit
				if(modelVar[maxPropInd] > maxVar)
					modelVar[maxPropInd] = maxVar-4;

				if(modelVar[maxPropInd] < minVar)
					modelVar[maxPropInd] = minVar+2;
			}
			else // not match
			{
				// if not match it is foreground
				//foreground->imageData[pixelInd] = 255;

				//free slot already available
				modNumber[pixelInd] += 1;
				ntmp = modelIndStart + 1;
				modelWeight[ntmp] = 1;
				modelMean[ntmp]   = (float)gpixel;
				modelVar[ntmp]    = highVar;

				//normalize weight
				//weightSum = modelWeight[modelIndStart] + lowWeight;
				//for(i = modelIndStart; i <= ntmp; i++)
				//	modelWeight[i] /= weightSum;
			}
		}
		else
		{
			// calculate end index
			modelIndEnd = modelIndStart + nModVary;
			//get probability of current intensity value belong to Gaussians
			maxProp = -1.0;
			for(i = modelIndStart; i < modelIndEnd; i++)
			{
				gprop = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];
				if(gprop > maxProp) {maxProp = gprop;maxPropInd=i;}
			}

			//check match
			dist = modelMean[maxPropInd] - (float)gpixel;
			if(/*pow(dist,2)*/dist*dist < 12.25*(modelVar[maxPropInd] + 12 ))
					match = 1;

			//foreground decision and model updates
			if(match)
			{
				//foreground decision
				/*currentRatio = 0;
				for(i = modelIndStart; i < maxPropInd ;i++)
				{
					currentRatio = currentRatio + modelWeight[i];
				}

				if(currentRatio > BGR)
					foreground->imageData[pixelInd] = 255;
				else
					foreground->imageData[pixelInd] = 0;*/


				//update of gaussians in case of match
				//for(i = modelIndStart; i < modelIndEnd; i++)
				//	modelWeight[i] = modelWeight[i] * oneMinusAlfaWeightInit;


				modelWeight[maxPropInd] +=1;// modelWeight[maxPropInd] + alfaWeightInit;
				modelMean[maxPropInd]   = oneMinusAlfaMeanInit*modelMean[maxPropInd] + alfaMeanInit*(float)gpixel;
				ftmp = (float)gpixel-modelMean[maxPropInd];
				modelVar[maxPropInd]    = oneMinusAlfaVarInit*modelVar[maxPropInd] + alfaVarInit*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

				if(modelVar[maxPropInd] > maxVar)
					modelVar[maxPropInd] = maxVar-4;

				if(modelVar[maxPropInd] < minVar)
					modelVar[maxPropInd] = minVar+2;
				//keep weight order
				if(maxPropInd != modelIndStart)
				{
					if(modelWeight[maxPropInd] > modelWeight[maxPropInd-1])
					{
						ftmp = modelWeight[maxPropInd];
						modelWeight[maxPropInd]   = modelWeight[maxPropInd-1];
						modelWeight[maxPropInd-1] = ftmp;

						ftmp = modelMean[maxPropInd];
						modelMean[maxPropInd]   = modelMean[maxPropInd-1];
						modelMean[maxPropInd-1] = ftmp;

						ftmp = modelVar[maxPropInd];
						modelVar[maxPropInd]   = modelVar[maxPropInd-1];
						modelVar[maxPropInd-1] = ftmp;
					}
				}
				//eliminate low weighted mod
				if(modelWeight[modelIndEnd-1] == 0)
					modNumber[pixelInd] -=1;

			}
			else // not match
			{
				// if not match it is foreground
				//foreground->imageData[pixelInd] = 255;
				//check free slot
				if(nModVary != nMod)
				{
					modNumber[pixelInd] += 1;
					modelWeight[modelIndEnd] = 1;
					modelMean[modelIndEnd]   = (float)gpixel;
					modelVar[modelIndEnd]    = highVar;
					modelIndEnd += 1;
				}
				else
				{
					ntmp = modelIndEnd -1;
					modelWeight[ntmp] = 1;
					modelMean[ntmp]   = (float)gpixel;
					modelVar[ntmp]    = highVar;
				}
				//normalize weight
				/*weightSum = 0;
				for(i = modelIndStart; i < modelIndEnd; i++)
					weightSum += modelWeight[i];
				for(i = modelIndStart; i < modelIndEnd; i++)
					modelWeight[i] /= weightSum;*/
			}
		}
/*
		if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			//for(i=0; i < modelIndEnd-modelIndStart; i++)
			unsigned int fg = (unsigned int)foreground->imageData[pixelInd];
				cout<<alfaWeight<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}
*/
		/*
		if(match)
			modNumberImage->imageData[pixelInd] = (unsigned char)(maxPropInd-modelIndStart)*63;//(modNumber[pixelInd]-1)*63;
		else*/
			modNumberImage->imageData[pixelInd] = (unsigned char)(modNumber[pixelInd]-1)*63;

	}
}





void MOG::getForeGround6(IplImage* img)
{
	nframes++;
	if(nframes>1e9)
		nframes = history+1;
	if(nframes*2 > history)
		{alfaWeight = 1.0/history;oneMinusAlfaWeight=1-alfaWeight;}
	else
		{alfaWeight = 1.0/(nframes*2);oneMinusAlfaWeight=1-alfaWeight;}

	int testInd = 103 + imWidth*372;

	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0,nModVary;
	float gprop, *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		nModVary = modNumber[pixelInd];
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];
		if(nModVary == 1)
		{
			//get probability of current intensity value belong to Gaussians
			maxPropInd = modelIndStart;modelIndEnd = modelIndStart + nModVary;
			//check match
			dist = modelMean[maxPropInd] - (float)gpixel;
			//if(/*pow(dist,2)*/dist*dist < 16*(modelVar[maxPropInd] ))
			if(/*pow(dist,2)*/dist*dist < 12.25*(modelVar[maxPropInd] + 12 ))
					match = 1;

			//foreground decision and model updates
			if(match)
			{
				//foreground decision
				foreground->imageData[pixelInd] = 0;

				//update of gaussians in case of match
				modelMean[maxPropInd]   = oneMinusAlfaMean*modelMean[maxPropInd] + alfaMean*(float)gpixel;
				ftmp = (float)gpixel-modelMean[maxPropInd];
				modelVar[maxPropInd]    = oneMinusAlfaVar*modelVar[maxPropInd] + alfaVar*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

				// check variance limit
				if(modelVar[maxPropInd] > maxVar)
					modelVar[maxPropInd] = maxVar-4;

				if(modelVar[maxPropInd] < minVar)
					modelVar[maxPropInd] = minVar+2;
			}
			else // not match
			{
				// if not match it is foreground
				foreground->imageData[pixelInd] = 255;

				//free slot already available
				modNumber[pixelInd] += 1;
				ntmp = modelIndStart + 1;
				modelWeight[ntmp] = lowWeight;
				modelMean[ntmp]   = (float)gpixel;
				modelVar[ntmp]    = highVar;

				//normalize weight
				weightSum = modelWeight[modelIndStart] + lowWeight;
				for(i = modelIndStart; i <= ntmp; i++)
					modelWeight[i] /= weightSum;
			}
		}
		else
		{
			// calculate end index
			modelIndEnd = modelIndStart + nModVary;

			//check match
			for(i = modelIndStart; i < modelIndEnd; i++)
			{
				dist = modelMean[i] - (float)gpixel;
				if(dist*dist < 12.25*(modelVar[i] + 12 ))
						{match = 1;maxPropInd=i;break;}
			}

			//foreground decision and model updates
			if(match)
			{
				//foreground decision
				currentRatio = 0;
				for(i = modelIndStart; i < maxPropInd ;i++)
				{
					currentRatio = currentRatio + modelWeight[i];
				}

				if(currentRatio > BGR)
					foreground->imageData[pixelInd] = 255;
				else
					foreground->imageData[pixelInd] = 0;


				//update of gaussians in case of match
				for(i = modelIndStart; i < modelIndEnd; i++)
					modelWeight[i] = modelWeight[i] * oneMinusAlfaWeight;


				modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeight;
				modelMean[maxPropInd]   = oneMinusAlfaMean*modelMean[maxPropInd] + alfaMean*(float)gpixel;
				ftmp = (float)gpixel-modelMean[maxPropInd];
				modelVar[maxPropInd]    = oneMinusAlfaVar*modelVar[maxPropInd] + alfaVar*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

				if(modelVar[maxPropInd] > maxVar)
					modelVar[maxPropInd] = maxVar-4;

				if(modelVar[maxPropInd] < minVar)
					modelVar[maxPropInd] = minVar+2;
				//keep weight order
				if(maxPropInd != modelIndStart)
				{
					if(modelWeight[maxPropInd] > modelWeight[maxPropInd-1])
					{
						ftmp = modelWeight[maxPropInd];
						modelWeight[maxPropInd]   = modelWeight[maxPropInd-1];
						modelWeight[maxPropInd-1] = ftmp;

						ftmp = modelMean[maxPropInd];
						modelMean[maxPropInd]   = modelMean[maxPropInd-1];
						modelMean[maxPropInd-1] = ftmp;

						ftmp = modelVar[maxPropInd];
						modelVar[maxPropInd]   = modelVar[maxPropInd-1];
						modelVar[maxPropInd-1] = ftmp;
					}
				}
				//eliminate low weighted mod
				if(modelWeight[modelIndEnd-1] < minAcceptedWeight)
					modNumber[pixelInd] -=1;

			}
			else // not match
			{
				// if not match it is foreground
				foreground->imageData[pixelInd] = 255;
				//check free slot
				if(nModVary != nMod)
				{
					modNumber[pixelInd] += 1;
					modelWeight[modelIndEnd] = lowWeight;
					modelMean[modelIndEnd]   = (float)gpixel;
					modelVar[modelIndEnd]    = highVar;
					modelIndEnd += 1;
				}
				else
				{
					ntmp = modelIndEnd -1;
					modelWeight[ntmp] = lowWeight;
					modelMean[ntmp]   = (float)gpixel;
					modelVar[ntmp]    = highVar;
				}
				//normalize weight
				weightSum = 0;
				for(i = modelIndStart; i < modelIndEnd; i++)
					weightSum += modelWeight[i];
				for(i = modelIndStart; i < modelIndEnd; i++)
					modelWeight[i] /= weightSum;
			}
		}

		/*if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			//for(i=0; i < modelIndEnd-modelIndStart; i++)
			if(foreground->imageData[pixelInd] == 255)
				cout<<"255"<<endl;
			else
				cout<<"0"<<endl;
			cout<<alfaWeight<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}
*/
		/*
		if(match)
			modNumberImage->imageData[pixelInd] = (unsigned char)(maxPropInd-modelIndStart)*63;//(modNumber[pixelInd]-1)*63;
		else
		*/
			modNumberImage->imageData[pixelInd] = (unsigned char)(modNumber[pixelInd]-1)*63;

	}
}





void MOG::init6(IplImage* img)
{
	nframes++;
	int testInd = 103 + imWidth*372;

	int i,j,ntmp, modelIndStart, modelIndEnd, pixelInd, modelLength, maxPropInd=0, nModVary;
	float gprop, dist, currentRatio, maxProp, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		nModVary = modNumber[pixelInd];
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];
		if(nModVary == 1)
		{
			//get probability of current intensity value belong to Gaussians
			maxPropInd = modelIndStart;modelIndEnd = modelIndStart + nModVary;
			//check match
			dist = modelMean[maxPropInd] - (float)gpixel;
			if(/*pow(dist,2)*/dist*dist < 12.25*(modelVar[maxPropInd] + 12 ))
					match = 1;

			//foreground decision and model updates
			foreground->imageData[pixelInd] = 0;
			if(match)
			{
				//foreground decision
				//foreground->imageData[pixelInd] = 0;

				//update of gaussians in case of match
				modelWeight[maxPropInd]   +=1;
				modelMean[maxPropInd]   = oneMinusAlfaMeanInit*modelMean[maxPropInd] + alfaMeanInit*(float)gpixel;
				ftmp = (float)gpixel-modelMean[maxPropInd];
				modelVar[maxPropInd]    = oneMinusAlfaVarInit*modelVar[maxPropInd] + alfaVarInit*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

				// check variance limit
				if(modelVar[maxPropInd] > maxVar)
					modelVar[maxPropInd] = maxVar-4;

				if(modelVar[maxPropInd] < minVar)
					modelVar[maxPropInd] = minVar+2;
			}
			else // not match
			{
				// if not match it is foreground
				//foreground->imageData[pixelInd] = 255;

				//free slot already available
				modNumber[pixelInd] += 1;
				ntmp = modelIndStart + 1;
				modelWeight[ntmp] = 1;
				modelMean[ntmp]   = (float)gpixel;
				modelVar[ntmp]    = highVar;

				//normalize weight
				//weightSum = modelWeight[modelIndStart] + lowWeight;
				//for(i = modelIndStart; i <= ntmp; i++)
				//	modelWeight[i] /= weightSum;
			}
		}
		else
		{
			// calculate end index
			modelIndEnd = modelIndStart + nModVary;

			//check match
			for(i = modelIndStart; i < modelIndEnd; i++)
			{
				dist = modelMean[i] - (float)gpixel;
				if(dist*dist < 12.25*(modelVar[i] + 12 ))
						{match = 1;maxPropInd=i;break;}
			}




			//foreground decision and model updates
			if(match)
			{
				//foreground decision
				/*currentRatio = 0;
				for(i = modelIndStart; i < maxPropInd ;i++)
				{
					currentRatio = currentRatio + modelWeight[i];
				}

				if(currentRatio > BGR)
					foreground->imageData[pixelInd] = 255;
				else
					foreground->imageData[pixelInd] = 0;*/


				//update of gaussians in case of match
				//for(i = modelIndStart; i < modelIndEnd; i++)
				//	modelWeight[i] = modelWeight[i] * oneMinusAlfaWeightInit;


				modelWeight[maxPropInd] +=1;// modelWeight[maxPropInd] + alfaWeightInit;
				modelMean[maxPropInd]   = oneMinusAlfaMeanInit*modelMean[maxPropInd] + alfaMeanInit*(float)gpixel;
				ftmp = (float)gpixel-modelMean[maxPropInd];
				modelVar[maxPropInd]    = oneMinusAlfaVarInit*modelVar[maxPropInd] + alfaVarInit*ftmp*ftmp;//pow((float)gpixel-modelMean[maxPropInd],2);

				if(modelVar[maxPropInd] > maxVar)
					modelVar[maxPropInd] = maxVar-4;

				if(modelVar[maxPropInd] < minVar)
					modelVar[maxPropInd] = minVar+2;
				//keep weight order
				if(maxPropInd != modelIndStart)
				{
					if(modelWeight[maxPropInd] > modelWeight[maxPropInd-1])
					{
						ftmp = modelWeight[maxPropInd];
						modelWeight[maxPropInd]   = modelWeight[maxPropInd-1];
						modelWeight[maxPropInd-1] = ftmp;

						ftmp = modelMean[maxPropInd];
						modelMean[maxPropInd]   = modelMean[maxPropInd-1];
						modelMean[maxPropInd-1] = ftmp;

						ftmp = modelVar[maxPropInd];
						modelVar[maxPropInd]   = modelVar[maxPropInd-1];
						modelVar[maxPropInd-1] = ftmp;
					}
				}
				//eliminate low weighted mod
				if(modelWeight[modelIndEnd-1] == 0)
					modNumber[pixelInd] -=1;

			}
			else // not match
			{
				// if not match it is foreground
				//foreground->imageData[pixelInd] = 255;
				//check free slot
				if(nModVary != nMod)
				{
					modNumber[pixelInd] += 1;
					modelWeight[modelIndEnd] = 1;
					modelMean[modelIndEnd]   = (float)gpixel;
					modelVar[modelIndEnd]    = highVar;
					modelIndEnd += 1;
				}
				else
				{
					ntmp = modelIndEnd -1;
					modelWeight[ntmp] = 1;
					modelMean[ntmp]   = (float)gpixel;
					modelVar[ntmp]    = highVar;
				}
				//normalize weight
				/*weightSum = 0;
				for(i = modelIndStart; i < modelIndEnd; i++)
					weightSum += modelWeight[i];
				for(i = modelIndStart; i < modelIndEnd; i++)
					modelWeight[i] /= weightSum;*/
			}
		}
/*
		if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			//for(i=0; i < modelIndEnd-modelIndStart; i++)
			unsigned int fg = (unsigned int)foreground->imageData[pixelInd];
				cout<<alfaWeight<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}

		/*
		if(match)
			modNumberImage->imageData[pixelInd] = (unsigned char)(maxPropInd-modelIndStart)*63;//(modNumber[pixelInd]-1)*63;
		else*/
			modNumberImage->imageData[pixelInd] = (unsigned char)(modNumber[pixelInd]-1)*63;

	}
}






void MOG::normalizeWeight()
{
	int pixelInd = 0, modelIndStart = 0,i,nModVary,modelIndEnd;
	int modelLength = imHeight*imWidth*nMod;
	float weightSum;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		nModVary = modNumber[pixelInd];
		modelIndEnd = modelIndStart + nModVary;
		weightSum = 0;
		for(i = modelIndStart; i < modelIndEnd; i++)
			weightSum += modelWeight[i];
		for(i = modelIndStart; i < modelIndEnd; i++)
			modelWeight[i] /= weightSum;
	}
}



void MOG::init(IplImage* img)
{
	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0;
	float gprop[5], *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		modelIndEnd = modelIndStart + nMod;
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];

		//get probability of current intensity value belong to Gaussians
		maxProp = -1.0;
		for(i = modelIndStart,j=0; i < modelIndEnd; i++,j++)
		{
			//ntmp = (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i];
			//ftmp = gaussianLUT[ ntmp ];
			gprop[j] = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];
			//ftmp = (-1.0/2.0)*pow((float)gpixel-modelMean[i],2)/modelVar[i];
			//gprop[j] = modelWeight[i]*(1/(sqrt(2*CV_PI*modelVar[i]))) * exp(ftmp);
			if(gprop[j] > maxProp) {maxProp = gprop[j];maxPropInd=i;}
		}

		//check match
		dist = modelMean[maxPropInd] - (float)gpixel;
		if(pow(dist,2) < 12.25*(modelVar[maxPropInd] + 12))
				match = 1;

		//foreground decision and model updates
		if(match)
		{
			//calculate weight over std
			for(j=0,i = modelIndStart; i < modelIndEnd;i++,j++)
				WOS[j] = modelWeight[i]/sqrt(modelVar[i]);


			// sort WOS in descending order
			bool swapped = true;
			j = 0;
			int ind[] = {0,1,2,3,4};
			while (swapped)
			{
				swapped = false;
				j++;
				for (int i = 0; i < nMod - j; i++)
				{
					  if (WOS[i] < WOS[i + 1])
					  {
							ftmp = WOS[i];
							WOS[i] = WOS[i + 1];
							WOS[i + 1] = ftmp;

							ntmp = ind[i];
							ind[i] = ind[i + 1];
							ind[i + 1] = ntmp;

							swapped = true;
					  }
				}
			}

			//foreground decision
			currentRatio = 0;
			for(i = 0; i < nMod ;i++)
			{
				modelInddum = modelIndStart + ind[i];
				currentRatio = currentRatio + modelWeight[modelInddum];
				if (maxPropInd == modelInddum)
				{
					foreground->imageData[pixelInd] = 0;
					break;
				}
				if(currentRatio > BGR)
				{
					foreground->imageData[pixelInd] = 255;
					break;
				}
			}



			//update of gaussians in case of match
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] = modelWeight[i] * oneMinusAlfaWeightInit;

			modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeightInit;
			modelMean[maxPropInd]   = oneMinusAlfaMeanInit*modelMean[maxPropInd] + alfaMeanInit*(float)gpixel;
			modelVar[maxPropInd]    = oneMinusAlfaVarInit*modelVar[maxPropInd] + alfaVarInit*pow((float)gpixel-modelMean[maxPropInd],2);

			if(modelVar[maxPropInd] > maxVar)
				modelVar[maxPropInd] = maxVar;

			if(modelVar[maxPropInd] < minVar)
				modelVar[maxPropInd] = minVar;
		}
		else // not match
		{
			// if not match it is foreground
			foreground->imageData[pixelInd] = 255;
			//find minimum weighted mode and replace it
			minWeight = modelWeight[modelIndStart]; minWeightInd = modelIndStart;
			for(i = modelIndStart+1; i < modelIndEnd; i++)
				if(modelWeight[i] < minWeight)
				{	minWeight = modelWeight[i];	minWeightInd = i;	}

			modelWeight[minWeightInd] = lowWeightInit;
			modelMean[minWeightInd]   = (float)gpixel;
			modelVar[minWeightInd]    = highVar;

			//normalize weight
			weightSum = 0;
			for(i = modelIndStart; i < modelIndEnd; i++)
				weightSum += modelWeight[i];
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] /= weightSum;
		}
	}
}






void MOG::init3(IplImage* img)
{
	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0;
	float gprop[5], *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{
		modelIndEnd = modelIndStart + nMod;
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];

		//get probability of current intensity value belong to Gaussians
		maxProp = -1.0;
		for(i = modelIndStart,j=0; i < modelIndEnd; i++,j++)
		{
			//ntmp = (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i];
			//ftmp = gaussianLUT[ ntmp ];
			gprop[j] = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];
			//ftmp = (-1.0/2.0)*pow((float)gpixel-modelMean[i],2)/modelVar[i];
			//gprop[j] = modelWeight[i]*(1/(sqrt(2*CV_PI*modelVar[i]))) * exp(ftmp);
			if(gprop[j] > maxProp) {maxProp = gprop[j];maxPropInd=i;}
		}

		//check match
		dist = modelMean[maxPropInd] - (float)gpixel;
		if(pow(dist,2) < 12.25*(modelVar[maxPropInd] + 12))
				match = 1;

		//foreground decision and model updates
		if(match)
		{
			//foreground decision
			currentRatio = 0;
			for(i = modelIndStart; i < modelIndEnd ;i++)
			{
				currentRatio = currentRatio + modelWeight[i];
				if (maxPropInd == i)
				{
					foreground->imageData[pixelInd] = 0;
					break;
				}
				if(currentRatio > BGR)
				{
					foreground->imageData[pixelInd] = 255;
					break;
				}
			}



			//update of gaussians in case of match
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] = modelWeight[i] * oneMinusAlfaWeightInit;

			modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeightInit;
			modelMean[maxPropInd]   = oneMinusAlfaMeanInit*modelMean[maxPropInd] + alfaMeanInit*(float)gpixel;
			modelVar[maxPropInd]    = oneMinusAlfaVarInit*modelVar[maxPropInd] + alfaVarInit*pow((float)gpixel-modelMean[maxPropInd],2);

			if(modelVar[maxPropInd] > maxVar)
				modelVar[maxPropInd] = maxVar;

			if(modelVar[maxPropInd] < minVar)
				modelVar[maxPropInd] = minVar;

			//keep weight order
			if(maxPropInd != modelIndStart)
			{
				if(modelWeight[maxPropInd] > modelWeight[maxPropInd-1])
				{
					ftmp = modelWeight[maxPropInd];
					modelWeight[maxPropInd]   = modelWeight[maxPropInd-1];
					modelWeight[maxPropInd-1] = ftmp;

					ftmp = modelMean[maxPropInd];
					modelMean[maxPropInd]   = modelMean[maxPropInd-1];
					modelMean[maxPropInd-1] = ftmp;

					ftmp = modelVar[maxPropInd];
					modelVar[maxPropInd]   = modelVar[maxPropInd-1];
					modelVar[maxPropInd-1] = ftmp;
				}
			}
		}
		else // not match
		{
			// if not match it is foreground
			foreground->imageData[pixelInd] = 255;
			//find minimum weighted mode and replace it
			minWeight = modelWeight[modelIndStart]; minWeightInd = modelIndStart;
			for(i = modelIndStart+1; i < modelIndEnd; i++)
				if(modelWeight[i] < minWeight)
				{	minWeight = modelWeight[i];	minWeightInd = i;	}

			modelWeight[minWeightInd] = lowWeightInit;
			modelMean[minWeightInd]   = (float)gpixel;
			modelVar[minWeightInd]    = highVar;

			//normalize weight
			weightSum = 0;
			for(i = modelIndStart; i < modelIndEnd; i++)
				weightSum += modelWeight[i];
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] /= weightSum;
		}
	}
}





void MOG::init4(IplImage* img)
{

	nframes++;
	int testInd  = 114 + imWidth*386;

	int i,j,ntmp, modelIndStart, modelIndEnd, modelInddum, pixelInd, modelLength, maxPropInd=0, minWeightInd=0,nModVary;
	float gprop, *WOS = new float[nMod], dist, currentRatio, maxProp, minWeight, weightSum, ftmp;
	bool match;
	unsigned char gpixel;

	modelLength = imHeight*imWidth*nMod;

	pixelInd = 0;
	modelIndStart = 0;

	for(; modelIndStart < modelLength; modelIndStart+=nMod,pixelInd++)
	{

		nModVary = modNumber[pixelInd];
		modelIndEnd = modelIndStart + nModVary;
		//reset match and get the pixel value
		match = 0;
		gpixel = (unsigned char)img->imageData[pixelInd];

		//get probability of current intensity value belong to Gaussians
		maxProp = -1.0;
		for(i = modelIndStart,j=0; i < modelIndEnd; i++,j++)
		{

			gprop = modelWeight[i] * gaussianLUT[ (int)abs( (float)gpixel-modelMean[i] ) + diffLength*(int)modelVar[i] ];

			if(gprop > maxProp) {maxProp = gprop;maxPropInd=i;}
		}

		//check match
		dist = modelMean[maxPropInd] - (float)gpixel;
		if(pow(dist,2) < 12.25*(modelVar[maxPropInd] + 12))
				match = 1;

		//foreground decision and model updates
		if(match)
		{


			//foreground decision
			currentRatio = 0;
			for(i = modelIndStart; i < maxPropInd ;i++)
			{
				currentRatio = currentRatio + modelWeight[i];
			}

			if(currentRatio > BGR)
				foreground->imageData[pixelInd] = 255;
			else
				foreground->imageData[pixelInd] = 0;


			//update of gaussians in case of match
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] = modelWeight[i] * oneMinusAlfaWeightInit;


			modelWeight[maxPropInd] = modelWeight[maxPropInd] + alfaWeightInit;
			modelMean[maxPropInd]   = oneMinusAlfaMeanInit*modelMean[maxPropInd] + alfaMeanInit*(float)gpixel;
			modelVar[maxPropInd]    = oneMinusAlfaVarInit*modelVar[maxPropInd] + alfaVarInit*pow((float)gpixel-modelMean[maxPropInd],2);

			if(modelVar[maxPropInd] > maxVar)
				modelVar[maxPropInd] = maxVar;

			if(modelVar[maxPropInd] < minVar)
				modelVar[maxPropInd] = minVar;
			//keep weight order
			if(maxPropInd != modelIndStart)
			{
				if(modelWeight[maxPropInd] > modelWeight[maxPropInd-1])
				{
					ftmp = modelWeight[maxPropInd];
					modelWeight[maxPropInd]   = modelWeight[maxPropInd-1];
					modelWeight[maxPropInd-1] = ftmp;

					ftmp = modelMean[maxPropInd];
					modelMean[maxPropInd]   = modelMean[maxPropInd-1];
					modelMean[maxPropInd-1] = ftmp;

					ftmp = modelVar[maxPropInd];
					modelVar[maxPropInd]   = modelVar[maxPropInd-1];
					modelVar[maxPropInd-1] = ftmp;
				}
			}
			//eliminate low weighted mod
			if(modelWeight[modelIndEnd-1] < minAcceptedWeight)
				modNumber[pixelInd] -= 1;

		}
		else // not match
		{
			// if not match it is foreground
			foreground->imageData[pixelInd] = 255;
			//check free slot
			if(nModVary != nMod)
			{
				modNumber[pixelInd] += 1;
				modelWeight[modelIndEnd] = lowWeightInit;
				modelMean[modelIndEnd]   = (float)gpixel;
				modelVar[modelIndEnd]    = highVar;
				modelIndEnd +=1;
			}
			else
			{
				ntmp = modelIndEnd -1;
				modelWeight[ntmp] = lowWeightInit;
				modelMean[ntmp]   = (float)gpixel;
				modelVar[ntmp]    = highVar;
			}
			//normalize weight
			weightSum = 0;
			for(i = modelIndStart; i < modelIndEnd; i++)
				weightSum += modelWeight[i];
			for(i = modelIndStart; i < modelIndEnd; i++)
				modelWeight[i] /= weightSum;
		}

		/*if(pixelInd == testInd)
		{
			cout<<(float)gpixel<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelMean[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelVar[i]<<"		";
			cout<<endl;
			for(i=modelIndStart; i < modelIndEnd; i++)
				cout<<modelWeight[i]<<"	";
			cout<<endl;
			//for(i=0; i < modelIndEnd-modelIndStart; i++)
			if(foreground->imageData[pixelInd] == 255)
				cout<<"255"<<endl;
			else
				cout<<"0"<<endl;
			cout<<alfaWeight<<"	";

			//ftmp = 1/sqrt(2*CV_PI*(144));
			//cout<<endl<<ftmp<<endl;
			//ftmp = (float)(-1.0/2.0)*pow(56.69-11,2)/144.0;
			//ftmp = exp(ftmp);
			//cout<<endl<<ftmp<<endl;

			cout<<endl<<"*****************"<<endl;
		}
		*/
		modNumberImage->imageData[pixelInd] = (unsigned char)(modNumber[pixelInd]-1)*63;

	}
}
