#include "OuterContourFinder.h"
#include <stdio.h>

OuterContourFinder::OuterContourFinder()
{
    // Buffers

    StartFreeContours = 0;
    StartFreePoints = 0;
    Reset();
}

OuterContourFinder::~OuterContourFinder()
{
    if (StartFreeContours != 0) delete[] StartFreeContours;
    if (StartFreePoints != 0) delete[] StartFreePoints;
}

void OuterContourFinder::Reset()
{
    if (StartFreeContours != 0) delete[] StartFreeContours;
    if (StartFreePoints != 0) delete[] StartFreePoints;
    ImageWidth = 0;
    ImageHeight = 0;
    ImageWidthStep = 0;

    // Buffers

    StartFreeContours = 0;
    EndFreeContours = 0;
    StartFreePoints = 0;
    EndFreePoints = 0;
    Contours = StartFreeContours;
    Points = StartFreePoints;
}

bool OuterContourFinder::IsConfigured() const
{
    return ((StartFreeContours != 0) && (StartFreePoints != 0));
 }

// Unit-testing-functions (returning some data for evaluation)

unsigned long OuterContourFinder::GetNumberOfAllocatedContours() const
{
    if (StartFreeContours == 0) return 0;
    return (EndFreeContours - StartFreeContours) / sizeof(class OuterContourFinder::Contour);
}

unsigned long OuterContourFinder::GetNumberOfAllocatedPoints() const
{
    if (StartFreePoints == 0) return 0;
    return (EndFreePoints - StartFreePoints) / sizeof(class CvPoint);
}

void OuterContourFinder::Configure(int ImageWidth,int ImageHeight,int ImageWidthStep,unsigned long SizeContourBuffer,unsigned long SizePointsBuffer)
{
    this->ImageWidth = ImageWidth;
    this->ImageHeight = ImageHeight;
    this->ImageWidthStep = ImageWidthStep;

    // Allocate storage for fast processing

    if ((SizeContourBuffer > 0) && (SizePointsBuffer > 0))
    {
        StartFreeContours= new Contour[SizeContourBuffer];
        if (StartFreeContours != 0)
        {
            EndFreeContours = StartFreeContours + SizeContourBuffer;
            StartFreePoints = new CvPoint[SizePointsBuffer];
            EndFreePoints = StartFreePoints  + SizePointsBuffer;
        }
    }

    // Create offset-arrays for fast accessing the3*3 neighborhood of a pixel
    //
    // Offsets
    //  +---+---+---+
    //  | 3 | 2 | 1 |
    //  +---+---+---+
    //  | 4 | * | 0 |
    //  +---+---+---+
    //  | 5 | 6 | 7 |
    //  +---+---+---+
    //
    // CW-scanning => decrease offsets
    // CCW-scanning = increase offsets

    // 2 arrays are used for faster-processing (avoiding mask-operation
    // in the 2nd ++loop of fetch-contour)

    PixelOffsets[0] = PixelOffsets[8] = 1;
    PixelOffsets[1] = PixelOffsets[9] = -ImageWidthStep + 1;
    PixelOffsets[2] = PixelOffsets[10] = -ImageWidthStep;
    PixelOffsets[3] = PixelOffsets[11] = -ImageWidthStep - 1;
    PixelOffsets[4] = PixelOffsets[12] = - 1;
    PixelOffsets[5] = PixelOffsets[13] = ImageWidthStep - 1;
    PixelOffsets[6] = PixelOffsets[14] = ImageWidthStep;
    PixelOffsets[7] = PixelOffsets[15] = ImageWidthStep + 1;
}

// See PixelOffsets above

static const CvPoint CodeDeltasContourFinder[8] =
{ {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1} };


// Marks domain border with +/-<constant> and stores the contour into ContourOI.
// Method = simple approximation
//
// Comments refer to the article of Satoshi Suziki

