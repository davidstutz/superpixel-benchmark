#include "stdafx.h"
#include "structures.h"
#include "functions.h"
#include <functional>

// Local functions, structures, classes
///////////////////////////////////////////////////////////////////////////////

class DistinctRandomNumberGenerator {
private:
    std::vector<int> v;
public:
    DistinctRandomNumberGenerator(int N) : v(N) 
    {
        for (int i = 0; i < N; i++) v[i] = i;
    }

    void GetNDistinct(int n, int result[])
    {
        int N = v.size();
        for (int k = 0; k < n; k++) {
            int l = rand() % N;
            result[k] = v[l];
            std::swap(v[l], v[--N]);
        }
    }
};

Matrix<int> BitsToPatch3x3(int b)
{
    Matrix<int> patch(3, 3);
    int bit = 1;

    for (int i = 0; i < 9; i++) {
        patch(i / 3, i % 3) = ((bit & b) == 0) ? 0 : 1;
        bit <<= 1;
    }
    return patch;
}

template<typename T> int Patch3x3ToBits(const Matrix<T>& m, int ulr, int ulc, const function<bool(const T*)>& compare)
{
    int patch = 0;
    int bit = 1;

    for (int r = ulr; r < ulr + 3; r++) {
        if (r >= m.rows)
            break;
        if (r < 0) {
            bit <<= 3;
        } else {
            for (int c = ulc; c < ulc + 3; c++) {
                if (c >= 0 && c < m.cols && compare(m.PtrTo(r, c))) patch |= bit;
                bit <<= 1;
            }
        }
    }
    return patch;
}

int Patch3x3ToBits(const Matrix<Pixel>& m, int ulr, int ulc, Pixel* p)
{
    int patch = 0;
    int bit = 1;
    const Pixel* q;

    for (int r = ulr; r < ulr + 3; r++) {
        if (r >= m.rows)
            break;
        if (r < 0) {
            bit <<= 3;
        } else {
            for (int c = ulc; c < ulc + 3; c++) {
                if (c >= 0 && c < m.cols) {
                    q = m.PtrTo(r, c);
                    if (p != q && q->superPixel == p->superPixel) patch |= bit;
                }
                bit <<= 1;
            }
        }
    }
    return patch;
}


ConnectivityCache::ConnectivityCache()
{
    Initialize();
}

void ConnectivityCache::Initialize()
{
    cache.resize(512);
    for (int i = 0; i < 512; i++) {
        cache[i] = IsPatch3x3Connected(i);
    }
}

void ConnectivityCache::Print()
{
    for (int i = 0; i < 512; i++) {
        Matrix<int> patch = BitsToPatch3x3(i);
        
        cout << cache[i] << ':';
        for (int r = 0; r < 3; r++) {
            cout << '\t';
            for (int c = 0; c < 3; c++) {
                cout << patch(r, c) << ' ';
            }
            cout << endl;
        }
        cout << endl;
    }
}

bool IsPatch3x3Connected(int bits)
{
    Matrix<int> patch = BitsToPatch3x3(bits);
    int rows = 3;
    int cols = 3;
    UnionFind uf(rows*cols);
    cv::Mat1i comp = cv::Mat1i(rows + 1, cols + 1, -1);
    int cCount = 0;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int q = patch(r, c);

            if (q == 1) {
                int topi = comp(r, c + 1);
                int lefti = comp(r + 1, c);

                if (topi < 0) {
                    if (lefti < 0) comp(r + 1, c + 1) = cCount++;
                    else comp(r + 1, c + 1) = lefti;
                }
                else { // topi >= 0
                    if (lefti < 0) comp(r + 1, c + 1) = topi;
                    else {
                        comp(r + 1, c + 1) = lefti;
                        uf.Union(lefti, topi);
                    }
                }
            }
        }
    }
    return uf.Size(0) == cCount;
}

// Static classes
///////////////////////////////////////////////////////////////////////////////

static ConnectivityCache connectivityCache;


// Timer
///////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const Timer& t)
{
    os << ((double)(t.time) / CLOCKS_PER_SEC);
    return os;
}

// Function definitions
///////////////////////////////////////////////////////////////////////////////

