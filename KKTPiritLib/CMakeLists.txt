SET(KKTPiritLib_SRC 
    CPiritKKT.cpp
    CPiritKKT.h
    ceSerial.cpp
    ceSerial.h
    convstring.h
    PiritPacket.cpp
    PiritPacket.h
    )

add_library(KKTPiritLib STATIC ${KKTPiritLib_SRC})