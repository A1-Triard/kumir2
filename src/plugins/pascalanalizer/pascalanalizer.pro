TEMPLATE = lib
TARGET = PascalAnalizer

include(../../kumir2plugin.pri)
include(../../shared/extensionsystem/extensionsystem.pri)

HEADERS += \
    pasplugin.h \
    ippc.h

OTHER_FILES = \
    PascalAnalizer.pluginspec \

#LIBS += -L/home/victor/work/kumirfreepascal/compiler -lfreepascal

SOURCES += \
    pasplugin.cpp \
    ippc.cpp

PPCNAME = ppc386
linux-* {
    ARCH = $$system(arch)
    contains(ARCH,x86_64): PPCNAME = ppcx64
    MAKE = make
}
win32 {
    PPCNAME = ppc386.exe
    MAKE = mingw32-make
}


ippcbin.target = $$IDE_BUILD_TREE/libexec/kumir2/ippc
ippcbin.commands = \
    cd $${_PRO_FILE_PWD_}/ippc/rtl && \
    fpcmake && \
    cd $${_PRO_FILE_PWD_}/ippc/compiler && \
    fpcmake && \
    cd $${_PRO_FILE_PWD_}/ippc/ && \
    $$MAKE rtl FPC=$$PPCNAME && \
    $$MAKE compiler FPC=$$PPCNAME && \
    $$QMAKE_MKDIR $$IDE_BUILD_TREE/libexec/kumir2/ && \
    $$QMAKE_COPY $${_PRO_FILE_PWD_}/ippc/compiler/$$PPCNAME $$IDE_BUILD_TREE/libexec/kumir2/ippc
ippcbin.files = $$IDE_BUILD_TREE/libexec/kumir2/ippc
ippcbin.path = libexec/kumir2


QMAKE_EXTRA_TARGETS += ippcbin
PRE_TARGETDEPS += $$IDE_BUILD_TREE/libexec/kumir2/ippc
INSTALLS += ippcbin