// Return length of superpixel boundary -- for debug purposes only it's inefficient
int CalcSuperpixelBoundaryLength(const Matrix<Pixel>& pixelsImg, Superpixel* sp)
{
    int result = 0;

    for (const Pixel& p : pixelsImg) {
        if (p.superPixel == sp) {
            const Pixel* q = PixelAt(pixelsImg, p.row - 1, p.col);
            if (q == nullptr || q->superPixel != sp) result += p.GetCSize();
            q = PixelAt(pixelsImg, p.row + 1, p.col);
            if (q == nullptr || q->superPixel != sp) result += p.GetCSize();
            q = PixelAt(pixelsImg, p.row, p.col - 1);
            if (q == nullptr || q->superPixel != sp) result += p.GetRSize();
            q = PixelAt(pixelsImg, p.row, p.col + 1);
            if (q == nullptr || q->superPixel != sp) result += p.GetRSize();
        }
    }
    return result;
}

// Return length of boundary between Pixel p and superpixel sp (spbl), between Pixel p and superpixel sq (sqbl) and between
// Pixel p and other superpixel (sobl)
void CalcSuperpixelBoundaryLength(const Matrix<Pixel>& pixelsImg, Pixel* p, Superpixel* sp, Superpixel* sq,
    int& spbl, int& sqbl, int& sobl)
{
    spbl = sqbl = sobl = 0;
    
    const Pixel* q; 

    q = PixelAt(pixelsImg, p->row - 1, p->col);
    if (q == nullptr || (q->superPixel != sp && q->superPixel != sq)) sobl += p->GetCSize(); 
    else if (q->superPixel == sp) spbl += p->GetCSize();
    else sqbl += p->GetCSize();
    q = PixelAt(pixelsImg, p->row, p->col - 1);
    if (q == nullptr || (q->superPixel != sp && q->superPixel != sq)) sobl += p->GetRSize();
    else if (q->superPixel == sp) spbl += p->GetRSize();
    else sqbl += p->GetRSize();
    q = PixelAt(pixelsImg, p->row + 1, p->col);
    if (q == nullptr || (q->superPixel != sp && q->superPixel != sq)) sobl += p->GetCSize();
    else if (q->superPixel == sp) spbl += p->GetCSize();
    else sqbl += p->GetCSize();
    q = PixelAt(pixelsImg, p->row, p->col + 1);
    if (q == nullptr || (q->superPixel != sp && q->superPixel != sq)) sobl += p->GetRSize();
    else if (q->superPixel == sp) spbl += p->GetRSize();
    else sqbl += p->GetRSize();
}

// Checks whether Pixel p separates Pixel q at (p->row + dRow, p->col + dCol) from superpixel sp
// At exactly one of dRow and dCol must be 0 and -1 <= dRow, dCol <= 1
//bool SeparatingPixel(const cv::Mat_<Pixel*>& pixelsImg, Pixel* p, int dRow, int dCol)
//{
//    Pixel* q = PixelAt(pixelsImg, p->row + dRow, p->col + dCol);
//
//    if (q == nullptr || p->superPixel != q->superPixel) return false; // on border || not in the same superpixel
//    if (dRow != 0) return q->bLeft && q->bRight;
//    if (dCol != 0) return q->bTop && q->bBottom;
//    return false;
//}
    

// Move Pixel p from superpixel of p to superpixel of q
void MovePixel(Matrix<Pixel>& pixelsImg, PixelMoveData& pmd)
{
    Pixel* p = pmd.p;
    Pixel* q = pmd.q;
    Superpixel* sp = p->superPixel;
    Superpixel* sq = q->superPixel;
    int spbl, sqbl, sobl;

    CalcSuperpixelBoundaryLength(pixelsImg, p, sp, sq, spbl, sqbl, sobl);

    // Change precalculated sums
    sp->RemovePixel(pmd.pixelData);
    sp->AddToBorderLength(spbl - sqbl - sobl);
    sq->AddPixel(pmd.pixelData);
    sq->AddToBorderLength(spbl - sqbl + sobl);

    // Fix border information
    Pixel* r;
    
    if ((r = PixelAt(pixelsImg, p->row, p->col - 1)) != nullptr && (r->superPixel == sq || r->superPixel == sp)) {
        r->SwapBRight();
        p->SwapBLeft();
    }
    if ((r = PixelAt(pixelsImg, p->row, p->col + 1)) != nullptr && (r->superPixel == sq || r->superPixel == sp)) {
        r->SwapBLeft();
        p->SwapBRight();
    }
    if ((r = PixelAt(pixelsImg, p->row - 1, p->col)) != nullptr && (r->superPixel == sq || r->superPixel == sp)) {
        r->SwapBBottom();
        p->SwapBTop();
    }
    if ((r = PixelAt(pixelsImg, p->row + 1, p->col)) != nullptr && (r->superPixel == sq || r->superPixel == sp)) {
        r->SwapBTop();
        p->SwapBBottom();
    }

}

