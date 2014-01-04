cmake_minimum_required(VERSION 2.8)

include("../../../cmake/OptionsCommon.cmake")
include("../../../cmake/Options${PORT}.cmake")

traf_set_public_headers("ResearchGeneral")

set(SRC_COMMON 
	"${TRAF_INCLUDE_PUBLIC}/DetAlgoI.h"
        "${TRAF_INCLUDE_PUBLIC}/ResearchGeneral.h"
        "src/ResearchGeneral.cpp"
            )

set(SRC_TARGET detectionResearchGeneral)

traf_start_library()
traf_end_library()

traf_lib_use("3rdparty" "TinyXml++")

traf_include_public_headers("ResearchGeneral")

traf_lib_include("3rdparty" "TinyXml++")

traf_lib_include("detection" "OpenCVWrappers")


