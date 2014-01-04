#include "OfflineFrame.h"

namespace vipnt
{

OfflineFrame::OfflineFrame()
{
    Width = 0;
    Height = 0;
}

OfflineFrame::~OfflineFrame()
{
}

// Reset the image

void OfflineFrame::Reset()
{
    Width = 0;
    Height = 0;
    Data.clear();
}

void OfflineFrame::Reset(unsigned char ResetValue)
{
    Data.assign(Data.size(), ResetValue);
}

void OfflineFrame::Resize(unsigned int Width, unsigned int Height)
{
    unsigned int Size = Height * Width;
    Data.resize(Size);
    this->Width = Width;
    this->Height = Height;
}

void OfflineFrame::Set(unsigned char Value)
{
    unsigned int Size = Height * Width;
    if (Size != 0) Data.assign(Data.size(), Value);
}

void OfflineFrame::Set(unsigned int Width,unsigned int Height,unsigned char Value)
{
    unsigned int Size = Height * Width;
    if ((this->Width != Width) || (this->Height != Height))
    {
        // Resize

        Data.resize(Size);
         this->Width = Width;
        this->Height = Height;
    }

    // Set to the specified value

    Data.assign(Size, Value);
}

void OfflineFrame::Set(unsigned int Width,unsigned int Height, const unsigned char* Data)
{
    unsigned int Size = Height * Width;
    this->Data.resize(Size);
    this->Width = Width;
    this->Height = Height;
    if (Size > 0) std::copy(Data, Data + Size, GetData());
}

void OfflineFrame::SetSubsampled(unsigned int SampleWidth,unsigned int SampleHeight, const OfflineFrame* ImageOI)
{
    if (ImageOI != 0) SetSubsampled(ImageOI->GetWidth(),ImageOI->GetHeight(),SampleWidth,SampleHeight,ImageOI->GetData());
}

void OfflineFrame::SetSubsampled(unsigned int Width, unsigned int Height,unsigned int SampleWidth,unsigned int SampleHeight, const unsigned char* Data)
{
    if ((Data == 0)
            || (SampleWidth == 0)
            || (SampleHeight == 0)) return;     // Invalid input-parameters

    // Calculate dimensions of the subsampled-image

    this->Width = Width/SampleWidth;
    this->Height = Height/SampleHeight;

    // In case the sample-distance isn't a divider of the according
    // dimension, then increment the according subsampled dimension

    if (Width%SampleWidth != 0) this->Width += 1;
    if (Height%SampleHeight != 0) this->Height += 1;

    unsigned int Size = (this->Width*this->Height);
    this->Data.resize(Size);
    unsigned int RowStep = (Width*SampleHeight);

    if (Size > 0)
    {
        const unsigned char* Source = Data;
        unsigned char* Target = GetData();
        for (unsigned int y = 0; y < this->Height; y++)
        {
            const unsigned char* SourceData = Source;
            for (unsigned int x = 0; x < this->Width; x++)
            {
                *Target++ = *SourceData;
                SourceData += SampleWidth;
            } // end columns
            Source += RowStep;
        } // end rows
    } // end non-empty image
}

void OfflineFrame::Swap(OfflineFrame& FrameOI)
{
    std::swap(Height, FrameOI.Height);
    std::swap(Width, FrameOI.Width);
    Data.swap(FrameOI.Data);
}

void OfflineFrame::Set(const OfflineFrame& FrameOI)
{
    Set(FrameOI.GetWidth(),FrameOI.GetHeight(),FrameOI.GetData());
}

void OfflineFrame::SetPixel(unsigned int Column, unsigned int Row,unsigned char Value)
{
    if ((Width > 0)
            && (Height > 0)
            && (Column < Width)
            && (Row < Height))  *(GetData() + (Row*Width) + Column) = Value;
}

unsigned char OfflineFrame::GetPixel(unsigned int Column, unsigned int Row) const
{
    unsigned char Value = 0;
    if ((Width > 0)
            && (Height > 0)
            && (Column < Width)
            && (Row < Height)) Value = *(GetData() + (Row*Width) + Column);

    return Value;
}

void OfflineFrame::CreateCheckerBoard(unsigned int SizeOfSquareSide,unsigned char GraylevelBackground, unsigned char GraylevelForeground)
{
    if (GetSize() == 0) return;
    if (SizeOfSquareSide == 0) Set(GraylevelBackground);
    else
    {
        unsigned int VerticalSquares = Height/SizeOfSquareSide;
        unsigned int HorizontalSquares = Width/SizeOfSquareSide;

        if ((VerticalSquares == 0) || (HorizontalSquares == 0)) Set(GraylevelBackground);
        else
        {
            unsigned char* Destination = GetData();

            unsigned int XOffset = (Width-(HorizontalSquares*SizeOfSquareSide))/2;
            unsigned int YOffset = (Height-(VerticalSquares*SizeOfSquareSide))/2;
            unsigned int VerticalSquareCounter = 0;
            unsigned int SquareHeight = 0;
            unsigned char RowSquareColour = GraylevelForeground;

            for (unsigned int y = 0; y < Height;y++)
            {
                unsigned int HorizontalSquareCounter = 0;
                unsigned int SquareWidth = 0;
                unsigned char ColumnSquareColour =RowSquareColour;

                if (( y >= YOffset)
                        && (VerticalSquareCounter < VerticalSquares))
                {
                    for (unsigned int x = 0;x < Width;x++)
                    {
                        unsigned char Value = GraylevelBackground;
                        if ((x >= XOffset)
                                && (HorizontalSquareCounter < HorizontalSquares))
                        {
                            Value = ColumnSquareColour;
                            if (++SquareWidth == SizeOfSquareSide)
                            {
                                HorizontalSquareCounter++;
                                SquareWidth = 0;
                                if (ColumnSquareColour == GraylevelForeground) ColumnSquareColour = GraylevelBackground;
                                else ColumnSquareColour = GraylevelForeground;
                            }
                        }

                        *Destination++ = Value;
                    } // end columns with squares

                    if (++SquareHeight == SizeOfSquareSide)
                    {
                        SquareHeight = 0;
                        VerticalSquareCounter++;
                        if (RowSquareColour == GraylevelForeground) RowSquareColour = GraylevelBackground;
                        else RowSquareColour = GraylevelForeground;
                    }
                }  else for (unsigned int x = 0;x < Width;x++) *Destination++ = GraylevelBackground;
            } // end rows
        } // end fill squares
    } // end defined square-size
}

void OfflineFrame::Set(unsigned int Width,unsigned int Height, unsigned int WidthStep,const unsigned char* Data)
{
    unsigned int Size = Height * Width;
    this->Data.resize(Size);
    this->Width = Width;
    this->Height = Height;
    if (Size > 0)
    {
        const unsigned char* RowSource = Data;
        unsigned char* Target = GetData();
        for (unsigned int y = 0; y < Height; y++)
        {
            const unsigned char* Source = RowSource;
            for (unsigned int x = 0; x < Width; x++) *Target++ = *Source++;
            RowSource += WidthStep;
        } // end rows
    }
}

void OfflineFrame::SetSubsampled(unsigned int Width, unsigned int Height,
                              unsigned int WidthStep,unsigned int SampleWidth,
                              unsigned int SampleHeight, unsigned int NumChs, const unsigned char* Data)
{
    if ((Data == 0)
            || (SampleWidth == 0)
            || (SampleHeight == 0)) return;     // Invalid input-parameters

    // Calculate dimensions of the subsampled-image

    this->Width = Width/SampleWidth;
    this->Height = Height/SampleHeight;

    // In case the sample-distance isn't a divider of the according
    // dimension, then increment the according subsampled dimension

    if (Width%SampleWidth != 0) this->Width += 1;
    if (Height%SampleHeight != 0) this->Height += 1;

    unsigned int Size = (this->Width*this->Height);
    this->Data.resize(Size);
    unsigned int RowStep = (WidthStep*SampleHeight);

    if (Size > 0)
    {
        const unsigned char* Source = Data;
        unsigned char* Target = GetData();
        for (unsigned int y = 0; y < this->Height; y++)
        {
            const unsigned char* SourceData = Source;
            for (unsigned int x = 0; x < this->Width; x++)
            {
                *Target++ = *SourceData;
                SourceData += SampleWidth * NumChs;
            } // end columns
            Source += RowStep;
        } // end columns
    } // end non-empty image
}

// Return true if the size and the data matches those of the frame referenced

bool OfflineFrame::CheckIfEqual(const OfflineFrame& FrameOI) const
{
    if (Width != FrameOI.GetWidth()) return false;
    if (Height != FrameOI.GetHeight()) return false;
    unsigned int Size = GetSize();
    if (Size > 0)
    {
        const unsigned char* Data1 = GetData();
        const unsigned char* Data2 = FrameOI.GetData();
        for (unsigned int i=0;i<Size;i++)
        {
            if (*Data1++ != *Data2++) return false;
        }
    }

    return true;
}


} // namespace vipnt
