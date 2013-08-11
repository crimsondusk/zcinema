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
FORMS           = ui/*.ui
QMAKE_CXXFLAGS += -std=c++0x

# The widgets are separated into a different module in Qt5, so we need to add
# it here. Doing so under Qt4 just results in a warning, though.
greaterThan (QT_MAJOR_VERSION, 4): QT += widgets