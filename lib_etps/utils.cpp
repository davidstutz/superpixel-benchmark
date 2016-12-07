#include "stdafx.h"
#include "utils.h"
#include "tinydir.h"
#include <regex>
#include <memory>
#include <cstdarg>
#ifdef WIN32
#include <direct.h>
#endif

using namespace std;
using namespace cv;

// Uses tinydir.h (https://github.com/cxong/tinydir) and C++11 regular expressions
// to be platform independent and not using dependencies as boost...
// Note that it only works for "normal" file names -- consisting of letters and digits 
// (does not escape all regex special characters...)
// Returns list of files (without dir name)
void FindFiles(const string& dir, const string& pattern, vector<string>& files, bool fullPath)
{
    try {
        files.clear();
        string regexPattern = "";
        tinydir_dir dirInfo;

        for (auto ch : pattern) {
            if (ch == '.') regexPattern += "\\.";
            else if (ch == '*') regexPattern += ".*";
            else if (ch == '?') regexPattern += ".";
            else regexPattern += ch;
        }

        regex reg(regexPattern);

        if (tinydir_open(&dirInfo, dir.c_str()) == -1) {
            return;
        }
        while (dirInfo.has_next) {
            tinydir_file fileInfo;

            if (tinydir_readfile(&dirInfo, &fileInfo) == -1)
                return;
            if (!fileInfo.is_dir && regex_match(fileInfo.name, reg)) {
                if (fullPath) files.push_back(fileInfo.path);
                else files.push_back(fileInfo.name);
            }
            tinydir_next(&dirInfo);
        }
    } catch (...) {
    }
}

string ChangeExtension(const string& fileName, const string& newExt)
{
    size_t dotPos = fileName.find_last_of('.');

    if (dotPos == string::npos) return fileName + newExt;
    else return fileName.substr(0, dotPos) + newExt;
}

string FilePath(const string& fileName)
{
    size_t pos = fileName.find_last_of("/\\");
    string path = pos == string::npos ? "" : fileName.substr(0, pos);
    EndDir(path);
    return path;
}

string FileName(const string& fileName)
{
    size_t pos = fileName.find_last_of("/\\");
    return pos == string::npos ? fileName : fileName.substr(pos + 1);
}


void EndDir(string& dirName)
{
    if (!dirName.empty() && dirName.back() != '\\' && dirName.back() != '/')
        dirName += '/';
}

void MkDir(const string& dirName)
{
    if (!dirName.empty()) {
#ifdef WIN32
        _mkdir(dirName.c_str());
#else
        mkdir(dirName.c_str(), 0777);
#endif
    }
}

// From Jian's code
Mat ConvertRGBToLab(const Mat& img)
{
    const int RGB2LABCONVERTER_XYZ_TABLE_SIZE = 1024;
    // CIE standard parameters
    const double epsilon = 0.008856;
    const double kappa = 903.3;
    // Reference white
    const double referenceWhite[3] = { 0.950456, 1.0, 1.088754 };
    /// Maximum values
    const double maxXYZValues[3] = { 0.95047, 1.0, 1.08883 };

    vector<float> sRGBGammaCorrections(256);
    for (int pixelValue = 0; pixelValue < 256; ++pixelValue) {
        double normalizedValue = pixelValue / 255.0;
        double transformedValue = (normalizedValue <= 0.04045) ? normalizedValue / 12.92 : pow((normalizedValue + 0.055) / 1.055, 2.4);

        sRGBGammaCorrections[pixelValue] = transformedValue;
    }

    int tableSize = RGB2LABCONVERTER_XYZ_TABLE_SIZE;
    vector<double> xyzTableIndexCoefficients(3);
    xyzTableIndexCoefficients[0] = (tableSize - 1) / maxXYZValues[0];
    xyzTableIndexCoefficients[1] = (tableSize - 1) / maxXYZValues[1];
    xyzTableIndexCoefficients[2] = (tableSize - 1) / maxXYZValues[2];

    vector<vector<float>> fXYZConversions(3);
    for (int xyzIndex = 0; xyzIndex < 3; ++xyzIndex) {
        fXYZConversions[xyzIndex].resize(tableSize);
        double stepValue = maxXYZValues[xyzIndex] / tableSize;
        for (int tableIndex = 0; tableIndex < tableSize; ++tableIndex) {
            double originalValue = stepValue*tableIndex;
            double normalizedValue = originalValue / referenceWhite[xyzIndex];
            double transformedValue = (normalizedValue > epsilon) ? pow(normalizedValue, 1.0 / 3.0) : (kappa*normalizedValue + 16.0) / 116.0;

            fXYZConversions[xyzIndex][tableIndex] = transformedValue;
        }
    }

    Mat3d result = Mat3d(img.rows, img.cols);

    for (int y = 0; y < img.rows; ++y) {
        for (int x = 0; x < img.cols; ++x) {
            const Vec3b& rgbColor = img.at<Vec3b>(y, x);
            Vec3d& labColor = result(y, x);

            float correctedR = sRGBGammaCorrections[rgbColor[2]];
            float correctedG = sRGBGammaCorrections[rgbColor[1]];
            float correctedB = sRGBGammaCorrections[rgbColor[0]];
            float xyzColor[3];

            xyzColor[0] = correctedR*0.4124564f + correctedG*0.3575761f + correctedB*0.1804375f;
            xyzColor[1] = correctedR*0.2126729f + correctedG*0.7151522f + correctedB*0.0721750f;
            xyzColor[2] = correctedR*0.0193339f + correctedG*0.1191920f + correctedB*0.9503041f;

            int tableIndexX = static_cast<int>(xyzColor[0] * xyzTableIndexCoefficients[0] + 0.5);
            int tableIndexY = static_cast<int>(xyzColor[1] * xyzTableIndexCoefficients[1] + 0.5);
            int tableIndexZ = static_cast<int>(xyzColor[2] * xyzTableIndexCoefficients[2] + 0.5);

            float fX = fXYZConversions[0][tableIndexX];
            float fY = fXYZConversions[1][tableIndexY];
            float fZ = fXYZConversions[2][tableIndexZ];

            labColor[0] = 116.0*fY - 16.0;
            labColor[1] = 500.0*(fX - fY);
            labColor[2] = 200.0*(fY - fZ);
        }
    }
    return result;
}

