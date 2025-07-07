#ifndef LOBBYUI_H
#define LOBBYUI_H

#include <QWidget>
#include <QTcpSocket>
#include "clientchat.h"

class ClientChat;
class Backend;

namespace Ui {
class lobbyUI; // UI 파일 생성할 떄 저장한 클래스 이름
}

class lobbyUI : public QWidget
{
    Q_OBJECT

public:
    explicit lobbyUI(ClientChat* clientchat,QWidget *parent = nullptr);
    ~lobbyUI();

signals:
    void requestPageChange(int index); // 페이지 전환 요청 시그널

private slots:
    void on_join_server_clicked(); // 서버 연결 버튼 클릭 시
    void on_join_server_error(QAbstractSocket::SocketError socketError); // 서버 연결 오류 처리
    void on_serverConnected(); // 서버 연결 성공 시 처리 (추가)
    void on_goto_prod_clicked(); // 상품 페이지 이동 버튼 클릭 시
    void on_sendButton_clicked(); // 채팅 메시지 전송 버튼 클릭 시

private:
    Ui::lobbyUI *ui;
    ClientChat* m_clientChat;
};

#endif // LOBBYUI_H
