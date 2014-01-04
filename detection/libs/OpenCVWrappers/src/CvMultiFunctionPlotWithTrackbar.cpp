#include "CvMultiFunctionPlotWithTrackbar.h"
#include <opencv/highgui.h> // includes highGUI definitions
#include <sstream>

// Trackbar-callback

void TrackBarCvMultiFunctionPlot( int TrackBarPosition, void* ControlObject)
{
    if (ControlObject == 0) return;
    CvMultiFunctionPlotWithTrackbar* Control = (CvMultiFunctionPlotWithTrackbar*)ControlObject;
    Control->TrackBarCallBack(TrackBarPosition);
}

// MouseCallBack

void MouseHandlerCvMultiFunctionPlot(int event, int x, int y, int flags, void *param)
{
    OpenCvMouseEventControl* MouseControl = (OpenCvMouseEventControl*)param;
    if (MouseControl == 0) return;		// No mouse-control ???

    // References expected
    // ref[0] = parent

    if (MouseControl->DataReferences.size() < 1) return;	// no proper data available
    CvMultiFunctionPlotWithTrackbar* ObjectOI = (CvMultiFunctionPlotWithTrackbar*) MouseControl->DataReferences[0];

    // Process mouse-events

    MouseControl->ProcessEvent(event,x,y,flags);
//    if (MouseControl->IsLeftMouseDoubleClicked())
    {
//        ObjectOI->LogSnapShot();
    }
}

void CvMultiFunctionPlotWithTrackbar::FunctionPlotProperties::Reset()
{
    PlotData.clear();
    ROIPlot.x = 0;
    ROIPlot.y = 0;
    ROIPlot.width = 0;
    ROIPlot.height = 0;
}

CvMultiFunctionPlotWithTrackbar::CvMultiFunctionPlotWithTrackbar()
{
}

CvMultiFunctionPlotWithTrackbar::~CvMultiFunctionPlotWithTrackbar()
{
    Reset();
}

void CvMultiFunctionPlotWithTrackbar::Reset()
{
    DataToPlot.clear();
    PlotCanvas.Reset();

    if (NameWindow.size() > 0)
    {
        cvDestroyWindow(NameWindow.c_str());
    }

    NameWindow.clear();
    NameTrackBar.clear();

    SizeCanvas.width = 0;
    SizeCanvas.height = 0;

    // Trackbar-control

    MinimumTrackBarPosition = 0;
    MaximumTrackBarPosition = 0;

    // Mousecontrol

    MyMouse.ReleaseAllMouses();
}

const IplImage* CvMultiFunctionPlotWithTrackbar::GetCanvas() const
{
    return PlotCanvas.GetCanvas();
}

void CvMultiFunctionPlotWithTrackbar::SetMargins(const CvSize& CanvasPlotMargin,const CvSize& ROIDistance)
{
    this->CanvasPlotMargin = CanvasPlotMargin;
    this->ROIDistance = ROIDistance;
}

void CvMultiFunctionPlotWithTrackbar::TrackBarCallBack(int TrackBarPosition)
{
    // Limit trackbar-position

    int MyTrackBarPosition = TrackBarPosition;


    if (MyTrackBarPosition < MinimumTrackBarPosition)
    {
        MyTrackBarPosition = MinimumTrackBarPosition;
        cv::setTrackbarPos(NameTrackBar,NameWindow,MyTrackBarPosition);
    }
    else if (MyTrackBarPosition > MaximumTrackBarPosition)
    {
        MyTrackBarPosition = MaximumTrackBarPosition;
        cv::setTrackbarPos(NameTrackBar,NameWindow,MyTrackBarPosition);
    }

    // Erase canvas

    PlotCanvas.EraseCanvas(CanvasColour);

    // Replot-canvas

    Plot(true,MyTrackBarPosition);

    // Finaly, show the canvas !

    PlotCanvas.ShowCanvas(NameWindow.c_str());
}

