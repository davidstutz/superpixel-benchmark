#pragma once

#include "stdafx.h"
#include "structures.h"
#include "tsdeque.h"
#include <map>

#define ADD_LEVEL_PARAM_DOUBLE(field, node, name) \
    updateDouble[name] = [](SPSegmentationParameters& params, double val) { params.field = val; };\
    AddLevelParamFromNodeDouble(node, name);

#define ADD_LEVEL_PARAM_INT(field, node, name) \
    updateInt[name] = [](SPSegmentationParameters& params, int val) { params.field = val; };\
    AddLevelParamFromNodeInt(node, name);



using namespace cv;
using namespace std;

void UpdateFromNode(double& val, const FileNode& node);
void UpdateFromNode(int& val, const FileNode& node);
void UpdateFromNode(bool& val, const FileNode& node);

template <typename T> 
void AddLevelParamFromNode(const FileNode& parentNode, const string& nodeName,
    vector<pair<string, vector<T>>>& paramsList)
{
    FileNode n = parentNode[nodeName];

    if (n.empty()) return;

    vector<T> levParams;

    if (n.type() != FileNode::SEQ) {
        levParams.push_back((T)n);
    } else {
        for (FileNode n1 : n) {
            levParams.push_back((T)n1);
        }
    }
    paramsList.push_back(pair<string, vector<T>>(nodeName, levParams));
}


struct SPSegmentationParameters {
    SPSegmentationParameters() :
        superpixelNum(300),        // Number of superpixels (the actual number can be different)
        gridSize(0),
        appWeight(1.0),
        regWeight(1.0),
        lenWeight(1.0),
        sizeWeight(1.0),        
        dispWeight(2000.0),     // \lambda_{disp}
        smoWeight(0.2),         // \lambda_{smo}
        smoWeightCo(0.2),       // \lambda_{smo}
        smoWeightHi(0.2),       // \lambda_{smo}
        priorWeight(0.2),       // \lambda_{prior}
        occPriorWeight(15.0),   // \lambda_{occ}
        hiPriorWeight(5.0),     // \lambda_{hinge}
        noDisp(9.0),            // \lambda_{d}
        inlierThreshold(3.0),
        peblThreshold(0),          // planeEstimationBundaryLengthThreshold 
        updateThreshold(0.01),
        iterations(1),
        maxUpdates(400000),
        minPixelSize(1),
        maxPixelSize(16),
        reSteps(10),
        instantBoundary(false),   // Boundary re-estimation on each step of iteration
        stereo(false),
        computeSGM(false),
        batchProcessing(true),
        inpaint(false),           // use opencv's inpaint method to fill gaps in
        debugOutput(false),
        timingOutput(true),
        randomSeed(0) {};
    
    int superpixelNum;        // Number of superpixels (the actual number can be different)
    int gridSize;
    
    double appWeight;
    double regWeight;
    double lenWeight;
    double sizeWeight;        
    double dispWeight;     // \lambda_{disp}
    double smoWeight;         // \lambda_{smo}
    double smoWeightCo;       // \lambda_{smo}
    double smoWeightHi;       // \lambda_{smo}
    double priorWeight;       // \lambda_{prior}
    double occPriorWeight;   // \lambda_{occ}
    double hiPriorWeight;     // \lambda_{hinge}
    double noDisp;            // \lambda_{d}
    double inlierThreshold;
    int peblThreshold;          // planeEstimationBundaryLengthThreshold 
    double updateThreshold;

    int iterations;
    int maxUpdates;
    int minPixelSize;
    int maxPixelSize;
    int reSteps;

    bool instantBoundary;   // Boundary re-estimation on each step of iteration
    bool stereo;
    bool computeSGM;
    bool batchProcessing;
    bool inpaint;           // use opencv's inpaint method to fill gaps in
                                    // disparity image
    bool debugOutput;
    bool timingOutput;
    int randomSeed;

    vector<pair<string, vector<double>>> levelParamsDouble;
    vector<pair<string, vector<int>>> levelParamsInt;
    map<string, function<void(SPSegmentationParameters&, double)>> updateDouble;
    map<string, function<void(SPSegmentationParameters&, int)>> updateInt;

    void SetLevelParams(int level);

