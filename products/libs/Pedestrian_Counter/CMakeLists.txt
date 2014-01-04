cmake_minimum_required(VERSION 2.8)
project( Pedestrian_Counter )

#QTCREATOR_CMAKE_TARGETS_FILE ../../../cmake/QtcreatorCMakeTargets.txt

include("../../../cmake/OptionsCommon.cmake")
include("../../../cmake/Options${PORT}.cmake")


set(SRC_TARGET Pedestrian_Counter)

set(SRC_COMMON
        "src/main.cpp"
        "src/PreDefinedParameters.h"
   )

find_package(OpenCV REQUIRED)

#set(DEBUG_PORTS Win32Desktop)
set(SRC_WIN32_DESKTOP ${DEBUG_PORTS})
set(SRC_LINUX_DESKTOP ${DEBUG_PORTS})

traf_start_application()

target_link_libraries(Pedestrian_Counter ${OpenCV_LIBS})

traf_lib_use("3rdparty" "TinyXml" )
traf_lib_use("3rdparty" "TinyXml++" )

traf_lib_use("detection" "ResearchGeneral")
traf_lib_use("detection" "ResearchDetectionFramework")
traf_lib_use("detection" "IntrusionWrapperAlgo")
traf_lib_use("detection" "OpenCVWrappers")
traf_lib_use("detection" "PeopleCounter")


target_link_libraries(Pedestrian_Counter ${OpenCV_LIBS})

#traf_application_is_product()

traf_end_application()

include_directories("inc")

traf_lib_include("detection" "ResearchDetectionFramework")
traf_lib_include("detection" "ResearchGeneral")
traf_lib_include("3rdparty" "TinyXml" )
traf_lib_include("3rdparty" "TinyXml++" )
traf_lib_include("detection" "PeopleCounter")
traf_lib_include("detection" "OpenCVWrappers")
