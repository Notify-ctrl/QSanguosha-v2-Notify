#ifndef PCH_H
#define PCH_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

//#define LOGNETWORK

#include <ft2build.h>

#ifdef __cplusplus

#include <QtCore>
#include <QtNetwork>
#include <QtGui>
#include <QtWidgets>

#include <QtQml>

#ifdef AUDIO_SUPPORT
#include <fmod.hpp>
#endif

#endif

#endif // PCH_H

