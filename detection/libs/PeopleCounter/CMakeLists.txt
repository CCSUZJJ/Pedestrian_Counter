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



