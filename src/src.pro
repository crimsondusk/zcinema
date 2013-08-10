TARGET          = ../zandemo
DEPENDPATH     += .
INCLUDEPATH    += .
# RC_FILE         = ../ldforge.rc
# RESOURCES       = ../ldforge.qrc
RCC_DIR         = ./build/
OBJECTS_DIR     = ./build/
MOC_DIR         = ./build/
RCC_DIR         = ./build/
SOURCES         = *.cpp
HEADERS         = *.h
QT             += network
FORMS           = ui/*.ui
QMAKE_CXXFLAGS += -std=c++0x