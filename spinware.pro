purge.commands = rm -f *~ && rm -f rm -f */*~

CONFIG		+= qt release thread warn_on
LANGUAGE	= C++
QT		+=
TEMPLATE	= app

QMAKE_CLEAN	+= spinware
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -Os -Wall -Werror -Wextra \
			  -Wpointer-arith -Wstack-protector \
			  -Wstrict-overflow=3 \
			  -fPIE -fstack-protector-all -pie
QMAKE_EXTRA_TARGETS = purge

INCLUDEPATH	+= Source

FORMS           = UI/spinware.ui

HEADERS		= Source/spinware.h

SOURCES		= Source/spinware.cc \
                  Source/spinware_main.cc

PROJECTNAME	= spinware
TARGET		= spinware
