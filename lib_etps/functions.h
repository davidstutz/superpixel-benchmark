#pragma once

#include "stdafx.h"
#include "structures.h"

class Timer {
private:
    clock_t startTime, time;
    bool running;
public:
    Timer(bool run = true)
    { 
        if (run) Reset();
        else {
            startTime = time = 0;
            running = false;
        }
    }

    void Reset() 
    { 
        time = 0;
        startTime = clock();
        running = true;
    }

    void Stop() 
    { 
        if (running) {
            time += clock() - startTime;
            running = false;
        }
    }

    void Resume()
    {
        startTime = clock();
        running = true;
    }

    clock_t GetTime()
    {
        return time;
    }

    double GetTimeInSec()
    {
        return (double)time / CLOCKS_PER_SEC;
    }

    friend ostream& operator<<(ostream& os, const Timer& t);
};

// Cached info for connectivity of 3x3 patches
class ConnectivityCache {
    vector<bool> cache;
public:
    ConnectivityCache();
    bool IsConnected(int b) const { return cache[b]; }
    void Print(); // for debug
private:
    void Initialize();
};


// See definition for description
void MovePixel(Matrix<Pixel>& pixelsImg, PixelMoveData& pmd);
void MovePixelStereo(Matrix<Pixel>& pixelsImg, PixelMoveData& pmd, bool changeBoundary);

// Return true if superpixel sp is connected in region defined by upper left/lower right corners of pixelsImg
bool IsSuperpixelRegionConnected(const Matrix<Pixel>& pixelsImg, Pixel* p, int ulr, int ulc, int lrr, int lrc);

// Return true if superpixel sp is connected in region defined by upper left/lower right corners of pixelsImg
bool IsSuperpixelRegionConnectedOptimized(const Matrix<Pixel>& pixelsImg, Pixel* p, int ulr, int ulc, int lrr, int lrc);

// For debug purposes; inefficient!
int CalcSuperpixelBoundaryLength(const Matrix<Pixel>& pixelsImg, Superpixel* sp);

// Length of superpixel boundary
void CalcSuperpixelBoundaryLength(const Matrix<Pixel>& pixelsImg, Pixel* p, Superpixel* sp, Superpixel* sq,
    int& spbl, int& sqbl, int& sobl);

bool IsPatch3x3Connected(int bits);

// Plane through 3 points, returns false if p1, p2, p3 are approximately coplanar (eps normal size)
bool Plane3P(const cv::Point3d& p1, const cv::Point3d& p2, const cv::Point3d& p3, Plane_d& plane);

// Returns false if pixels.size() < 3 or no 3 points were found to 
// form a plane
bool RANSACPlane(const vector<cv::Point3d>& pixels, Plane_d& plane);
void InitSuperpixelPlane(SuperpixelStereo* sp, const cv::Mat1d& depthImg);

// Equation (8) for superpixel sp
void CalcDispEnergy(SuperpixelStereo* sp, const cv::Mat1d& dispImg, double noDisp);

void CalcCoSmoothnessSum(const cv::Mat1d& depthImg, double inlierThresh, SuperpixelStereo* sp, SuperpixelStereo* sq, double& eSmo, int& count);

void CalcCoSmoothnessSum2(SuperpixelStereo* sp, SuperpixelStereo* sq, double& eSmo, int& count);

bool LeastSquaresPlaneDebug(const double x1, const double y1, const double z1, const double d1,
    const double x2, const double y2, const double z2, const double d2,
    const double x3, const double y3, const double z3, const double d3,
    Plane_d& plane);