#ifndef _IMAGERECONSTRUCTION_H_
#define _IMAGERECONSTRUCTION_H_

#include <opencv2/opencv.hpp>
#include <queue>

struct Pixel {
	int row;
	int col;
};

using namespace cv;
using namespace std;

template<typename T> void PropagationStep(Mat& marker, Mat& mask ,int x, int y, int offsetX, int offsetY, queue<Pixel>& queue)
{
	if(marker.at<T>(x+offsetX, y+offsetY) < marker.at<T>(x, y)) {
		if(mask.at<T>(x+offsetX, y+offsetY) != marker.at<T>(x+offsetX, y+offsetY)) {
			
			Pixel pixel = { x+offsetX, y+offsetY };
			queue.push(pixel);

			if(mask.at<T>(x+offsetX, y+offsetY) < marker.at<T>(x, y)) {
				marker.at<T>(x+offsetX, y+offsetY) = mask.at<T>(x+offsetX, y+offsetY);
			} else {
				marker.at<T>(x+offsetX, y+offsetY) = marker.at<T>(x, y);
			}
		}
	}
}

template<typename T> void ImageReconstruct(Mat& marker, Mat& mask)
{
	queue<Pixel> queue;

	int offset = 4;
	Scalar scalar = Scalar(0);

	Mat tempMarkerImage = Mat(marker.rows + 2 * offset, marker.cols + 2 * offset, marker.depth());
	copyMakeBorder(marker, tempMarkerImage, offset, offset, offset, offset, BORDER_CONSTANT, scalar);

	Mat tempMaskImage = Mat(mask.rows + 2 * offset, mask.cols + 2 * offset, mask.depth());
	copyMakeBorder(mask, tempMaskImage, offset, offset, offset, offset, BORDER_CONSTANT, scalar);

	for(int row = offset; row < tempMarkerImage.rows - offset; row++) {
		for(int col = offset; col < tempMarkerImage.cols - offset; col++) {
			T currentPixel = tempMarkerImage.at<T>(row, col);

			if(currentPixel < tempMarkerImage.at<T>(row, col-1))
				currentPixel = tempMarkerImage.at<T>(row, col-1);
			if(currentPixel < tempMarkerImage.at<T>(row-1, col-1))
				currentPixel = tempMarkerImage.at<T>(row-1, col-1);
			if(currentPixel < tempMarkerImage.at<T>(row-1, col))
				currentPixel = tempMarkerImage.at<T>(row-1, col);
			if(currentPixel < tempMarkerImage.at<T>(row-1, col+1))
				currentPixel = tempMarkerImage.at<T>(row-1, col+1);

			if(currentPixel > tempMaskImage.at<T>(row, col))
				tempMarkerImage.at<T>(row, col) = tempMaskImage.at<T>(row, col);
			else
				tempMarkerImage.at<T>(row, col) = currentPixel;
		}
	}

	for(int row = tempMarkerImage.rows - offset - 1; row > offset - 1; row--) {
		for(int col = tempMarkerImage.cols - offset - 1; col > offset - 1; col--) {
			T currentPixel = tempMarkerImage.at<T>(row, col);

			if(currentPixel < tempMarkerImage.at<T>(row, col+1))
				currentPixel = tempMarkerImage.at<T>(row, col+1);
			if(currentPixel < tempMarkerImage.at<T>(row+1, col+1))
				currentPixel = tempMarkerImage.at<T>(row+1, col+1);
			if(currentPixel < tempMarkerImage.at<T>(row+1, col))
				currentPixel = tempMarkerImage.at<T>(row+1, col);
			if(currentPixel < tempMarkerImage.at<T>(row+1, col-1))
				currentPixel = tempMarkerImage.at<T>(row+1, col-1);

			if(currentPixel > tempMaskImage.at<T>(row, col))
				tempMarkerImage.at<T>(row, col) = tempMaskImage.at<T>(row, col);
			else
				tempMarkerImage.at<T>(row, col) = currentPixel;

			currentPixel = tempMarkerImage.at<T>(row, col);
			if(tempMarkerImage.at<T>(row, col+1) < currentPixel &&
			   tempMarkerImage.at<T>(row, col+1) < tempMaskImage.at<T>(row, col+1))
			{
				Pixel pixel = { row, col };
				queue.push(pixel);
			}
			else if(tempMarkerImage.at<T>(row+1, col+1) < currentPixel &&
			   tempMarkerImage.at<T>(row+1, col+1) < tempMaskImage.at<T>(row+1, col+1))
			{
				Pixel pixel = { row, col };
				queue.push(pixel);
			}
			else if(tempMarkerImage.at<T>(row+1, col) < currentPixel &&
			   tempMarkerImage.at<T>(row+1, col) < tempMaskImage.at<T>(row+1, col))
			{
				Pixel pixel = { row, col };
				queue.push(pixel);
			}
			else if(tempMarkerImage.at<T>(row+1, col-1) < currentPixel &&
			   tempMarkerImage.at<T>(row+1, col-1) < tempMaskImage.at<T>(row+1, col-1))
			{
				Pixel pixel = { row, col };
				queue.push(pixel);
			}
		}
	}

	while(!queue.empty()) {
		Pixel pixel = queue.front(); queue.pop();

		for(int i = -1; i <= 1; i++) {
			for(int k = -1; k <= 1; k++) {
				if(i == 0 && k == 0)
					continue;

				PropagationStep<T>(tempMarkerImage, tempMaskImage, pixel.row, pixel.col, i, k, queue);
			}
		}
	}

	tempMarkerImage.adjustROI(-offset, -offset, -offset, -offset);
	tempMarkerImage.copyTo(marker);

	tempMarkerImage.release();
	tempMaskImage.release();
}

#endif
