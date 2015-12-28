purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= qt release thread warn_on
LANGUAGE	= C++
QT		+=
TEMPLATE	= app

QMAKE_CLEAN	+= spinware
QMAKE_CXXFLAGS_RELEASE += -Wall -Werror -Wextra \
			  -Wpointer-arith -Wstack-protector \
			  -Wstrict-overflow=5 \
			  -fPIE -fstack-protector-all -fwrapv -pie
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
