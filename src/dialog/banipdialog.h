#ifndef BANIPDIALOG_H
#define BANIPDIALOG_H

class Server;
class QListWidget;
class ServerPlayer;

class BanIpDialog : public QDialog
{
    Q_OBJECT

public:
    BanIpDialog(QWidget *parent, Server *server);

private:
    QListWidget *left;
    QListWidget *right;

    Server *server;
    QList<ServerPlayer *> sp_list;

    void loadIPList();
    void loadBannedList();

private slots:
    void addPlayer(ServerPlayer *player);
    void removePlayer();

    void insertClicked();
    void removeClicked();
    void kickClicked();

    void save();

};

#endif // BANIPDIALOG_H
