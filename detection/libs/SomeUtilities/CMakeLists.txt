cmake_minimum_required(VERSION 2.8)

include("../../../cmake/OptionsCommon.cmake")
include("../../../cmake/Options${PORT}.cmake")

set(SRC_TARGET detectionSomeUtilities)

traf_set_public_headers("SomeUtilities")

set(SRC_COMMON 
            "${TRAF_INCLUDE_PUBLIC}/CircularBuffer.h"
            "${TRAF_INCLUDE_PUBLIC}/ConnectedPixelClusters.h"
            "${TRAF_INCLUDE_PUBLIC}/ClosedInterval.h"
            "${TRAF_INCLUDE_PUBLIC}/CvDetectionFramework.h"
            "${TRAF_INCLUDE_PUBLIC}/DetectionTestEntry.h"
            "${TRAF_INCLUDE_PUBLIC}/DetectionTestSpecifications.h"
            "${TRAF_INCLUDE_PUBLIC}/ .h"
            "${TRAF_INCLUDE_PUBLIC}/FrameDecoding.h"
            "${TRAF_INCLUDE_PUBLIC}/RegionOfRows.h"  
            "${TRAF_INCLUDE_PUBLIC}/Image.h"  
            "src/ConnectedPixelClusters.cpp"
            "src/CvDetectionFramework.cpp"
            "src/DetectionTestEntry.cpp"
            "src/DetectionTestSpecifications.cpp"
            "src/FrameDecoding.cpp"
            "src/RegionOfRows.cpp"
             )

if(DEPENDS_ON_DESKTOP)
   find_package(TraficonOpenCvBinaryLibs REQUIRED)
   find_package(FFMpegBinaryLibs REQUIRED)
endif()

traf_start_library()

if(DEPENDS_ON_DESKTOP)
    traf_lib_use("xstream" "FFMpeg" "submodules/Elio")
    include_directories(${TRAFICONOPENCVBINARYLIBS_INCLUDE_DIRS})
endif()

traf_end_library()

traf_include_public_headers("SomeUtilities")


traf_lib_include("3rdparty" "TinyXml" "submodules/Elio")
traf_lib_include("detection" "OpenCVWrappers")
