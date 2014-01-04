#ifndef OUTERCONTOURFINDER_H
#define OUTERCONTOURFINDER_H

#include "OpenCvUtilities.h"
#include "CvBoundingBox.h"

// Find outer contours in a binary image (0,1)
// Algorithm is based on a Suzuki-algorithm
// Ref: Topological Structural Analysis of Digitized Binary
//      Images By Border Following (Satoshi Suziki and Keiichi Abe)
//

// Remark : Algorithm assumes pixelvalues [0,1]. The algorithm
// will fail if other values are present. The image is modified
// with markers

// This algorithm has been obtained from opencv2.3 (contours.cpp) and has
// been optimized for only finding outer-contours. For speeding-up the
// algo, a local memory-buffer is used (size to be specified at configuration)
// which must be able to hold all the contour-points

class OuterContourFinder
{
public:

    // Helper-Class
    // Contours are stored in local memory

    class Contour
    {
    public:
        Contour* Next;
        unsigned long NumberOfPoints;

        // Points are stored in local allocated memory and don't have to be released by the caller

        CvPoint* Points;        // Pointer to an array of points (size of array = NumberOfPoints)

        // Boundingbox of the object described by the contour

        vipnt::CvBoundingBox BoundingBox;
    };

    // Constructor/destructor

    OuterContourFinder();
    virtual ~OuterContourFinder();

    // Setting up => setting available memory for saving the results

    void Configure(int ImageWidth,int ImageHeight,int ImageWidthStep,unsigned long SizeContourBuffer,unsigned long SizePointsBuffer);

    // Get the contours. The contours are valid as long as the object exists, and no
    // other GetContours() or Reset() has been called. The contours are stored into
    // local allocated memory

    const Contour* GetContours(IplImage* Image,unsigned long MinimumRequiredPoints);
    void Reset();
    bool IsConfigured() const;

    // Unit-testing-functions (returning some data for evaluation)

    unsigned long GetNumberOfAllocatedContours() const;
    unsigned long GetNumberOfAllocatedPoints() const;

private :

    int ImageWidth;
    int ImageHeight;
    int ImageWidthStep;

    // Precalculated offsets for accessing the3*3 neighborhood of a pixel. The array in 8*2 size
    // to facilitate processing

    int PixelOffsets[16];

    // Buffers

    Contour* StartFreeContours;     // Start of buffer (where contours are saved)
    Contour* EndFreeContours;
    Contour* Contours;              // Current processed contour

    CvPoint* StartFreePoints;       // Start of buffer where points are stored
    CvPoint* EndFreePoints;
    CvPoint* Points;                // Current processed point

    // Local functions

    void FetchContour(signed char *ptr, CvPoint& pt, Contour* ContourOI);

    OuterContourFinder(const OuterContourFinder&);      // Prevent copy-constructor
};

#endif // OUTERCONTOURFINDER_H
