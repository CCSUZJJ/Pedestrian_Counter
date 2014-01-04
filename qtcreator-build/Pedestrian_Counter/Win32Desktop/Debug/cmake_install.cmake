# Install script for directory: F:/user/Documents/FLIR/Pedestrian_Counter/products/libs/Pedestrian_Counter

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Project")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("F:/user/Documents/FLIR/Pedestrian_Counter/qtcreator-build/Pedestrian_Counter/Win32Desktop/Debug/3rdparty/libs/TinyXml/cmake_install.cmake")
  INCLUDE("F:/user/Documents/FLIR/Pedestrian_Counter/qtcreator-build/Pedestrian_Counter/Win32Desktop/Debug/3rdparty/libs/TinyXml++/cmake_install.cmake")
  INCLUDE("F:/user/Documents/FLIR/Pedestrian_Counter/qtcreator-build/Pedestrian_Counter/Win32Desktop/Debug/detection/libs/ResearchGeneral/cmake_install.cmake")
  INCLUDE("F:/user/Documents/FLIR/Pedestrian_Counter/qtcreator-build/Pedestrian_Counter/Win32Desktop/Debug/detection/libs/ResearchDetectionFramework/cmake_install.cmake")
  INCLUDE("F:/user/Documents/FLIR/Pedestrian_Counter/qtcreator-build/Pedestrian_Counter/Win32Desktop/Debug/detection/libs/PeopleCounter/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "F:/user/Documents/FLIR/Pedestrian_Counter/qtcreator-build/Pedestrian_Counter/Win32Desktop/Debug/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "F:/user/Documents/FLIR/Pedestrian_Counter/qtcreator-build/Pedestrian_Counter/Win32Desktop/Debug/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
