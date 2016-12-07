#include <string>
#include <vector>
#include <functional>

using namespace std;

// Returns list of files in dir following pattern. See implementation comment for restrictions.
void FindFiles(const string& dir, const string& pattern, vector<string>& files, bool fullPath = false);

// Returns fileName with extension replaced by newExt (extension is substring of fileName from (including) the last dot)
string ChangeExtension(const string& fileName, const string& newExt);

// Extracts path name from fileName (appended with '/' inf not empty)
string FilePath(const string& fileName);

string FileName(const string& fileName);

// Append '/' to the end of dirName if not present and non-empty
void EndDir(string& dirName);
void MkDir(const string& dirName);

string Format(const string& fs, ...);

// From Jian's original code
cv::Mat ConvertRGBToLab(const cv::Mat& img);

// Input is single channel ushort image, output is single channel double image
// Input is divided by 256.0 and gaps (regions with value 0) are "filled", 
cv::Mat AdjustDisparityImage(const cv::Mat& img);

cv::Mat FillGapsInDisparityImage(const cv::Mat& img);

cv::Mat InpaintDisparityImage(const cv::Mat& img);

struct StatData {
    double mean, var, min, max;
};

template<typename It, typename Func>
void MeanAndVariance(It first, It last, Func f, StatData& stat)
{
    double s = 0.0;
    int N = 0;
    stat.max = DBL_MIN;
    stat.min = DBL_MAX;

    for (It iter = first; iter != last; iter++) {
        double val = f(*iter);
        if (val > stat.max) stat.max = val;
        if (val < stat.min) stat.min = val;
        s += val;
        N++;
    }
    if (N == 0) {
        stat.mean = stat.var = 0;
        stat.max = stat.min = 0;
        return;
    }
    stat.mean = s / N;
    if (N == 1) {
        stat.var = 0;
        return;
    }
    s = 0.0;
    for (It iter = first; iter != last; iter++) {
        s += Sqr(f(*iter) - stat.mean);
    }
    stat.var = sqrt(s / (N - 1));
}
