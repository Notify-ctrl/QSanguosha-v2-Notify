# -------------------------------------------------
# Project created by QtCreator 2010-06-13T04:26:52
# -------------------------------------------------
TARGET = QSanguosha
QT += network widgets
QT += qml quick
QML_IMPORT_NAME = Sanguosha
QML_IMPORT_VERSION = 1.0
TEMPLATE = app

CONFIG(release,debug|release){
# fmod will prevent us to debug
CONFIG += audio
}

CONFIG += lua
CONFIG += qmltypes
CONFIG -= flat

CONFIG += precompile_header
PRECOMPILED_HEADER = src/pch.h
DEFINES += USING_PCH

SOURCES += \
    src/main.cpp \
    src/client/aux-skills.cpp \
    src/client/client.cpp \
    src/client/clientplayer.cpp \
    src/client/clientstruct.cpp \
    src/core/banpair.cpp \
    src/core/card.cpp \
    src/core/engine.cpp \
    src/core/general.cpp \
    src/core/json.cpp \
    src/core/lua-wrapper.cpp \
    src/core/player.cpp \
    src/core/protocol.cpp \
    src/core/settings.cpp \
    src/core/skill.cpp \
    src/core/structs.cpp \
    src/core/util.cpp \
    src/package/exppattern.cpp \
    src/package/god.cpp \
    src/package/maneuvering.cpp \
    src/package/nostalgia.cpp \
    src/package/package.cpp \
    src/package/standard.cpp \
    src/package/standard-cards.cpp \
    src/package/standard-generals.cpp \
    src/package/standard-skillcards.cpp \
    src/package/thicket.cpp \
    src/package/wind.cpp \
    src/scenario/boss-mode-scenario.cpp \
    src/scenario/couple-scenario.cpp \
    src/scenario/guandu-scenario.cpp \
    src/scenario/miniscenarios.cpp \
    src/scenario/scenario.cpp \
    src/scenario/scenerule.cpp \
    src/scenario/zombie-scenario.cpp \
    src/server/ai.cpp \
    src/server/gamerule.cpp \
    src/server/generalselector.cpp \
    src/server/room.cpp \
    src/server/roomthread.cpp \
    src/server/server.cpp \
    src/server/serverplayer.cpp \
    src/package/hegemony.cpp \
    src/scenario/fancheng-scenario.cpp \
    src/core/room-state.cpp \
    src/core/wrapped-card.cpp \
    src/core/record-analysis.cpp \
    src/package/bgm.cpp \
    src/package/fire.cpp \
    src/package/h-formation.cpp \
    src/package/h-momentum.cpp \
    src/package/mountain.cpp \
    src/package/sp.cpp \
    src/package/yjcm.cpp \
    src/package/yjcm2012.cpp \
    src/package/yjcm2013.cpp \
    src/package/yjcm2014.cpp \
    src/ui/pcconsolestartdialog.cpp \
    src/ui/roomscene.cpp \
    src/ui/startgamedialog.cpp \
    src/ui/startserverdialog.cpp \
    swig/sanguosha_wrap.cxx \
    src/package/jsp.cpp \
    src/util/detector.cpp \
    src/util/nativesocket.cpp \
    src/util/recorder.cpp

HEADERS += \
    src/client/aux-skills.h \
    src/client/client.h \
    src/client/clientplayer.h \
    src/client/clientstruct.h \
    src/core/audio.h \
    src/core/banpair.h \
    src/core/card.h \
    src/core/compiler-specific.h \
    src/core/engine.h \
    src/core/general.h \
    src/core/json.h \
    src/core/lua-wrapper.h \
    src/core/player.h \
    src/core/protocol.h \
    src/core/settings.h \
    src/core/skill.h \
    src/core/structs.h \
    src/core/util.h \
    src/main.h \
    src/package/exppattern.h \
    src/package/god.h \
    src/package/maneuvering.h \
    src/package/nostalgia.h \
    src/package/package.h \
    src/package/standard.h \
    src/package/standard-equips.h \
    src/package/standard-skillcards.h \
    src/scenario/boss-mode-scenario.h \
    src/scenario/couple-scenario.h \
    src/scenario/guandu-scenario.h \
    src/scenario/miniscenarios.h \
    src/scenario/scenario.h \
    src/scenario/scenerule.h \
    src/scenario/zombie-scenario.h \
    src/server/ai.h \
    src/server/gamerule.h \
    src/server/generalselector.h \
    src/server/room.h \
    src/server/roomthread.h \
    src/server/server.h \
    src/server/serverplayer.h \
    src/ui/pcconsolestartdialog.h \
    src/ui/roomscene.h \
    src/ui/startgamedialog.h \
    src/ui/startserverdialog.h \
    src/util/detector.h \
    src/util/nativesocket.h \
    src/util/recorder.h \
    src/util/socket.h \
    src/core/record-analysis.h \
    src/package/hegemony.h \
    src/scenario/fancheng-scenario.h \
    src/package/bgm.h \
    src/package/fire.h \
    src/package/h-formation.h \
    src/package/h-momentum.h \
    src/package/mountain.h \
    src/package/sp.h \
    src/package/yjcm.h \
    src/package/yjcm2012.h \
    src/package/yjcm2013.h \
    src/package/yjcm2014.h \
    src/core/room-state.h \
    src/core/wrapped-card.h \
    src/package/thicket.h \
    src/package/wind.h \
    src/package/jsp.h \
    src/pch.h

FORMS +=

