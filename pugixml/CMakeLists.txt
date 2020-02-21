
add_library(PUGIXML STATIC pugiconfig.hpp pugixml.cpp)
target_include_directories(KKTPiritLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})