#ifndef _SERVER_H
#define _SERVER_H

class Room;
class QGroupBox;
class QLabel;
class QRadioButton;
class ServerSocket;
class ClientSocket;

#include "src/pch.h"

class Package;

class BanlistDialog : public QDialog
{
    Q_OBJECT

public:
    BanlistDialog(QWidget *parent, bool view = false);

private:
    QList<QListWidget *>lists;
    QListWidget *list;
    int item;
    QStringList ban_list;
    QPushButton *add2nd;
    QMap<QString, QStringList> banned_items;
    QLineEdit *card_to_ban;

private slots:
    void addGeneral(const QString &name);
    void add2ndGeneral(const QString &name);
    void addPair(const QString &first, const QString &second);
    void doAdd2ndButton();
    void doAddButton();
    void doRemoveButton();
    void save();
    void saveAll();
    void switchTo(int item);
};

class ServerDialog : public QDialog
{
    Q_OBJECT

public:
    ServerDialog(QWidget *parent);
    int config();


private:
    QWidget *createBasicTab();
    QWidget *createPackageTab();
    QWidget *createAdvancedTab();
    QWidget *createMiscTab();
    QLayout *createButtonLayout();

    QGroupBox *createGameModeBox();

    QLineEdit *server_name_edit;
    QSpinBox *timeout_spinbox;
    QCheckBox *nolimit_checkbox;
    QCheckBox *random_seat_checkbox;
    QCheckBox *enable_cheat_checkbox;
    QCheckBox *free_choose_checkbox;
    QCheckBox *free_assign_checkbox;
    QCheckBox *free_assign_self_checkbox;
    QLabel *pile_swapping_label;
    QSpinBox *pile_swapping_spinbox;
    QCheckBox *without_lordskill_checkbox;
    QCheckBox *sp_convert_checkbox;
    QSpinBox *maxchoice_spinbox;
    QLabel *lord_maxchoice_label;
    QSpinBox *lord_maxchoice_spinbox;
    QSpinBox *nonlord_maxchoice_spinbox;
    QCheckBox *forbid_same_ip_checkbox;
    QCheckBox *disable_chat_checkbox;
    QCheckBox *second_general_checkbox;
    QLabel *max_hp_label;
    QComboBox *max_hp_scheme_ComboBox;
    QLabel *scheme0_subtraction_label;
    QSpinBox *scheme0_subtraction_spinbox;
    QCheckBox *prevent_awaken_below3_checkbox;
    QComboBox *scenario_ComboBox;
    QComboBox *mini_scene_ComboBox;
    QPushButton *mini_scene_button;
    QPushButton *boss_mode_button;
    QLineEdit *address_edit;
    QLineEdit *port_edit;
    QSpinBox *game_start_spinbox;
    QSpinBox *nullification_spinbox;
    QCheckBox *minimize_dialog_checkbox;
    QCheckBox *ai_enable_checkbox;
    QCheckBox *ai_chat_checkbox;
    QSpinBox *ai_delay_spinbox;
    QCheckBox *ai_delay_altered_checkbox;
    QSpinBox *ai_delay_ad_spinbox;
    QCheckBox *surrender_at_death_checkbox;
    QCheckBox *luck_card_checkbox;
    QComboBox *role_choose_ComboBox;
    QCheckBox *kof_using_extension_checkbox;
    QCheckBox *kof_card_extension_checkbox;
    QComboBox *role_choose_xmode_ComboBox;
    QCheckBox *disable_lua_checkbox;
    QPushButton *select_all_generals_button;
    QPushButton *deselect_all_generals_button;
    QPushButton *select_reverse_generals_button;
    QPushButton *select_all_cards_button;
    QPushButton *deselect_all_cards_button;
    QPushButton *select_reverse_cards_button;


    QButtonGroup *extension_group;
    QButtonGroup *mode_group;

    int accept_type; // -1 means console start while 1 means server start


    QList<QCheckBox *> m_generalPackages;
    QList<QCheckBox *> m_cardPackages;

private slots:
    void setMaxHpSchemeBox();

    void onConsoleButtonClicked();
    void onServerButtonClicked();
    void onDetectButtonClicked();
    void updateButtonEnablility(QAbstractButton *button);

    void doCustomAssign();
    void setMiniCheckBox();
    void editBanlist();

    void selectAllGenerals();
    void deselectAllGenerals();
    void selectReverseGenerals();

    void selectAllCards();
    void deselectAllCards();
    void selectReverseCards();
};

class Scenario;
class ServerPlayer;

class Server : public QObject
{
    Q_OBJECT

public:
    explicit Server(QObject *parent);

    friend class BanIpDialog;

    void broadcast(const QString &msg);
    bool listen();
    void daemonize();
    Room *createNewRoom();
    void signupPlayer(ServerPlayer *player);

private:
    ServerSocket *server;
    Room *current;
    QSet<Room *> rooms;
    QHash<QString, ServerPlayer *> players;
    QSet<QString> addresses;
    QMultiHash<QString, QString> name2objname;
    bool created_successfully;
	int playerCount;

private slots:
    void processNewConnection(ClientSocket *socket);
    void processRequest(const char *request);
    void cleanup();
    void gameOver();

signals:
    void server_message(const QString &);
    void newPlayer(ServerPlayer *player);
};

#endif

