cmake_minimum_required(VERSION 2.8)


include("../../../cmake/OptionsCommon.cmake")

include("../../../cmake/Options${PORT}.cmake")


traf_set_public_headers("ResearchDetectionFramework")


set(SRC_COMMON 
	"${TRAF_INCLUDE_PUBLIC}/ResearchDetectionFramework.h"
    "src/ResearchDetectionFramework.cpp"
            )


set(SRC_TARGET detectionResearchDetectionFramework)


traf_start_library()

traf_end_library()


traf_lib_use("detection" "ResearchGeneral")

traf_lib_use("detection" "OpenCVWrappers")

traf_lib_use("detection" "PeopleCounter")

traf_lib_use("3rdparty" "TinyXml" )

traf_lib_use("3rdparty" "TinyXml++")

traf_include_public_headers("ResearchDetectionFramework")

traf_lib_include("detection" "ResearchGeneral")

traf_lib_include("detection" "OpenCVWrappers")

traf_lib_include("detection" "PeopleCounter")

traf_lib_include("3rdparty" "TinyXml" )

traf_lib_include("3rdparty" "TinyXml++")
