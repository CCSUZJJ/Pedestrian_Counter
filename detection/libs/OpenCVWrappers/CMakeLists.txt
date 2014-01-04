cmake_minimum_required(VERSION 2.8)

include("../../../cmake/OptionsCommon.cmake")
include("../../../cmake/Options${PORT}.cmake")

set(SRC_TARGET detectionOpenCVWrappers)

traf_set_public_headers("OpenCVWrappers")

set(SRC_COMMON 
            "${TRAF_INCLUDE_PUBLIC}/OpenCvFrame.h"
            "${TRAF_INCLUDE_PUBLIC}/OpenCvVideo.h"
            "${TRAF_INCLUDE_PUBLIC}/OpenCvVideoRecorder.h"
            "${TRAF_INCLUDE_PUBLIC}/OpenCvMouse.h"
            "${TRAF_INCLUDE_PUBLIC}/CvBoundingBox.h"
            "${TRAF_INCLUDE_PUBLIC}/OfflineFrame.h"
            "${TRAF_INCLUDE_PUBLIC}/OpenCvUtilities.h"
            "${TRAF_INCLUDE_PUBLIC}/OpenCvYFrameUtilities.h"
            "${TRAF_INCLUDE_PUBLIC}/OpenCvColours.h"
            "${TRAF_INCLUDE_PUBLIC}/CvCanvas.h"
            "${TRAF_INCLUDE_PUBLIC}/CvFunctionPlot.h"
            "${TRAF_INCLUDE_PUBLIC}/CvHistogramPlot.h"
            "${TRAF_INCLUDE_PUBLIC}/CvLine.h"
            "${TRAF_INCLUDE_PUBLIC}/CvMultiFunctionPlotWithTrackbar.h"
            "${TRAF_INCLUDE_PUBLIC}/CvPoint.h"
            "${TRAF_INCLUDE_PUBLIC}/CvSize.h"
            "${TRAF_INCLUDE_PUBLIC}/CvRect.h"
            "${TRAF_INCLUDE_PUBLIC}/CvPointUtilities.h"
            "${TRAF_INCLUDE_PUBLIC}/CvPlotUtilities.h"
            "${TRAF_INCLUDE_PUBLIC}/LinePieceFunctions.h"
            "${TRAF_INCLUDE_PUBLIC}/OuterContourFinder.h"
            "${TRAF_INCLUDE_PUBLIC}/OpenCVColours.h"
            "${TRAF_INCLUDE_PUBLIC}/cvplot.h"
            "${TRAF_INCLUDE_PUBLIC}/Point.h"
            "src/Point.cpp"
            "src/OfflineFrame.cpp"
            "src/CvBoundingBox.cpp"
            "src/CvCanvas.cpp"
            "src/CvCanvasWithGrayImages.cpp"
            "src/CvFunctionPlot.cpp"
            "src/CvHistogramPlot.cpp"
            "src/CvLine.cpp"
            "src/CvMultiFunctionPlotWithTrackbar.cpp"
            "src/CvPlotUtilities.cpp"
            "src/CvPoint.cpp"
            "src/CvSize.cpp"
            "src/CvRect.cpp"
            "src/CvPointUtilities.cpp"
            "src/CvRect.cpp"
            "src/CvSize.cpp"
            "src/LinePieceFunctions.cpp"
            "src/OpenCvFrame.cpp"
            "src/OpenCvMouse.cpp"
            "src/OpenCvUtilities.cpp"
            "src/OpenCvVideo.cpp"
            "src/OpenCvVideoRecorder.cpp"
            "src/OuterContourFinder.cpp"
            "src/cvplot.cpp"
          )

traf_start_library()

traf_end_library()

traf_include_public_headers("OpenCVWrappers")

