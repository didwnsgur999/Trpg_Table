#include "lobbyui.h"
#include "ui_lobbyui.h"
#include "clientchat.h"

lobbyUI::lobbyUI(Backend* backend,QWidget *parent)
    : QWidget(parent),m_backend(backend)
    , ui(new Ui::lobbyUI)
{
    ui->setupUi(this);
}

lobbyUI::~lobbyUI()
{
    delete ui;
}

void lobbyUI::on_join_server_clicked()
{
    QString rname = ui->nameEdit->text();
    QString ipNum = ui->ipEdit->text();
    int portNum = ui->portEdit->text().toInt();

    auto client_chat= new ClientChat();
    m_clientchat = client_chat;
    client_chat->makeSocket(ipNum,portNum);
    connect(m_clientchat, &ClientChat::newMessage, this, [this](const QString& msg){
        // 예: textEdit에 메시지를 추가
        ui->textEdit->append("Server: " + msg);
    });
}

void lobbyUI::on_goto_prod_clicked()
{
    emit requestPageChange(0);
}

void lobbyUI::on_sendButton_clicked()
{
    QString message = ui->sendMesgEdit->text();
    m_clientchat->sendData(message);
}