cv::Mat InpaintDisparityImage(const cv::Mat& img)
{
    cv::Mat1b toInpaint, inpainted;
    cv::Mat1d result;

    img.convertTo(toInpaint, CV_8UC1);
    cv::inpaint(toInpaint, (toInpaint == 0), inpainted, 5.0, cv::INPAINT_TELEA);
    //cv::imwrite("c:\\tmp\\inp.png", inpainted);
    inpainted.convertTo(result, CV_64FC1);
    return result;
}



// Fills pixel with zero value with min of its neighbors in
// the same row. As in Jian's code input is Mat1d
Mat FillGapsInDisparityImage(const Mat& img)
{
    if (img.rows == 0 || img.cols == 0)
        return Mat1d();

    Mat1d result = Mat1d(img.rows, img.cols);
    
    for (int r = 0; r < img.rows; r++) {
        int zCount = 0;
        double nzVal = 0.0;

        for (int c = 0; c < img.cols; c++) {
            const double& p = img.at<double>(r, c);
            double& rp = result(r, c);

            if (p == 0) zCount++;
            else {
                rp = p;
                if (zCount > 0) {
                    nzVal = min(nzVal, p);
                    for (int zc = c - zCount; zc < c; zc++) result(r, zc) = nzVal;
                }
                nzVal = p;
                zCount = 0;
            }
        }
        for (int c = 0; c < img.cols; c++) {
            const double& p = img.at<double>(r, c);

            if (p > 0) {
                for (int zc = 0; zc < c; zc++) result(r, zc) = p;
                break;
            }
        }
        for (int c = img.cols - 1; c >= 0; c--) {
            const double& p = img.at<double>(r, c);

            if (p > 0) {
                for (int zc = c + 1; zc < img.cols; zc++) result(r, zc) = p;
                break;
            }
        }
    }

    // Fill from bottom and top
    for (int c = 0; c < result.cols; c++) {
        for (int r = 0; r < result.rows; r++) {
            const double& p = result(r, c);

            if (p > 0) {
                for (int zr = 0; zr < r; zr++) result(zr, c) = p;
                break;
            }
        }
        for (int r = result.rows - 1; r >= 0; r--) {
            const double& p = result(r, c);

            if (p > 0) {
                for (int zr = r + 1; zr < result.rows; zr++) result(zr, c) = p;
                break;
            }
        }
    }

    return result;
}

cv::Mat AdjustDisparityImage(const cv::Mat& img)
{
    cv::Mat imgConv;

    img.convertTo(imgConv, CV_64FC1, 1 / 256.);
    return imgConv;
}

string Format(const string& fs, ...)
{
    unique_ptr<char[]> result;
    va_list ap;
    int newn, n = 2 * fs.size();

    va_start(ap, fs);
    while (true) {
        result.reset(new char[n]);
        strcpy(result.get(), fs.c_str());
        newn = vsnprintf(result.get(), n, fs.c_str(), ap);
        if (newn > n) n = newn + 1;
        else if (newn < 0) n *= 2;
        else break;
    }
    va_end(ap);
    return string(result.get());
}

