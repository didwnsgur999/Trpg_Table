#ifndef LOBBYUI_H
#define LOBBYUI_H

#include <QWidget>
class ClientChat;
class Backend;

namespace Ui {
class lobbyUI;
}

class lobbyUI : public QWidget
{
    Q_OBJECT

public:
    explicit lobbyUI(Backend* backend,QWidget *parent = nullptr);
    ~lobbyUI();

signals:
    void requestPageChange(int index);

private slots:
    void on_join_server_clicked();

    void on_goto_prod_clicked();

    void on_sendButton_clicked();

private:
    Backend* m_backend;
    Ui::lobbyUI *ui;
    ClientChat* m_clientchat;
};

#endif // LOBBYUI_H
