
add_library(KKTPiritLib STATIC CPiritKKT.cpp ceSerial.cpp PiritPacket.cpp convstring.cpp)
target_include_directories(KKTPiritLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})