void OuterContourFinder::FetchContour(signed char *ptr, CvPoint& pt, Contour* ContourOI)
{
    int w1= 0;
    int w2 = 0;
    int w3 = 0;
    int w4 = 0;

    signed char *i0 = ptr;
    signed char *i1, *i3, *i4;
    int s = 4;
    int s_end = 4;

    // BoundingBox-processing vars

    int Left;           // Smallest x
    int Right;          // Largest x
    int Top;            // Smallest y
    int Bottom;         // Largest y

    // (3) From the starting point (i, j), follow the detected border: this is done by
    // the following substeps (3.1) through (3.5).

    // (3.1) Starting from (i2, j2), look around clockwise the pixels in the neigh-
    // borhood of (i, j) and tind a nonzero pixel. Let (i1, j1) be the first found nonzero
    // pixel. If no nonzero pixel is found, assign -NBD to fij and go to (4).

    // (i2,j2) = (i,j-1) (from 1a. : outer-border)
    // 9*9-neigboroffset = 4 (see PixelOffsets)
    // Clockwise-scanning => decrease offsets (see definition of PixelOffsets)

    do
    {
        s = (s - 1) & 7;
        i1 = i0 + PixelOffsets[s];
        if( *i1 != 0 ) break; // 1st non-zero  pixel
    }
    while( s != s_end );

    if( s == s_end )            // single pixel domain
    {
        // Single pixel domain -> skip this (contour NOI)

        // In the original code (from opencv), this pixel is set to -2 (see
        // commented code). As we are not interested in single-pixel-domains, it
        // is not required to set this pixel to the specified value (saves an
        // unnecessary memory-write-operation). Note also, as we are not interested
        // in such contours, that the function doesn't return the pixel involved.

        *i0 = (2 | -128);
        return;
    }

    // (3.2) (i2,j2) <- (i1,j1) and (i3,j3) <- (i,j)

    i3 = i0;

    // (3.3) Starting from the next element of the pixel (i2, j2,) in the counterclock-
    // wise order, examine counterclockwise the pixels in the neighborhood of the current
    // pixel (i3, j3,) to find a nonzero pixel and let the first one be (i4, j4).

    // follow border in counterclockwise (increase offsets)
    for( ;; )
    {
        w3++;
        s_end = s;

        for( ;; )
        {
            w4++;
            i4 = i3 + PixelOffsets[++s];
            if( *i4 != 0 ) break;       // 1st non-zero  pixel
        }
        s &= 7;

        // (3.4) Change the value of the pixel (i3, j3) as follows:
        // (a) If the pixel (i3, j3 + 1) is a O-pixel examined in the substep (3.3)
        // then fi3, j3 <- -NBD.
        // (b) If the pixel (i3, j3 + 1) is not a O-pixel examined in the substep
        // (3.3) and fi3,j3 = 1, then fi3,j3 <- NBD.
        // (c) Otherwise, do not change fi3, j3.

        // check "right" bound
        if( (unsigned) (s - 1) < (unsigned) s_end )
        {
            *i3 = (signed char) (2 | -128);
            w1++;
        }
        else if( *i3 == 1 )
        {
            *i3 = 2;
            w2++;
        }

        // Add point to contour

        if (Points >= EndFreePoints)
        {
            // Emergency-exit => running out of memory
            // Reset number of points (incomplete contour)

            ContourOI->NumberOfPoints = 0;
            return;
        }

        if (ContourOI->NumberOfPoints++ == 0)
        {
            // Init boundingbox-vars

            Left = Right = pt.x;
            Top = Bottom = pt.y;
        }
        else
        {
            // Update boundingbox

            if (Left > pt.x) Left = pt.x;
            else if (Right < pt.x) Right = pt.x;
            if (Top > pt.y) Top = pt.y;
            else if (Bottom < pt.y) Bottom = pt.y;
        }

        Points->x = pt.x;
        Points->y = pt.y;
        Points++;

        // (3.5) If (i4, j4) = (i, j) and (i3, j3) = (i1, j1) (coming back to the starting
        // point), then go to (4); otherwise, (i2, j2) <- (i3, j2),(i3, j3) <- (i4, j4), and go back
        // to (3.3).

        pt.x += CodeDeltasContourFinder[s].x;
        pt.y += CodeDeltasContourFinder[s].y;

        if (( i4 == i0) && ( i3 == i1 )) break;

        i3 = i4;
        s = (s + 4) & 7;
    } //  end of border following loop

    // Set BoundingBox

    ContourOI->BoundingBox.Set(Left,Top,Right-Left+1,Bottom-Top+1);
}

