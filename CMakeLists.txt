# Cmake script for project AddInNative
# autor: 1C

CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

PROJECT(PiritNativeAPI)

SET(AddInNative_SRC 
    PiritNativeAPI.cpp
    PiritNativeAPI.h
    dllmain.cpp
    1cv8.cpp
    )


include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/KKTPiritLib)
include_directories(${CMAKE_SOURCE_DIR}/pugixml)

SET (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR})
SET(AddInDef_SRC)

if (UNIX)
	ADD_DEFINITIONS(-D__linux__ )
else()
    SET(AddInDef_SRC PiritNativeAPI.def)
    add_definitions(-DUNICODE -DWIN32)
endif()

add_subdirectory(KKTPiritLib)
add_subdirectory(pugixml)
add_executable(TestConsole ./TestConsole/TestConsole.cpp 1cv8.cpp)
target_link_libraries(TestConsole KKTPiritLib PUGIXML)
#target_include_directories(TestConsole PUBLIC ${CMAKE_SOURCE_DIR}/pugixml)
add_library(PiritNativeAPI SHARED ${AddInNative_SRC} ${AddInDef_SRC})
target_link_libraries(PiritNativeAPI KKTPiritLib PUGIXML)

install(FILES PiritNativeAPI.dll DESTINATION ${CMAKE_SOURCE_DIR}/bin)
#set_target_properties( ${PROJECT_NAME} PROPERTIES
#       OUTPUT_NAME ${PROJECT_NAME}${MySuffix1}${MySuffix2} 
#       POSITION_INDEPENDENT_CODE ON 
#       CXX_STANDARD 11
#       CXX_STANDARD_REQUIRED ON
#   )
