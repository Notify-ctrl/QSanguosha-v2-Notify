#ifndef QMLROUTER_H
#define QMLROUTER_H

#include <QObject>

// Class for communication between c++ and qml

class QmlRouter : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_ANONYMOUS
public:
    explicit QmlRouter(QObject *parent = nullptr);

signals:

};

extern QmlRouter *Router;

#endif // QMLROUTER_H
