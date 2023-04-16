purge.commands = rm -f *~ && rm -f */*~

CONFIG		+= qt release thread warn_on
LANGUAGE	= C++

greaterThan(QT_MAJOR_VERSION, 4) {
QT		+= concurrent widgets
}

QMAKE_CLEAN	+= spinware

contains(QMAKE_HOST.arch, ppc) {
QMAKE_CXXFLAGS_RELEASE += -mcpu=powerpc -mtune=powerpc
}

freebsd-* {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Werror \
                          -Wextra \
                          -Wformat=2 \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -Wzero-as-null-pointer-constant \
                          -fPIE \
                          -fno-common \
                          -fstack-protector-all \
                          -fwrapv \
                          -pedantic \
                          -std=c++11
} else:linux {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Werror \
                          -Wextra \
                          -Wformat=2 \
                          -Wlogical-op \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIE \
                          -fno-common \
                          -fstack-protector-all \
                          -fwrapv \
                          -pedantic \
                          -pie \
                          -std=c++11
} else {
QMAKE_CXXFLAGS_RELEASE += -Wall \
                          -Wcast-align \
                          -Wcast-qual \
                          -Wdouble-promotion \
                          -Werror \
                          -Wextra \
                          -Wformat=2 \
                          -Wpointer-arith \
                          -Wstack-protector \
                          -Wstrict-overflow=1 \
                          -Wundef \
                          -fPIE \
                          -fno-common \
                          -fstack-protector-all \
                          -fwrapv \
                          -pedantic \
                          -std=c++11
}

greaterThan(QT_MAJOR_VERSION, 4) {
freebsd-* {
} else {
}
}

greaterThan(QT_MAJOR_VERSION, 5) {
QMAKE_CXXFLAGS_RELEASE += -std=c++17
QMAKE_CXXFLAGS_RELEASE -= -std=c++11
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
TEMPLATE	= app