void MovePixelStereo(Matrix<Pixel>& pixelsImg, PixelMoveData& pmd, bool changeBoundary)
{
    MovePixel(pixelsImg, pmd);

    if (changeBoundary) {
        SuperpixelStereo* sp = (SuperpixelStereo*)pmd.p->superPixel;
        SuperpixelStereo* sq = (SuperpixelStereo*)pmd.q->superPixel;

        // Update boundary
        std::swap(sp->boundaryData, pmd.bDataP);
        std::swap(sq->boundaryData, pmd.bDataQ);

        // Update neighboring boundaries
        for (auto& bdIter : sp->boundaryData) {
            if (bdIter.first != sq) bdIter.first->boundaryData[sp] = bdIter.second;
        }
        for (auto& bdIter : sq->boundaryData) {
            if (bdIter.first != sp) bdIter.first->boundaryData[sq] = bdIter.second;
        }

        // Remove neighborhood data of sp, if any
        for (SuperpixelStereo* sr : pmd.prem) {
            sr->boundaryData.erase(sp);
        }
    }

}

// Return true if superpixel sp is connected in region defined by upper left/lower right corners of pixelsImg
// Corners are adjusted to be valid for image but we assume that lrr >= 0 and lrc >= 0 and ulr < pixelsImg.rows and ulc < pixelsImg.cols
bool IsSuperpixelRegionConnected(const Matrix<Pixel>& pixelsImg, Pixel* p, int ulr, int ulc, int lrr, int lrc)
{
    if (ulr < 0) ulr = 0;
    if (ulc < 0) ulc = 0;
    if (lrr >= pixelsImg.rows) lrr = pixelsImg.rows - 1;
    if (lrc >= pixelsImg.cols) lrc = pixelsImg.cols - 1;

    int rows = lrr - ulr;
    int cols = lrc - ulc;
    UnionFind uf(rows * cols);
    cv::Mat1i comp = cv::Mat1i(rows + 1, cols + 1, -1);
    int cCount = 0;

    for (int r = ulr; r < lrr; r++) {
        int ir = r - ulr + 1;

        for (int c = ulc; c < lrc; c++) {
            const Pixel* q = &pixelsImg(r, c);

            if (p != q && q->superPixel == p->superPixel) {
                int ic = c - ulc + 1;
                int topi = comp(ir - 1, ic);
                int lefti = comp(ir, ic - 1);

                if (topi < 0) {
                    if (lefti < 0) comp(ir, ic) = cCount++;
                    else comp(ir, ic) = lefti;
                } else { // topi >= 0
                    if (lefti < 0) comp(ir, ic) = topi;
                    else {
                        comp(ir, ic) = lefti;
                        uf.Union(lefti, topi);
                    }
                }
            }
        }
    }
    return uf.Size(0) == cCount;
}

// See IsSuperpixelRegionConnected; optimized version which uses pre-calculated connectivity data for 3x3 regions
bool IsSuperpixelRegionConnectedOptimized(const Matrix<Pixel>& pixelsImg, Pixel* p, int ulr, int ulc, int lrr, int lrc)
{
    CV_DbgAssert(ulr + 3 == lrr && ulc + 3 == lrc);

    int bits = Patch3x3ToBits(pixelsImg, ulr, ulc, p);

    return connectivityCache.IsConnected(bits);
}

// RANSAC & supporting functions
///////////////////////////////////////////////////////////////////////////////

// Try to fit plane to points
bool Plane3P(const cv::Point3d& p1, const cv::Point3d& p2, const cv::Point3d& p3, Plane_d& plane)
{
    cv::Point3d normal = (p1 - p2).cross(p1 - p3);
    if (normal.z < eps && -normal.z < eps) 
        return false;
    else {
        plane.x = -normal.x/normal.z;
        plane.y = -normal.y/normal.z;
        plane.z = p1.z - plane.x*p1.x - plane.y*p1.y;
    }
    return true;
}

bool Plane3PDebug(const cv::Point3d& p1, const cv::Point3d& p2, const cv::Point3d& p3, Plane_d& plane)
{
    cv::Mat1d left = (cv::Mat1d(3, 3) << p1.x, p1.y, 1.0, p2.x, p2.y, 1.0, p3.x, p3.y, 1.0);
    cv::Mat1d right = (cv::Mat1d(3, 1) << p1.z, p2.z, p3.z);
    cv::Mat1d result;

    if (cv::solve(left, right, result, cv::DECOMP_CHOLESKY)) {
        plane.x = result(0, 0);
        plane.y = result(1, 0);
        plane.z = result(2, 0);
        return true;
    } else {
        return false;
    }
}


