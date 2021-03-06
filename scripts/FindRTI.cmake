# - Try to find M&S HLA RTI libraries
# This module finds if any HLA RTI is installed and locates the standard RTI
# include files and libraries.
#
# RTI is a simulation infrastructure standardized by IEEE and SISO. It has a
# well defined C++ API that assures that simulation applications are
# independent on a particular RTI implementation.
#  http://en.wikipedia.org/wiki/Run-Time_Infrastructure_(simulation)
#
# This code sets the following variables:
#  RTI_PATH = the RTI install prefix
#  RTI_INCLUDE_DIR = the directory where RTI includes file are found
#  RTI_LIBRARIES = The libraries to link against to use RTI
#  RTI_DEFINITIONS = -DRTI_USES_STD_FSTREAM
#  RTI_FOUND = Set to FALSE if any HLA RTI was not found
#  RTI_VERSION = version of the detected HLA infrastructure
#
# Report problems to <certi-devel@nongnu.org>

MACRO(RTI_MESSAGE_QUIETLY QUIET TYPE MSG)
  IF(NOT ${QUIET})
    MESSAGE(${TYPE} "${MSG}")
  ENDIF(NOT ${QUIET})
ENDMACRO(RTI_MESSAGE_QUIETLY QUIET TYPE MSG)

IF(NOT RTI_PATH)
  # Detect the CERTI installation, http://www.cert.fr/CERTI
  IF ("$ENV{CERTI_HOME}" STRGREATER "")
    FILE(TO_CMAKE_PATH "$ENV{CERTI_HOME}" RTI_PATH)
    RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "Using environment defined CERTI_HOME: ${RTI_PATH}")
  ENDIF ("$ENV{CERTI_HOME}" STRGREATER "")

  SET(RTI_PATH ${RTI_PATH} CACHE PATH "The HLA RTI directory root.")
ENDIF(NOT RTI_PATH)

SET(RTI_DEFINITIONS "-DRTI_USES_STD_FSTREAM")

# Detect the MAK Technologies RTI installation, http://www.mak.com/products/rti.php
# note: the following list is ordered to find the most recent version first
SET(RTI_POSSIBLE_DIRS
  ${RTI_PATH}
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MAK Technologies\\MAK RTI 3.2 MSVC++ 8.0;Location]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\MAK RTI 3.2-win32-msvc++8.0;InstallLocation]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MAK Technologies\\MAK RTI 2.2;Location]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\MAK RTI 2.2;InstallLocation]")

SET(RTI_OLD_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES}")
# The MAK RTI has the "lib" prefix even on Windows.
SET(CMAKE_FIND_LIBRARY_PREFIXES "lib" "")

# Handle HLA 1.3 / RTI-NG
FIND_LIBRARY(RTI_LIBRARY
  NAMES RTI-NG RTI
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES lib Release
  DOC "The HLA 1.3 RTI Library")

FIND_LIBRARY(RTI_LIBRARY_DEBUG
  NAMES RTI-NGd RTId
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES lib Debug
  DOC "The HLA 1.3 RTI Debug Library")

# Handle IEEE 1516
FIND_LIBRARY(RTI1516_LIBRARY
  NAMES RTI1516
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES lib Release
  DOC "The IEEE 1516 RTI Library")

FIND_LIBRARY(RTI1516_LIBRARY_DEBUG
  NAMES RTI1516d
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES lib Debug
  DOC "The IEEE 1516 RTI Debug Library")

MACRO(RTI_SET_LIBRARIES VAR_LIBRARIES VAR_RELEASELIB VAR_DEBUGLIB MESSAGE)
  IF(${VAR_RELEASELIB} AND ${VAR_DEBUGLIB})
    SET(${VAR_LIBRARIES} optimized ${${VAR_RELEASELIB}} debug ${${VAR_DEBUGLIB}})
    RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "${MESSAGE} found: ${${VAR_RELEASELIB}} ${${VAR_DEBUGLIB}}")
  ELSE(${VAR_RELEASELIB} AND ${VAR_DEBUGLIB})
    IF(${VAR_RELEASELIB})
      SET(${VAR_LIBRARIES} ${${VAR_RELEASELIB}})
      RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "${MESSAGE} found: ${${VAR_RELEASELIB}}")
    ELSEIF(${VAR_DEBUGLIB})
      SET(${VAR_LIBRARIES} ${${VAR_DEBUGLIB}})
      RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "${MESSAGE} found: ${${VAR_DEBUGLIB}}")
    ELSE(${VAR_RELEASELIB})
      RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "${MESSAGE} NOT found")
    ENDIF(${VAR_RELEASELIB})
  ENDIF(${VAR_RELEASELIB} AND ${VAR_DEBUGLIB})
ENDMACRO(RTI_SET_LIBRARIES)

RTI_SET_LIBRARIES(
  RTI_LIBRARIES RTI_LIBRARY RTI_LIBRARY_DEBUG "HLA 1.3 RTI library")

RTI_SET_LIBRARIES(
  RTI1516_LIBRARIES RTI1516_LIBRARY RTI1516_LIBRARY_DEBUG "IEEE 1516 RTI library")

