#ifndef STARTGAMEDIALOG_H
#define STARTGAMEDIALOG_H

#include <QQuickItem>

class StartGameDialog : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
public:
    StartGameDialog(QQuickItem *parent = nullptr);
    Q_INVOKABLE void connectToServer();

signals:
    void enterRoom();
    void errorGet(const QString &error_msg);

public slots:
    void networkError(const QString &error_msg);
    void checkVersion(const QString &server_version, const QString &server_mod);
};

#endif // STARTGAMEDIALOG_H
