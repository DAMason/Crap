#originaly written by Florian Uhlig
#example of CMake script for compiling and linking root classse.  
#To create an Xcode project:
#   cmake -G Xcode ./ 
#extracted from FairRoot project (fairroot.gsi.de) by M. Al-Turany (m.al-turany@gsi.de)



# Check if cmake has the required version
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

# Set name of our project to "CrapPhoAnalysis". Has to be done
# after check of cmake version since this is a new feature                  
project(Crap)

# Load some basic macros which are needed later on
include(FindROOT.cmake)

# Point to SusyEvent src (incorporated into local libs)
set(SUSYNTUPLE_SRC /Users/dmason/play/photons/SUSYPhotonAnalysis/SusyNtuplizer/src)

#Check the compiler and set the compile and link flags
set(CMAKE_BUILD_TYPE Debug)

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")


# Create a library called "libCrapPhoAnalysis" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
# ${SUSYNTUPLE_SRC}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

if (CMAKE_SYSTEM_NAME MATCHES Darwin)
   EXEC_PROGRAM("sw_vers -productVersion | cut -d . -f 1-2" OUTPUT_VARIABLE MAC_OS_VERSION)
   MESSAGE("--- Found a Mac OS X System ${MAC_OS_VERSION}")
   if (CMAKE_COMPILER_IS_GNUCXX)
      MESSAGE("--- Found GNU compiler collection")

      STRING(COMPARE EQUAL "10.5" "${MAC_OS_VERSION}" MAC_OS_10_5)
      IF(MAC_OS_10_5)
        SET(CMAKE_CXX_FLAGS "-m64")
        SET(CMAKE_Fortran_FLAGS "-m64")
      ENDIF(MAC_OS_10_5)

      SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")
      SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")

      # Select flags.
      set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
      set(CMAKE_CXX_FLAGS_RELEASE        "-O2")
      set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
      set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -fno-check-new -fno-common")
      set(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
      set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
      set(CMAKE_C_FLAGS_RELEASE          "-O2")
      set(CMAKE_C_FLAGS_DEBUG            "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
      set(CMAKE_C_FLAGS_DEBUGFULL        "-g3 -fno-inline -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
      set(CMAKE_C_FLAGS_PROFILE          "-g3 -fno-inline -ftest-coverage -fprofile-arcs")

      
      
      endif (CMAKE_COMPILER_IS_GNUCXX)

endif (CMAKE_SYSTEM_NAME MATCHES Darwin) 

##########################################################
# CrapPhoAnalysis 
##########################################################

# List of source files
set(CrapPhoAnalysis_SRCS
${SUSYNTUPLE_SRC}/SusyEvent.cc
PhoAnalysis/CrapPhoAnalysis.cc
)


# fill list of header files 
set(CrapPhoAnalysis_HEADERS
SusyEvent.h 
)

# set everything needed for the root dictonary and create the
# dictionary
#set(CrapPhoAnalysis_LINKDEF PhoAnalysis/CrapPhoAnalysisLinkDef.h )
set(CrapPhoAnalysis_LINKDEF 
  ${SUSYNTUPLE_SRC}/SusyNtuplizer_LinkDef.h 
  )

set(CrapPhoAnalysis_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/CrapPhoAnalysisDict.cxx) 
ROOT_GENERATE_DICTIONARY("${CrapPhoAnalysis_HEADERS}" "${CrapPhoAnalysis_LINKDEF}" "${CrapPhoAnalysis_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

# add the dictionary to the list of source files
SET(CrapPhoAnalysis_SRCS ${CrapPhoAnalysis_SRCS} ${CrapPhoAnalysis_DICTIONARY}) 


# Set the library version in the main CMakeLists.txt
SET(CrapPhoAnalysis_MAJOR_VERSION 0)
SET(CrapPhoAnalysis_MINOR_VERSION 0)
SET(CrapPhoAnalysis_PATCH_VERSION 0)
SET(CrapPhoAnalysis_VERSION "${CrapPhoAnalysis_MAJOR_VERSION}.${CrapPhoAnalysis_MINOR_VERSION}.${CrapPhoAnalysis_PATCH_VERSION}")
SET(CrapPhoAnalysis_LIBRARY_PROPERTIES ${CrapPhoAnalysis_LIBRARY_PROPERTIES}
    VERSION "${CrapPhoAnalysis_VERSION}"
    SOVERSION "${CrapPhoAnalysis_MAJOR_VERSION}"
    SUFFIX ".so"
)


############### build the library #####################
add_library(CrapPhoAnalysis SHARED ${CrapPhoAnalysis_SRCS})
target_link_libraries(CrapPhoAnalysis ${ROOT_LIBRARIES})
set_target_properties(CrapPhoAnalysis PROPERTIES ${CrapPhoAnalysis_LIBRARY_PROPERTIES})

############### install the library ###################
install(TARGETS CrapPhoAnalysis DESTINATION ${CMAKE_BINARY_DIR}/lib)


############### build the executable ######################
add_executable(CrapPhoAnalysisExe PhoAnalysis/CrapPhoMain.cc)
target_link_libraries(CrapPhoAnalysisExe ${ROOT_LIBRARIES} CrapPhoAnalysis)
#set_target_properties(CrapPhoAnalysisExe)



##########################################################
# CrapLLAnalysis 
##########################################################



# List of source files
set(CrapLLAnalysis_SRCS
${SUSYNTUPLE_SRC}/SusyEvent.cc
LLAnalysis/CrapLLAnalysis.cc
)


# fill list of header files
set(CrapLLAnalysis_HEADERS
SusyEvent.h
LLAnalysis/CrapLLAnalysis.h
)

# Set the library version in the main CMakeLists.txt
SET(CrapLLAnalysis_MAJOR_VERSION 0)
SET(CrapLLAnalysis_MINOR_VERSION 0)
SET(CrapLLAnalysis_PATCH_VERSION 0)
SET(CrapLLAnalysis_VERSION "${CrapLLAnalysis_MAJOR_VERSION}.${CrapLLAnalysis_MINOR_VERSION}.${CrapLLAnalysis_PATCH_VERSION}")
SET(CrapLLAnalysis_LIBRARY_PROPERTIES ${CrapLLAnalysis_LIBRARY_PROPERTIES}
    VERSION "${CrapLLAnalysis_VERSION}"
    SOVERSION "${CrapLLAnalysis_MAJOR_VERSION}"
    SUFFIX ".so"
)

############### build the library #####################
add_library(CrapLLAnalysis SHARED ${CrapLLAnalysis_SRCS})
target_link_libraries(CrapLLAnalysis ${ROOT_LIBRARIES})
set_target_properties(CrapLLAnalysis PROPERTIES ${CrapLLAnalysis_LIBRARY_PROPERTIES})

############### install the library ###################
install(TARGETS CrapLLAnalysis DESTINATION ${CMAKE_BINARY_DIR}/lib)