// Description of the algorithm : Topological Structural Analysis of Digitized Binary
//    Images By Border Following (Satoshi Suziki and Keiichi Abe)
//
// This function implements only the following of the outer contours and skips the
// processing of the holes. The main algorithm is described in Appendix I: The formal
// description of algorithm 1. This function is described in section 4: The border following
// algorithm for extracting only the outermost borders -> marking-policy is using values
// 2 and -2 i.s.o. NBD and -NBD
//
// This function assumes that the difference-image only consists of zero's and ones. Any other
// value will make the algo fail. Note also that the difference-image is changed after
// calling this function, because the algo is setting specific markers into the image
//
// Note that the contour consists of the double number of points if the contour is single-ended
//
// All data is saved in local preallocated memory (for this reason the returned list of
// contours is const and can't be released nor be manipulated by the caller of this function
//
// The contours are chained into a list. Each contour points to a array of subsequent contour-points.
// If not enough memory has been preallocated, the algo will stop processing. In the latter case,
// not all contours are returned.
//
// Comments refer to the article of Satoshi Suziki

const OuterContourFinder::Contour* OuterContourFinder::GetContours(IplImage* Image,unsigned long MinimumRequiredPoints)
{
    // Memory configured ?

    if (StartFreeContours == 0) return 0;       // Can't return anything, as no local storage is available

    // Reset Pointers

    Contours = StartFreeContours;
    StartFreeContours->Next = 0;
    StartFreeContours->NumberOfPoints = 0;
    StartFreeContours->Points = 0;
    Points = StartFreePoints;

    // Reset borders

    {
        CvSize FrameSize = cvGetSize(Image);
        unsigned char* img = (unsigned char*) Image->imageData;
        int step = Image->widthStep;
        memset( img, 0, FrameSize.width );
        memset( img + step * (FrameSize.height - 1), 0,FrameSize.width );
        int y;
        for(  y = 1, img += step; y < FrameSize.height - 1; y++, img += step )
        {
            img[0] = img[FrameSize.width - 1] = 0;
        }
    }

    // Start processing

    if (Image == 0) return 0;

    int ScanWidth = ImageWidth - 1;   /* exclude rightest column */
    int ScanHeight = ImageHeight - 1; /* exclude bottomost row */

    // Setup

    CvPoint ScanPoint;
    ScanPoint.x = ScanPoint.y = 1;

    CvPoint ScanOffset;
    ScanOffset.x = 0;
    ScanOffset.y = 0;
    signed char *ImageDataPointer = (signed char* ) Image->imageData + ImageWidthStep;

    int PreviousValue = 0;
    int x = ScanPoint.x;
    int y = ScanPoint.y;

    for( ; y < ScanHeight; y++, ImageDataPointer += ImageWidthStep)
    {
        for( ; x < ScanWidth; x++ )
        {
            int p = ImageDataPointer[x];
            if (p != PreviousValue)
            {
                // external contour ?
                // 1a. if fij = 1 and fi,j-1 = 0, then decide that the pixel (i,j) is the
                // border following starting pointnof an outer border, increment NBD and
                // (i2,j2) <- (i,j-1)

                if ((PreviousValue == 0) && (p == 1))
                {
                    // Determine the parent border (not required as this is only
                    // necessary for algorithm 1 dealing with holes and outer borders

                    CvPoint* StartContourPoints = Points;
                    Contour* ContourOI = Contours++;

                    // Running out of local memory-storage ?

                    if (ContourOI == EndFreeContours)
                    {
                        // Emergency exit -> return the yet found contours

                        if (StartFreeContours->NumberOfPoints > 0) return StartFreeContours;
                        else return 0;
                    }

                    ContourOI->NumberOfPoints = 0;
                    ContourOI->Points = StartContourPoints;

                    CvPoint POI;
                    POI.x = x + ScanOffset.x;
                    POI.y = y + ScanOffset.y;

                    FetchContour( ImageDataPointer + x ,
                                  POI,
                                  ContourOI);

                    // Accept Contour ?

                    if (ContourOI->NumberOfPoints >= MinimumRequiredPoints)
                    {
                        // ContourOI (adjust linkage)

                        ContourOI->Next = 0;
                        if (ContourOI != StartFreeContours)
                        {
                            Contour* PreviousContour = (Contours-2);
                            PreviousContour->Next = ContourOI;
                        }
                    }
                    else
                    {
                        // Reject contour (Not enough pixels)

                        Points = StartContourPoints;
                        Contours--;
                    }

                }  // end outer border

                // (4) If fij != 1, then LNBD <- |fij| and resume the raster scan from the pixel
                //  (i, j + 1). The algorithm terminates when the scan reaches the lower right corner of
                //  the picture.

                PreviousValue = p;
            }  // end of prev != p
        } // End loop over columns (x)

        x = 1;
        PreviousValue = 0;
    }   // End loop of rows (y)

    if (StartFreeContours->NumberOfPoints > 0) return StartFreeContours;
    else return 0;
}
