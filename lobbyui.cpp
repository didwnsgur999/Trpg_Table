#include "lobbyui.h"
#include "ui_lobbyui.h"
#include "clientchat.h"
#include "chathandler.h"

lobbyUI::lobbyUI(ClientChat* clientChat,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::lobbyUI),m_clientChat(clientChat)
{
    ui->setupUi(this);
    connect(clientChat->getChatHandler(),&ChatHandler::chatreceived,this,[this](const QString&msg){
        ui->textEdit->append("서버 send" + msg);
    });
}

lobbyUI::~lobbyUI()
{
    delete ui;
}

void lobbyUI::on_join_server_clicked()
{
    qDebug()<<"onjoinserver_clicked";
    QString ipNum = ui->ipEdit->text();
    int portNum = ui->portEdit->text().toInt();

    m_clientChat->makeSocket(ipNum,portNum,this);
    //일단 이 뒤로 안나오니까
    connect(m_clientChat, &ClientChat::newMessage, this, [this](const QString& msg){
        // textEdit에 메시지를 추가
        ui->textEdit->append("서버: " + msg);
    });
}

void lobbyUI::on_join_server_error(QAbstractSocket::SocketError socketError){
    ui->textEdit->append("[server]failed connection. errorcode: "+QString::number(socketError));
}

void lobbyUI::on_goto_prod_clicked()
{
    emit requestPageChange(0);
}

void lobbyUI::on_sendButton_clicked()
{
    QString message = ui->sendMesgEdit->text();

    QJsonObject obj;
    obj["cmd"]="chat";
    obj["text"] = message;

    QJsonDocument doc(obj);
    //JsonDocument로 넘겨야 된다.
    m_clientChat->sendData(doc);
}

/* 비번 관련해서 잠시 삭제
void lobbyUI::on_loginButton_clicked()
{
    QString name = ui->nameEdit->text();
    //QString pwd = ui->pwdEdit->text();
    QJsonObject obj;
    obj["cmd"]="login";
    obj["cName"]=name;
    obj["cPwd"]=pwd;
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}
*/
