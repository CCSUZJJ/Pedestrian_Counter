cmake_minimum_required(VERSION 2.8)

include("../../../cmake/OptionsCommon.cmake")
include("../../../cmake/Options${PORT}.cmake")

traf_set_public_headers("TinyXml")

set(SRC_COMMON 
			"${TRAF_INCLUDE_PUBLIC}/tinystr.h"
			"${TRAF_INCLUDE_PUBLIC}/tinyxml.h"
			"src/tinystr.cpp"
			"src/tinyxml.cpp"
			"src/tinyxmlerror.cpp"
			"src/tinyxmlparser.cpp"
		   )

set(SRC_TARGET 3rdpartyTinyXml)

traf_start_library()
traf_end_library()

traf_include_public_headers("TinyXml")
