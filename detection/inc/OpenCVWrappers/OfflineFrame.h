#ifndef OFFLINEFRAME_H
#define OFFLINEFRAME_H

#include <vector>

namespace vipnt
{
// OfflineFrame contains the raw grabbed Y-frame from the grabber

// Remark: This type of image stores all the pixels in a subsequently
// (row1} ... {rowN}. The width-step (cfr. opencv) is therefore = number
// of columns. For speed-optimization, it can be useful to skip
// the classical row/column-forloops and replace them by one loop

class OfflineFrame
{
public:
    OfflineFrame();
    virtual ~OfflineFrame();

    // Reset the image

    void Reset();
    void Reset(unsigned char ResetValue);

    // Set the image

    void Resize(unsigned int Width, unsigned int Height);
    void Set(unsigned char Value= 0);
    void Set(unsigned int Width, unsigned int Height,unsigned char Value= 0);

    // The set-function underneath is a copy-utility

    void Set(unsigned int Width, unsigned int Height, const unsigned char* Data);

    // SampleWidth : 1 .. N : 1 = samples each pixel of a column, 2 = samples every 2 pixels, ...
    // SampleHeight : 1 .. N : 1 = samples each row, 2 = samples every 2 rows, ...

    void SetSubsampled(unsigned int SampleWidth,unsigned int SampleHeight, const OfflineFrame* ImageOI);
    void SetSubsampled(unsigned int Width, unsigned int Height,unsigned int SampleWidth,unsigned int SampleHeight, const unsigned char* Data);

    // OpenCV-stubs (Widthstep can differ from width)

    void Set(unsigned int Width, unsigned int Height, unsigned int WidthStep,const unsigned char* Data);
    void SetSubsampled(unsigned int Width, unsigned int Height, unsigned int WidthStep,unsigned int SampleWidth,unsigned int SampleHeight, unsigned int NumChs, const unsigned char* Data);

    // Set (is a copy-image)

    void Set(const OfflineFrame& FrameOI);
    
	/*!
		Swaps the image data with the incoming image. Make sure that the images have
		identical sizes!
		\param[in] Image Image to swap the content with the current object.
	*/
	
    void Swap(OfflineFrame& FrameOI);

    // Image-properties

    inline const unsigned char* GetData() const { return &Data.front(); }
    inline unsigned char* GetData() { return &Data.front(); }
    inline unsigned int GetWidth() const { return Width; } //!< Get the width of the image.
    inline unsigned int GetHeight() const { return Height; } //!< Get the height of the image.
    inline unsigned int GetSize() const { return Width * Height; }

    // Test-functionalities
    // Note : squares will be centered in the image

    void CreateCheckerBoard(unsigned int SizeOfSquareSide,unsigned char GraylevelBackground, unsigned char GraylevelForeground);
    void SetPixel(unsigned int Column, unsigned int Row,unsigned char Value);

    // Return true if the size and the data matches those of the frame referenced

    bool CheckIfEqual(const OfflineFrame& FrameOI) const;

    // Pixel returned == 0 in case of an undefined image, or coordinates
    // out of bounds

    unsigned char GetPixel(unsigned int Column, unsigned int Row) const;

private:

    std::vector<unsigned char> Data;
    unsigned int Width;
    unsigned int Height;
};

} // namespace vipnt

#endif // OfflineFrame_H