void CvMultiFunctionPlotWithTrackbar::SetDataToPlot(unsigned int Reference,const std::vector<unsigned int>& Data,const CvScalar& PlotColour,unsigned int PlotHeight)
{
    if (PlotHeight == 0) return;
    if (Reference == 0) return;
    unsigned int FunctionsToPlot = (unsigned int)DataToPlot.size();

    if (Reference == (FunctionsToPlot+1))
    {
        // Create new entry

        CvMultiFunctionPlotWithTrackbar::FunctionPlotProperties FunctionOI;
        DataToPlot.push_back(FunctionOI);

        CvMultiFunctionPlotWithTrackbar::FunctionPlotProperties& MyFunction = DataToPlot[Reference-1];
        MyFunction.PlotColour = PlotColour;
        MyFunction.PlotData = Data;
        MyFunction.ROIPlot.width = (int) Data.size();
        MyFunction.ROIPlot.height  = PlotHeight;
    }
    else
    {
        // update existing entry

        CvMultiFunctionPlotWithTrackbar::FunctionPlotProperties& MyFunction = DataToPlot[Reference-1];
        MyFunction.Reset();
        MyFunction.PlotColour = PlotColour;
        MyFunction.PlotData = Data;
        MyFunction.ROIPlot.width = (int) Data.size();
        MyFunction.ROIPlot.height  = PlotHeight;
    }
}

void CvMultiFunctionPlotWithTrackbar::ConfigurePlot(const CvScalar& BackGroundColour,const std::string& NameWindow)
{
    CanvasColour = BackGroundColour;
    this->NameWindow = NameWindow;

    // Determine Maximumsize of the plot-windows

    unsigned int FunctionsToPlot = (unsigned int)DataToPlot.size();
    if (FunctionsToPlot == 0) return;       // No functions to plot

    // Determine size of canvas

    SizeCanvas.width = 0;
    SizeCanvas.height = 0;

    CvRect ROIPlot;
    ROIPlot.x = CanvasPlotMargin.width;
    ROIPlot.y = CanvasPlotMargin.height;

    for (unsigned int i = 0;i<FunctionsToPlot;++i)
    {
        DataToPlot[i].ROIPlot.x = ROIPlot.x;
        DataToPlot[i].ROIPlot.y = ROIPlot.y;

        // Advance

        ROIPlot.y += DataToPlot[i].ROIPlot.height;
        if (i < (FunctionsToPlot-1))
        {
            ROIPlot.y += ROIDistance.height;
        }

        // Determine maximum-window-size

        int PlotWidth = DataToPlot[i].ROIPlot.width;
        PlotWidth += (2* CanvasPlotMargin.width);
        if (PlotWidth > SizeCanvas.width)
        {
            SizeCanvas.width = PlotWidth;
        }
    }

    ROIPlot.y += CanvasPlotMargin.height;
    SizeCanvas.height = ROIPlot.y;

    // Create canvas

    PlotCanvas.SetCanvas(SizeCanvas,CanvasColour);

    // Setup marker-constraints

    MinimumTrackBarPosition = CanvasPlotMargin.width;
    MaximumTrackBarPosition = SizeCanvas.width - CanvasPlotMargin.width;
}

void CvMultiFunctionPlotWithTrackbar::Plot(bool AddMarker,int TrackBarPosition)
{
    if (!PlotCanvas.HasBeenSetUp()) return;
    if (NameWindow.size() == 0) return;

    unsigned int FunctionsToPlot = (unsigned int)DataToPlot.size();
    if (FunctionsToPlot == 0) return;       // No functions to plot

    Draw();

    if (AddMarker) PlotMarker(TrackBarPosition);

    PlotCanvas.ShowCanvas(NameWindow.c_str());
}

// Draw the functions to pot on the canvas

