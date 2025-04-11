cmake_minimum_required (VERSION 3.20)

project(xSPIAnalyzer)

add_definitions( -DLOGIC2 )

# Use the C++20 standard
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED YES)

# enable generation of compile_commands.json, helpful for IDEs to locate include files.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# custom CMake Modules are located in the cmake directory.
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(ExternalAnalyzerSDK)

set_property(GLOBAL PROPERTY CXX_STANDARD 20)

set(SOURCES 
src/xSPIAnalyzer.cpp
src/xSPIAnalyzer.h
src/xSPIAnalyzerResults.cpp
src/xSPIAnalyzerResults.h
src/xSPIAnalyzerSettings.cpp
src/xSPIAnalyzerSettings.h
src/xSPISimulationDataGenerator.cpp
src/xSPISimulationDataGenerator.h
src/xSPIAnalyzerFrames.cpp
src/xSPIAnalyzerFrames.h
src/xSPIAnalyzerTypes.h
)

add_analyzer_plugin(${PROJECT_NAME} SOURCES ${SOURCES})
