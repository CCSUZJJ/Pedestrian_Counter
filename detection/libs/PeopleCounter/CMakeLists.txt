cmake_minimum_required(VERSION 2.8)

include("../../../cmake/OptionsCommon.cmake")
include("../../../cmake/Options${PORT}.cmake")

traf_set_public_headers("PeopleCounter")

set(SRC_COMMON 
	"${TRAF_INCLUDE_PUBLIC}/PeopleCounter.h"
    "src/PeopleCounter.cpp"
        "${TRAF_INCLUDE_PUBLIC}/ForegroundSubtraction.h"
    "src/ForegroundSubtraction.cpp"
        "${TRAF_INCLUDE_PUBLIC}/BlobSegmentation.h"
    "src/BlobSegmentation.cpp"
        "${TRAF_INCLUDE_PUBLIC}/Tracking.h"
    "src/Tracking.cpp"
        "${TRAF_INCLUDE_PUBLIC}/DetectedObject.h"
        "${TRAF_INCLUDE_PUBLIC}/DetectedBlob.h"
    "src/DetectedBlob.cpp"
        "${TRAF_INCLUDE_PUBLIC}/Track.h"
    "src/Track.cpp"
        "${TRAF_INCLUDE_PUBLIC}/AlgorithmEvent.h"
    "src/AlgorithmEvent.cpp"
        "${TRAF_INCLUDE_PUBLIC}/Geometry.h"
    "src/Geometry.cpp"
        "${TRAF_INCLUDE_PUBLIC}/gtTrack.h"
    "src/gtTrack.cpp"
        "${TRAF_INCLUDE_PUBLIC}/gtEvent.h"
    "src/gtEvent.cpp"
            )

set(SRC_TARGET detectionPeopleCounter)

traf_start_library()
traf_end_library()

traf_lib_use("detection" "ResearchGeneral")
traf_lib_use("detection" "OpenCVWrappers")
traf_lib_use("3rdparty" "TinyXml" )
traf_lib_use("3rdparty" "TinyXml++")

traf_include_public_headers("PeopleCounter")
traf_lib_include("detection" "ResearchGeneral")
traf_lib_include("detection" "OpenCVWrappers")
traf_lib_include("3rdparty" "TinyXml" )
traf_lib_include("3rdparty" "TinyXml++")



