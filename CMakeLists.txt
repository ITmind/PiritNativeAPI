
CMAKE_MINIMUM_REQUIRED(VERSION 3.15)

PROJECT(PiritNativeAPI VERSION 0.1 LANGUAGES CXX)

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

SET(AddInNative_SRC 
    PiritNativeAPI.cpp
    PiritNativeAPI.h
    dllmain.cpp
    1cv8.cpp
    )

include_directories(${CMAKE_SOURCE_DIR}/include)

#SET (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${CMAKE_SOURCE_DIR})
SET(AddInDef_SRC)

if(NOT BUILD_REVISION)
  set(BUILD_REVISION 0 CACHE STRING "")
else()
  math(EXPR BUILD_REVISION "${BUILD_REVISION} + 1")
endif()

add_definitions(-DBUILD_REVISION=${BUILD_REVISION})

if (UNIX)
	ADD_DEFINITIONS(-D__linux__ )
else()
    SET(AddInDef_SRC PiritNativeAPI.def)
    #add_definitions(-DUNICODE -D_WIN64)
    #-DWIN32
endif()

add_subdirectory(KKTPiritLib)
add_subdirectory(pugixml)

add_executable(TestConsole ./TestConsole/TestConsole.cpp 1cv8.cpp)
# все заголовки, которые ассоциированы с целью dependency, доступны для исходников, принадлежащих цели target
target_link_libraries(TestConsole PRIVATE KKTPiritLib PUGIXML)

add_library(PiritNativeAPI SHARED ${AddInNative_SRC} ${AddInDef_SRC})
target_link_libraries(PiritNativeAPI PRIVATE KKTPiritLib PUGIXML)

#install(TARGETS PiritNativeAPI RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
#install(FILES MANIFEST.xml INFO.xml DESTINATION ${CMAKE_SOURCE_DIR}/bin)
#install(SCRIPT pack.cmake)
#install(CODE "execute_process(COMMAND '${CMAKE_COMMAND}' -E tar cf ${CMAKE_SOURCE_DIR}/bin/abc.zip ${CMAKE_SOURCE_DIR}/bin/info.xml)")
#SET(CPACK_GENERATOR "ZIP")
#INCLUDE(CPack)

#add_custom_target(ZIP_FILES)
#add_custom_command(TARGET ZIP_FILES 
#COMMAND archive.ps)
#set_target_properties( ${PROJECT_NAME} PROPERTIES
#       OUTPUT_NAME ${PROJECT_NAME}${MySuffix1}${MySuffix2} 
#       POSITION_INDEPENDENT_CODE ON 
#       CXX_STANDARD 11
#       CXX_STANDARD_REQUIRED ON
#   )