// See http://en.wikipedia.org/wiki/RANSAC
// In our case, it will be n = 3, p = 0.99
int EstimateRANSACSteps(int n, int nInliers, int nPoints, double p)
{
    double w = (double)nInliers / nPoints;

    if (w == 1.0) return 1;
    return (int)round(log(1 - p) / log(1 - pow(w, n))); 
}

// Returns false if pixels.size() < 3 or no 3 points were found to 
// form a plane; pixels are (xi, yi, di)
// Plane parameters (a, b, c) satisfy equations (xi, yi, 1).(a, b, c) == di
// for selected three points
bool RANSACPlane(const vector<cv::Point3d>& pixels, Plane_d& plane)
{
    const double inlierThreshold = 1.0;
    const double confidence = 0.99;

    if (pixels.size() < 3)
        return false;

    int bestInlierCount = 0;
    Plane_d stepPlane;
    int N = 2*pixels.size();
    int n = 0;

    while (n < N) {
        const cv::Point3d& p1 = pixels[rand() % pixels.size()];
        const cv::Point3d& p2 = pixels[rand() % pixels.size()];
        const cv::Point3d& p3 = pixels[rand() % pixels.size()];

        if (Plane3P(p1, p2, p3, stepPlane)) {
            int inlierCount = 0;

            for (const cv::Point3d& p : pixels) {
                if (fabs(p.x*stepPlane.x + p.y*stepPlane.y + stepPlane.z - p.z) < inlierThreshold) {
                    inlierCount++;
                }
            }
            if (inlierCount > bestInlierCount) {
                bestInlierCount = inlierCount;
                plane = stepPlane;

                int NN = EstimateRANSACSteps(3, bestInlierCount, pixels.size(), confidence);
                
                if (NN < N) N = NN;
            }
        }
        n++;
    }
    return bestInlierCount > 0;
}


// Should be thread-safe!
void InitSuperpixelPlane(SuperpixelStereo* sp, const cv::Mat1d& depthImg)
{
    // We do not re-initialize the plane!
    if (sp->plane != zeroPlane)
        return;

    vector<cv::Point3d> pixels;

    pixels.reserve(sp->GetSize());
    for (Pixel* p : sp->pixels) {
        p->AddDispPixels(depthImg, pixels);
    }
    
    // Sort pixels to de-randomize thing (for debug purposes)
    sort(pixels.begin(), pixels.end(), [](const cv::Point3d& p1, const cv::Point3d& p2) { 
        return p1.x < p2.x || p1.x == p2.x && p1.y < p2.y || p1.x == p2.x && p1.y == p2.y && p1.z < p2.z;  
    });

    if (!RANSACPlane(pixels, sp->plane)) {
        sp->plane = Plane_d(0, 0, 0);
    } else {
        // Least squares re-estimation

        double sumRow = 0;
        double sumRow2 = 0;
        double sumCol = 0;
        double sumCol2 = 0;
        double sumRowCol = 0;
        double sumRowD = 0.0;
        double sumColD = 0.0;
        double sumD = 0.0;
        int n = 0;

        for (const cv::Point3d& p : pixels) {
            if (fabs(DotProduct(sp->plane, p.x, p.y, 1.0) - p.z) < 1.0) {
                sumRow += p.x; sumCol += p.y;
                sumRow2 += p.x*p.x; sumCol2 += p.y*p.y;
                sumRowCol += p.x*p.y;
                sumColD += p.y*p.z; sumRowD += p.x*p.z;
                sumD += p.z;
                n++;
            }
        }

        LeastSquaresPlane(sumRow, sumRow2, sumCol, sumCol2, sumRowCol, sumRowD, sumColD, sumD, n, sp->plane);
    }
}

void CalcOccSmoothnessEnergy(SuperpixelStereo* sp, SuperpixelStereo* sq, double occWeight, double hingeWeight,
    double& ePrior, double& eSmo)
{
    ePrior = occWeight;
    eSmo = 0;
}

void CalcCoSmoothnessSum(const cv::Mat1d& depthImg, double inlierThresh, SuperpixelStereo* sp, SuperpixelStereo* sq, double& eSmo, int& count)
{
    eSmo = 0;
    count = 0;

    if (sp->pixels.empty() && sq->pixels.empty())
        return;

    for (Pixel* p : sp->pixels) {
        p->AddToCoSmoothnessSum(depthImg, inlierThresh, sp->plane, sq->plane, eSmo, count);
    }
    for (Pixel* q : sq->pixels) {
        q->AddToCoSmoothnessSum(depthImg, inlierThresh, sp->plane, sq->plane, eSmo, count);
    }
}

