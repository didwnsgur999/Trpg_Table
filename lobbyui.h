#ifndef LOBBYUI_H
#define LOBBYUI_H

#include <QWidget>
#include <QAbstractSocket>

class ClientChat;
class Backend;

namespace Ui {
class lobbyUI;
}

class lobbyUI : public QWidget
{
    Q_OBJECT

public:
    explicit lobbyUI(ClientChat* clientchat,QWidget *parent = nullptr);
    ~lobbyUI();

signals:
    void requestPageChange(int index);

private slots:
    void on_join_server_clicked();

    void on_goto_prod_clicked();

    void on_sendButton_clicked();

public slots:
    void on_join_server_error(QAbstractSocket::SocketError socketError);

private:
    Ui::lobbyUI *ui;
    ClientChat* m_clientChat;
};

#endif // LOBBYUI_H
