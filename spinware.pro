purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= qt release thread warn_on
LANGUAGE	= C++

greaterThan(QT_MAJOR_VERSION, 4) {
QT		+= concurrent widgets
}

TEMPLATE	= app

QMAKE_CLEAN	+= spinware
QMAKE_CXXFLAGS_RELEASE += -Wall \
			  -Wconversion \
                          -Wdouble-promotion \
                          -Werror \
                          -Wextra \
                          -Wformat=2 \
                          -Wpointer-arith \
                          -Wsign-conversion \
                          -Wstack-protector \
                          -Wstrict-overflow=5 \
                          -fPIE \
                          -fno-common \
                          -fstack-protector-all \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++11

greaterThan(QT_MAJOR_VERSION, 4) {
QMAKE_CXXFLAGS_RELEASE += -Wconversion \
                          -Wsign-conversion
}

QMAKE_EXTRA_TARGETS = purge

INCLUDEPATH	+= Source

FORMS           = UI/spinware.ui \
		  UI/spinware_page.ui

HEADERS		= Source/spinware.h \
		  Source/spinware_page.h

RESOURCES       = Icons/icons.qrc

SOURCES		= Source/spinware.cc \
                  Source/spinware_main.cc \
		  Source/spinware_page.cc \
                  Source/spinware_tasks.cc

PROJECTNAME	= spinware
TARGET		= spinware
