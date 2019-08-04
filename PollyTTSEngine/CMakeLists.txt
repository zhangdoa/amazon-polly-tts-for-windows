cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

################### Variables. ####################
# Change if you want modify path or other values. #
###################################################

set(PROJECT_NAME PollyTTSWindows)
# Output Variables
set(OUTPUT_DEBUG \$(SolutionDir)\$(Platform)/)
set(OUTPUT_REL \$(SolutionDir)\$(Platform)/)
# Folders files
set(CPP_DIR ./)
set(HEADER_DIR_1 )
############## CMake Project ################
#        The main options of project        #
#############################################
OPTION(BUILD_STATIC_LIBS ON)
project(${PROJECT_NAME} CXX)

# Define Release by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "Build type not specified: Use Release by default.")
endif(NOT CMAKE_BUILD_TYPE)

# Definition of Macros
add_definitions(
   -DUSE_IMPORT_EXPORT 
   -D_WINDOWS 
   -D_USRDLL 
   -DTTSENG_EXPORTS 
   -DUNICODE
   -D_UNICODE
)

############## Artefacts Output #################
# Defines outputs , depending Debug or Release. #
#################################################

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_DEBUG}")
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
  set(CMAKE_EXECUTABLE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_REL}")
endif()

################# Flags ################
# Defines Flags for Windows and Linux. #
########################################

if(MSVC)
   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /MDd /Od /Zi /EHsc")
   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /GL /Zi /EHsc")
endif(MSVC)
if(NOT MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   endif()
endif(NOT MSVC)

################ Files ################
#   --   Add files to project.   --   #
#######################################

file(GLOB SRC_FILES
    *.cpp
    PollyTTSEngine.def
    PollyTTSEngine.idl
    *.h
)

ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_i.h ${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_i.c
	DEPENDS PollyTTSEngine.idl
	COMMAND ${CMAKE_COMMAND} -E remove  "${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_i.h" "${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_i.c" "${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_p.c"
	COMMAND midl /W1 /nologo /char signed /env win32 /Oicf /h "${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_i.h" /iid "${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_i.c" /proxy "${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_p.c" /tlb "${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine.tlb" /robust ${CMAKE_CURRENT_SOURCE_DIR}/PollyTTSEngine.idl
	)

# Add library to build.
add_library(${PROJECT_NAME} SHARED
   ${SRC_FILES}
   ${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_i.h ${CMAKE_CURRENT_BINARY_DIR}/PollyTTSEngine_i.c PollyTTSEngine.rc ttsengobj.rgs
)

if(MSVC)
   find_package(spdlog CONFIG REQUIRED)
   find_package(aws-cpp-sdk-text-to-speech CONFIG REQUIRED)
   find_package(aws-cpp-sdk-polly CONFIG REQUIRED)
   find_package(aws-cpp-sdk-core CONFIG REQUIRED)
   find_package(tinyxml2 CONFIG REQUIRED)
   find_package(SDL2 CONFIG REQUIRED)
   target_link_libraries(${PROJECT_NAME} spdlog::spdlog tinyxml2::tinyxml2 aws-cpp-sdk-text-to-speech aws-cpp-sdk-core aws-cpp-sdk-polly SDL2::SDL2 SDL2::SDL2main)
endif(MSVC)
