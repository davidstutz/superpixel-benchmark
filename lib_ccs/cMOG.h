/*
 * cMOG.h
 *
 *  Created on: 02 A�u 2011
 *      Author: Tu�han Marpu�
 */


#ifndef CMOG_H_
#define CMOG_H_


#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

class MOG
{
	private:
		// learning rates
		float alfaMean;
		float alfaVar;
		float alfaWeight;

		float alfaMean2;
		float alfaVar2;
		float alfaWeight2;

		float alfaMeanInit;
		float alfaVarInit;
		float alfaWeightInit;

		float oneMinusAlfaMean;
		float oneMinusAlfaVar;
		float oneMinusAlfaWeight;
		float oneMinusAlfaMeanInit;
		float oneMinusAlfaVarInit;
		float oneMinusAlfaWeightInit;

		// history
		float	history;

		// for new mode
		float lowWeight;
		float lowWeightInit;
		float highVar;

		// variance limits
		float minVar;
		float maxVar;

		// mode number, background ratio, image sizes
		int nMod;
		float BGR;
		int imHeight;
		int imWidth;



		// look up table to calculate Gaussian distribution
		float *gaussianLUT;
		int varLength;
		int diffLength;

		// for the version of 4,5
		int *modNumber;		//keeps the number of mods used for each pixel
		float minAcceptedWeight;

		//frame number
		int nframes;
	public:
		// MOG model
		float *modelMean;
		float *modelVar;
		float *modelWeight;

		//current foreground
		IplImage* foreground;
		IplImage* modNumberImage;

	public:
		MOG(int _nMod, float _BGR, int _imHeight, int _imWidth);
		void init(IplImage* img);
		void init3(IplImage* img);				// do not support the versions before 3 and after versions after next init.
		void init4(IplImage* img);				// do not support the versions before 4 and after versions  next init.
		void init5(IplImage* img);				// do not support the versions before 4 and after versions  next init.
		void init6(IplImage* img);				// do not support the versions before 4 and after versions  next init.
		void normalizeWeight();					//call after init5-6
		void getForeGround(IplImage* img);		// same as previous(same in the stauffer and grimson paper)
		void getForeGround2(IplImage* img);		// mods are sorted not in the order weight/sigma, instead only weight order is used
		void getForeGround3(IplImage* img);		// added to 2 -> weight array is kept sorted, so sorting operation is removed with a swap operation to check the order after the update.
		void getForeGround4(IplImage* img);		// added to 3 -> since weight array is kept sorted low weighted mods are ignored until they are replaced.
		void getForeGround5(IplImage* img);     // added to 4 -> one mode check is added, if there is only one mode many short cut are done to speed up the code
		void getForeGround6(IplImage* img);     // different from to 5 -> instead of gaussian probability use in matching, only matching is done in the order of weight

};


#endif /* CMOG_H_ */