    void Read(const FileNode& node)
    {
        UpdateFromNode(superpixelNum, node["superpixelNum"]);
        ADD_LEVEL_PARAM_DOUBLE(appWeight, node, "appWeight");
        ADD_LEVEL_PARAM_DOUBLE(regWeight, node, "regWeight");
        ADD_LEVEL_PARAM_DOUBLE(lenWeight, node, "lenWeight");
        ADD_LEVEL_PARAM_DOUBLE(sizeWeight, node, "sizeWeight");
        ADD_LEVEL_PARAM_DOUBLE(dispWeight, node, "dispWeight");
        ADD_LEVEL_PARAM_DOUBLE(smoWeight, node, "smoWeight");
        ADD_LEVEL_PARAM_DOUBLE(smoWeightCo, node, "smoWeightCo");
        ADD_LEVEL_PARAM_DOUBLE(smoWeightHi, node, "smoWeightHi");
        ADD_LEVEL_PARAM_DOUBLE(priorWeight, node, "priorWeight");
        ADD_LEVEL_PARAM_DOUBLE(occPriorWeight, node, "occPriorWeight");
        ADD_LEVEL_PARAM_DOUBLE(hiPriorWeight, node, "hiPriorWeight");
        UpdateFromNode(noDisp, node["noDisp"]);
        UpdateFromNode(stereo, node["stereo"]);
        UpdateFromNode(computeSGM, node["computeSGM"]);
        UpdateFromNode(batchProcessing, node["batchProcessing"]);
        UpdateFromNode(inpaint, node["inpaint"]);
        UpdateFromNode(instantBoundary, node["instantBoundary"]);
        UpdateFromNode(iterations, node["iterations"]);
        ADD_LEVEL_PARAM_INT(reSteps, node, "reSteps");
        UpdateFromNode(inlierThreshold, node["inlierThreshold"]);
        UpdateFromNode(maxUpdates, node["maxUpdates"]);
        UpdateFromNode(minPixelSize, node["minPixelSize"]);
        UpdateFromNode(maxPixelSize, node["maxPixelSize"]);
        ADD_LEVEL_PARAM_INT(peblThreshold, node, "peblThreshold");
        UpdateFromNode(updateThreshold, node["updateThreshold"]);
        UpdateFromNode(debugOutput, node["debugOutput"]);
        UpdateFromNode(timingOutput, node["timingOutput"]);
        UpdateFromNode(randomSeed, node["randomSeed"]);
        SetLevelParams(0);
    }


private:
    void AddLevelParamFromNodeDouble(const FileNode& parentNode, const string& nodeName)
    {
        AddLevelParamFromNode<double>(parentNode, nodeName, levelParamsDouble);
    }

    void AddLevelParamFromNodeInt(const FileNode& parentNode, const string& nodeName)
    {
        AddLevelParamFromNode<int>(parentNode, nodeName, levelParamsInt);
    }

};

//static void read(const FileNode& node, SPSegmentationParameters& x, const SPSegmentationParameters& defaultValue = SPSegmentationParameters());


class BInfoMatrix  {
private:
    BInfo** data;
    int rows;
    int cols;
public:
    BInfoMatrix() : data(nullptr), rows(0), cols(0) { }
    ~BInfoMatrix() { Release(); }

    void Resize(int _rows, int _cols)
    {
        Release();
        rows = _rows;
        cols = _cols;
        data = new BInfo*[rows*cols];
        memset(data, 0, sizeof(BInfo*)*rows*cols);
    }

    BInfo& operator ()(int r, int c) { return *(*data + r*cols + c); }

private:
    void Release()
    {
        if (data != nullptr) {
            BInfo* end = *data + rows*cols, *p = *data;
            for (; p != end; p++) { if (p != nullptr) delete p; }
            delete[] data;
        }
    }
};


class SPSegmentationEngine {
private:
    struct PerformanceInfo {
        PerformanceInfo() : init(0.0), imgproc(0.0), ransac(0.0), total(0.0) {}
        double init;
        double imgproc;
        double ransac;
        vector<double> levelTimes;
        vector<int> levelIterations;
        double total;
        vector<double> levelMaxEDelta;
        vector<int> levelMaxPixelSize;
    };

    PerformanceInfo performanceInfo;

    // Parameters
    SPSegmentationParameters params;
    double planeSmoothWeight;   // Calculated from params in initialization
    double planeSmoothWeightCo;
    double planeSmoothWeightHi;
    int initialMaxPixelSize;        // Calculated in initialization

    // Original image to process
    Mat origImg;

    // Depth image (required for stereo)
    Mat1d depthImg;
    Mat1d depthImgAdj;

    // Image to process (in lab color space)
    Mat img;

    // Support structures
    Matrix<Pixel> pixelsImg;    // pixels matrix, dimension varies, depends on level
    Matrix<Pixel*> ppImg;       // matrix of dimension of img, pointers to pixelsImg pixels (for stereo)
    vector<Superpixel*> superpixels;
public:
    SPSegmentationEngine(SPSegmentationParameters params, Mat img, Mat depthImg = Mat());
    virtual ~SPSegmentationEngine();

    void ProcessImage();
    void ProcessImageStereo();
    Mat GetSegmentedImage();
    Mat GetSegmentedImagePlain();
    Mat GetSegmentedImageStereo();
    Mat GetSegmentation() const;
    Mat GetDisparity() const;
    string GetSegmentedImageInfo();
    void PrintDebugInfo();
    void PrintDebugInfo2();
    void PrintDebugInfoStereo();
    void PrintPerformanceInfo();
    int GetNoOfSuperpixels() const;
    double ProcessingTime() { return performanceInfo.total; }
private:
    void Initialize(Superpixel* spGenerator(int));
    void InitializeStereo();
    void InitializeStereoEnergies();
    void InitializePPImage();
    void UpdatePPImage();
    int IterateMoves(int level);
    void ReEstimatePlaneParameters();
    void EstimatePlaneParameters();
    bool SplitPixels(int& newMaxPixelSize);
    void Reset();
    void UpdateBoundaryData();
    void UpdateBoundaryData2();
    void UpdateInlierSums();
    void UpdatePlaneParameters();
    void UpdateStereoSums();
    void UpdateHingeStereoSums();
    void UpdateDispSums();

    void DebugNeighborhoods();
    void DebugBoundary();
    void DebugDispSums();

    bool TryMovePixel(Pixel* p, Pixel* q, PixelMoveData& psd);
    bool TryMovePixelStereo(Pixel* p, Pixel* q, PixelMoveData& psd);

    int Iterate(Deque<Pixel*>& list, Matrix<bool>& inList);
};


SPSegmentationParameters ReadParameters(const string& fileName, const SPSegmentationParameters& defaultValue = SPSegmentationParameters());