OTHER_FILES += \
    script/* \
    script/ui/* \
    script/ui/Util/* \
    script/ui/RoomElement/* \
    script/ui/Dialog/*


CONFIG(buildbot) {
    DEFINES += USE_BUILDBOT
    SOURCES += src/bot_version.cpp
}


INCLUDEPATH += include
INCLUDEPATH += src/client
INCLUDEPATH += src/core
INCLUDEPATH += src/dialog
INCLUDEPATH += src/package
INCLUDEPATH += src/scenario
INCLUDEPATH += src/server
INCLUDEPATH += src/ui
INCLUDEPATH += src/util

win32{
    RC_FILE += resource/icon.rc
}

macx{
    ICON = resource/icon/sgs.icns
    CONFIG += windeployqt
}

LIBS += -L.
win32-g++{
    DEFINES += WIN32
    LIBS += -L"$$_PRO_FILE_PWD_/lib/win/MinGW"
    DEFINES += GPP
}
macx{
    DEFINES += MAC
    LIBS += -L"$$_PRO_FILE_PWD_/lib/mac/lib"
}
ios{
    DEFINES += IOS
    CONFIG(iphonesimulator){
        LIBS += -L"$$_PRO_FILE_PWD_/lib/ios/simulator/lib"
    }
    else {
        LIBS += -L"$$_PRO_FILE_PWD_/lib/ios/device/lib"
    }
}
linux{
    android{
        DEFINES += ANDROID
        ANDROID_LIBPATH = $$_PRO_FILE_PWD_/lib/android/$$ANDROID_ARCHITECTURE/lib
        LIBS += -L"$$ANDROID_LIBPATH"
    }
    else {
        DEFINES += LINUX
        !contains(QMAKE_HOST.arch, x86_64) {
            LIBS += -L"$$_PRO_FILE_PWD_/lib/linux/x86"
            QMAKE_LFLAGS += -Wl,--rpath=lib/linux/x86
        }
        else {
            LIBS += -L"$$_PRO_FILE_PWD_/lib/linux/x64"
            QMAKE_LFLAGS += -Wl,--rpath=lib/linux/x64
        }
    }
}

CONFIG(audio){
    DEFINES += AUDIO_SUPPORT
    INCLUDEPATH += include/fmod
    CONFIG(debug, debug|release): LIBS += -lfmodexL
    else:LIBS += -lfmodex
    SOURCES += src/core/audio.cpp

    android{
        CONFIG(debug, debug|release):ANDROID_EXTRA_LIBS += $$ANDROID_LIBPATH/libfmodexL.so
        else:ANDROID_EXTRA_LIBS += $$ANDROID_LIBPATH/libfmodex.so
    }
}

CONFIG(lua){

android:DEFINES += "\"getlocaledecpoint()='.'\""

    SOURCES += \
        src/lua/lzio.c \
        src/lua/lvm.c \
        src/lua/lundump.c \
        src/lua/ltm.c \
        src/lua/ltablib.c \
        src/lua/ltable.c \
        src/lua/lstrlib.c \
        src/lua/lstring.c \
        src/lua/lstate.c \
        src/lua/lparser.c \
        src/lua/loslib.c \
        src/lua/lopcodes.c \
        src/lua/lobject.c \
        src/lua/loadlib.c \
        src/lua/lmem.c \
        src/lua/lmathlib.c \
        src/lua/llex.c \
        src/lua/liolib.c \
        src/lua/linit.c \
        src/lua/lgc.c \
        src/lua/lfunc.c \
        src/lua/ldump.c \
        src/lua/ldo.c \
        src/lua/ldebug.c \
        src/lua/ldblib.c \
        src/lua/lctype.c \
        src/lua/lcorolib.c \
        src/lua/lcode.c \
        src/lua/lbitlib.c \
        src/lua/lbaselib.c \
        src/lua/lauxlib.c \
        src/lua/lapi.c
    HEADERS += \
        src/lua/lzio.h \
        src/lua/lvm.h \
        src/lua/lundump.h \
        src/lua/lualib.h \
        src/lua/luaconf.h \
        src/lua/lua.hpp \
        src/lua/lua.h \
        src/lua/ltm.h \
        src/lua/ltable.h \
        src/lua/lstring.h \
        src/lua/lstate.h \
        src/lua/lparser.h \
        src/lua/lopcodes.h \
        src/lua/lobject.h \
        src/lua/lmem.h \
        src/lua/llimits.h \
        src/lua/llex.h \
        src/lua/lgc.h \
        src/lua/lfunc.h \
        src/lua/ldo.h \
        src/lua/ldebug.h \
        src/lua/lctype.h \
        src/lua/lcode.h \
        src/lua/lauxlib.h \
        src/lua/lapi.h
    INCLUDEPATH += src/lua
}



!build_pass{
    system("lrelease $$_PRO_FILE_PWD_/builds/sanguosha.ts -qm $$_PRO_FILE_PWD_/sanguosha.qm")

    SWIG_bin = "swig"
    contains(QMAKE_HOST.os, "Windows"): SWIG_bin = "$$_PRO_FILE_PWD_/tools/swig/swig.exe"

    system("$$SWIG_bin -c++ -lua $$_PRO_FILE_PWD_/swig/sanguosha.i")
}

TRANSLATIONS += builds/sanguosha.ts

CONFIG(debug, debug|release): LIBS += -lfreetype_D
else:LIBS += -lfreetype

INCLUDEPATH += $$_PRO_FILE_PWD_/include/freetype
DEPENDPATH += $$_PRO_FILE_PWD_/include/freetype

ANDROID_PACKAGE_SOURCE_DIR = $$_PRO_FILE_PWD_/resource/android

DISTFILES += \
    script/ui/RoomElement/LightBox.qml \
    script/ui/RoomElement/SpecialArea.qml \
    script/ui/Util/util.js