void CvMultiFunctionPlotWithTrackbar::Draw()
{
    if (!PlotCanvas.HasBeenSetUp()) return;

    unsigned int FunctionsToPlot = (unsigned int)DataToPlot.size();
    if (FunctionsToPlot == 0) return;       // No functions to plot

    for (unsigned int i = 0;i<FunctionsToPlot;++i)
    {
        const CvMultiFunctionPlotWithTrackbar::FunctionPlotProperties& PlotData = DataToPlot.at(i);
        PlotCanvas.PlotFunction(PlotData.PlotData,PlotData.PlotColour,PlotData.ROIPlot,1.0);
        DrawAxis(PlotData);
    }
}


void CvMultiFunctionPlotWithTrackbar::DrawAxis(const CvMultiFunctionPlotWithTrackbar::FunctionPlotProperties& PlotData)
{
    CvPoint P1;
    P1.x = PlotData.ROIPlot.x;
    P1.y = PlotData.ROIPlot.y + PlotData.ROIPlot.height;
    CvPoint P2;
    P2.x = PlotData.ROIPlot.x + PlotData.ROIPlot.width;
    P2.y = P1.y;

    PlotCanvas.PlotLine(P1,P2,AxisColour);
}

void CvMultiFunctionPlotWithTrackbar::AddTrackbar(const std::string& NameTrackBar)
{
    if (NameTrackBar.size() == 0) return;
    if (NameWindow.size() == 0) return;
    if (!PlotCanvas.HasBeenSetUp()) return;
    if (MaximumTrackBarPosition == 0) return;

    this->NameTrackBar = NameTrackBar;
    cv::createTrackbar(NameTrackBar,NameWindow, 0,MaximumTrackBarPosition, TrackBarCvMultiFunctionPlot ,(void*) this);
}

void CvMultiFunctionPlotWithTrackbar::PlotMarker(unsigned int Position)
{
    if (((int)Position >= MinimumTrackBarPosition)
            && ((int)Position <= MaximumTrackBarPosition))
    {
        CvPoint P1;
        P1.x = Position;
        P1.y = CanvasPlotMargin.height;
        CvPoint P2;
        P2.x = Position;
        P2.y = SizeCanvas.height - CanvasPlotMargin.height ;

        PlotCanvas.PlotLine(P1,P2,MarkerColour);

        // Display the value in the left-top of the plot-window

        unsigned int FunctionsToPlot = (unsigned int)DataToPlot.size();
        if (FunctionsToPlot != 0)
        {
            int x = (int)Position - MinimumTrackBarPosition;

            for (unsigned int i = 0;i<FunctionsToPlot;++i)
            {
                const CvMultiFunctionPlotWithTrackbar::FunctionPlotProperties& PlotData = DataToPlot.at(i);
                int FunctionWidth = PlotData.PlotData.size();
                if ((x >= 0) && (x < FunctionWidth))
                {
                    unsigned int FunctionValue = PlotData.PlotData[x];

                    std::stringstream DisplayText;
                    DisplayText << FunctionValue;

                    CvPoint TextPosition;
                    TextPosition.x = PlotData.ROIPlot.x + 2;
                    TextPosition.y = PlotData.ROIPlot.y + 10;
                    PlotCanvas.WriteTextOnCanvas(DisplayText.str(),PlotData.PlotColour,TextPosition);
                }
            }
        }
    }
}

void CvMultiFunctionPlotWithTrackbar::AddMouseControl(const std::string& NameOfSnapshot)
{
    MyMouse.ReleaseAllMouses();
    if (NameOfSnapshot.size() == 0) return;

    // Enable mouse-control

    this->NameOfSnapshot = NameOfSnapshot;
    OpenCvMouseEventControl* AddedMouse = MyMouse.AddMouse(NameWindow.c_str(),&MouseHandlerCvMultiFunctionPlot);
    AddedMouse->DataReferences.push_back(this);
}


void CvMultiFunctionPlotWithTrackbar::LogSnapShot()
{
    if (NameOfSnapshot.size() == 0) return;
    const IplImage* SnapshotImage = PlotCanvas.GetCanvas();
    if (SnapshotImage != 0)
    {
        cvSaveImage(NameOfSnapshot.c_str(),SnapshotImage);
    }
}