void AddToCoSmoothnessSum(SuperpixelStereo* sp, const Plane_d& planeq, double& eSmo)
{
    double ppx = sp->plane.x, ppy = sp->plane.y, ppz = sp->plane.z;
    double qpx = planeq.x, qpy = planeq.y, qpz = planeq.z;

    eSmo += Sqr(ppz - qpz)*sp->size;
    eSmo += 2 * (ppx - qpx)*(ppz - qpz)*sp->sumRow + Sqr(ppx - qpx) * sp->sumRow2;
    eSmo += 2 * (ppy - qpy)*(ppz - qpz)*sp->sumCol + Sqr(ppy - qpy) * sp->sumCol2;
    eSmo += 2 * (ppx - qpx)*(ppy - qpy)*sp->sumRowCol;
}

void CalcCoSmoothnessSum2Debug(SuperpixelStereo* sp, SuperpixelStereo* sq, double& eSmo, int& count)
{
    eSmo = 0;
    count = 0;

    if (sp->pixels.empty() && sq->pixels.empty())
        return;

    for (Pixel* p : sp->pixels) {
        p->AddToCoSmoothnessSum(sp->plane, sq->plane, eSmo, count);
    }
    for (Pixel* q : sq->pixels) {
        q->AddToCoSmoothnessSum(sp->plane, sq->plane, eSmo, count);
    }
}

void CalcCoSmoothnessSum2(SuperpixelStereo* sp, SuperpixelStereo* sq, double& eSmo, int& count)
{
    eSmo = 0;
    count = sp->size + sq->size;

    if (count > 0) {
        AddToCoSmoothnessSum(sp, sq->plane, eSmo);
        AddToCoSmoothnessSum(sq, sp->plane, eSmo);
    }

    /*
    double eSmoDebug = 0;
    int countDebug = 0;

    CalcCoSmoothnessSum2Debug(sp, sq, eSmoDebug, countDebug);
    assert(fabs(eSmo - eSmoDebug) < 0.01 && count == countDebug);
    */
}


/*
void CalcBTEnergy(SuperpixelStereo* sp, SuperpixelStereo* sq, double occWeight, double hingeWeight,
    double& ePrior, double& eSmo)
{
    int type = sp->boundaryType[sq];

    if (type == BTLo || type == BTRo) {
        ePrior = occWeight;
        eSmo = 0;
    } else if (type == BTHi) {
        ePrior = hingeWeight;
        eSmo = CalcHiSmoothnessEnergy(sp, sq);
    } else if (type == BTCo) {
        ePrior = 0;
        eSmo = CalcCoSmoothnessEnergy(sp, sq);
    }
}
*/

bool LeastSquaresPlaneDebug(const double x1, const double y1, const double z1, const double d1,
    const double x2, const double y2, const double z2, const double d2,
    const double x3, const double y3, const double z3, const double d3,
    Plane_d& plane)
{
    const double epsilonValue = 1e-10;

    double denominatorA = (x1*z2 - x2*z1)*(y2*z3 - y3*z2) - (x2*z3 - x3*z2)*(y1*z2 - y2*z1);
    if (denominatorA < epsilonValue) {
        //plane.x = 0.0;
        //plane.y = 0.0;
        //plane.z = -1.0;
        return false;
    }

    plane.x = ((z2*d1 - z1*d2)*(y2*z3 - y3*z2) - (z3*d2 - z2*d3)*(y1*z2 - y2*z1)) / denominatorA;

    double denominatorB = y1*z2 - y2*z1;
    if (denominatorB > epsilonValue) {
        plane.y = (z2*d1 - z1*d2 - plane.x * (x1*z2 - x2*z1)) / denominatorB;
    } else {
        denominatorB = y2*z3 - y3*z2;
        plane.y = (z3*d2 - z2*d3 - plane.x * (x2*z3 - x3*z2)) / denominatorB;
    }
    if (z1 > epsilonValue) {
        plane.z = (d1 - plane.x * x1 - plane.y * y1) / z1;
    } else if (z2 > epsilonValue) {
        plane.z = (d2 - plane.x * x2 - plane.y * y2) / z2;
    } else {
        plane.z = (d3 - plane.x * x3 - plane.y * y3) / z3;
    }
}