FIND_LIBRARY(RTI_FEDTIME_LIBRARY
  NAMES FedTime
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES lib Release
  DOC "The HLA 1.3 FedTime Library")

FIND_LIBRARY(RTI_FEDTIME_LIBRARY_DEBUG
  NAMES FedTimed
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES lib Debug
  DOC "The HLA 1.3 FedTime Debug Library")

RTI_SET_LIBRARIES(
  RTI_FEDTIME_LIBRARIES RTI_FEDTIME_LIBRARY RTI_FEDTIME_LIBRARY_DEBUG "HLA 1.3 RTI FedTime")

FIND_LIBRARY(RTI1516_FEDTIME_LIBRARY
  NAMES FedTime1516
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES lib Release
  DOC "The IEEE 1516 FedTime Library")

FIND_LIBRARY(RTI1516_FEDTIME_LIBRARY_DEBUG
  NAMES FedTime1516d
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES lib Debug
  DOC "The IEEE 1516 FedTime Debug Library")

RTI_SET_LIBRARIES(
  RTI1516_FEDTIME_LIBRARIES RTI1516_FEDTIME_LIBRARY RTI1516_FEDTIME_LIBRARY_DEBUG "IEEE 1516 RTI FedTime")

FIND_PATH(RTI_INCLUDE_DIR
  NAMES RTI.hh
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES "include" "include/hla13"
  DOC "The HLA 1.3 RTI Include Files")

IF (RTI_INCLUDE_DIR)
  RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "HLA 1.3 RTI headers found: ${RTI_INCLUDE_DIR}")
ELSE (RTI_INCLUDE_DIR)
  RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "HLA 1.3 RTI headers NOT found")
ENDIF (RTI_INCLUDE_DIR)

FIND_PATH(RTI1516_INCLUDE_DIR
  NAMES RTI1516.h
  PATHS ${RTI_POSSIBLE_DIRS}
  PATH_SUFFIXES "include/RTI" "include/ieee1516-2000/RTI"
  DOC "The IEEE 1516 RTI Include Files")

IF (RTI1516_INCLUDE_DIR)
  string(REGEX REPLACE "RTI$" "" RTI1516_INCLUDE_DIR ${RTI1516_INCLUDE_DIR})
  RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "IEEE 1516 RTI headers found: ${RTI_INCLUDE_DIR}")
ELSE (RTI1516_INCLUDE_DIR)
  RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "IEEE 1516 RTI headers NOT found")
ENDIF (RTI1516_INCLUDE_DIR)

FIND_PROGRAM(CERTI_RTIG_EXECUTABLE
  NAMES rtig
  PATHS ${RTI_PATH}
  PATH_SUFFIXES bin
  DOC "The RTI Gateway")
IF (CERTI_RTIG_EXECUTABLE)
  EXECUTE_PROCESS(COMMAND ${CERTI_RTIG_EXECUTABLE} -V
    RESULT_VARIABLE RTI_RESV
    OUTPUT_VARIABLE RTI_OUTV
    ERROR_VARIABLE  RTI_ERRV
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  # Check if rtig was executed properly
  IF (RTI_RESV EQUAL 0)
    STRING(REGEX REPLACE ".*RTIG " "" RTI_VERSION ${RTI_OUTV})
    RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "Found CERTI version: ${RTI_VERSION}")
  ELSE (RTI_RESV EQUAL 0)
    RTI_MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "CERTI RTIG cannot be run properly, check your CERTI installation")
  ENDIF(RTI_RESV EQUAL 0)
ENDIF (CERTI_RTIG_EXECUTABLE)

# Set the modified system variables back to the original value.
SET(CMAKE_FIND_LIBRARY_PREFIXES "${RTI_OLD_FIND_LIBRARY_PREFIXES}")

IF (RTI_LIBRARIES AND RTI_INCLUDE_DIR)
  SET(RTI13_FOUND TRUE)
ELSE (RTI_LIBRARIES AND RTI_INCLUDE_DIR)
  SET(RTI13_FOUND FALSE)
ENDIF(RTI_LIBRARIES AND RTI_INCLUDE_DIR)

IF (RTI1516_LIBRARIES AND RTI1516_INCLUDE_DIR)
  SET(RTI1516_FOUND TRUE)
ELSE (RTI1516_LIBRARIES AND RTI1516_INCLUDE_DIR)
  SET(RTI1516_FOUND FALSE)
ENDIF(RTI1516_LIBRARIES AND RTI1516_INCLUDE_DIR)

IF (RTI13_FOUND OR RTI1516_FOUND)
  SET(RTI_FOUND TRUE)
ELSE (RTI13_FOUND OR RTI1516_FOUND)
  SET(RTI_FOUND FALSE)
  IF (RTI_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "No RTI found! Please install a HLA RTI and re-run.")
  ENDIF (RTI_FIND_REQUIRED)
ENDIF (RTI13_FOUND OR RTI1516_FOUND)  

SET(RTI_LIBRARIES
  ${RTI_LIBRARIES} ${RTI_FEDTIME_LIBRARIES})

SET(RTI1516_LIBRARIES
  ${RTI1516_LIBRARIES} ${RTI1516_FEDTIME_LIBRARIES})

# $Id: FindRTI.cmake,v 1.15 2013/12/09 15:49:39 erk Exp $
