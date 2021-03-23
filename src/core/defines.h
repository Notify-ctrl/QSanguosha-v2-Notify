#ifndef DEFINES_H
#define DEFINES_H

#define SERVERLIST_URL_DEFAULTGET "http://fairsgs-fairsgs.stor.sinaapp.com/"
#define SERVERLIST_URL_DEFAULTREG "http://fairsgs.sinaapp.com/reg.php"
#define SERVERLIST_VERSION_SERVERLIST 1
#define SERVERLIST_OFFICIALSERVER "115.159.24.202"

#define REGISTER_QMLTYPE(uri, versionMajor, versionMinor, qmlName) static void __cRegisterQmlType__ ## qmlName()\
{\
    qmlRegisterType<qmlName>(uri, versionMajor, versionMinor, #qmlName);\
}\
Q_COREAPP_STARTUP_FUNCTION(__cRegisterQmlType__ ## qmlName)

#define REGISTER_QMLTYPE_NOT_AVAILABLE(uri, versionMajor, versionMinor, qmlName) static void __cRegisterQmlType__ ## qmlName()\
{\
    qmlRegisterTypeNotAvailable(uri, versionMajor, versionMinor, #qmlName, QString(#qmlName) + QString(" can't be initialized."));\
}\
Q_COREAPP_STARTUP_FUNCTION(__cRegisterQmlType__ ## qmlName)

#endif // DEFINES_H

