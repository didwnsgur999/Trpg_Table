#ifndef LOBBYUI_H
#define LOBBYUI_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit> // 채팅메시지 표시용
#include <QLineEdit> // 메시지 입력용
#include <QPushButton> // 메시지 전송용
#include <QTcpSocket>
#include "clientchat.h"

class ClientChat;
class Backend; // 전방 선언 (사용되지 않지만 기존 코드에 있었음)

namespace Ui {
class lobbyUI; // UI 파일 생성할 떄 저장한 클래스 이름
}

class lobbyUI : public QWidget
{
    Q_OBJECT

public:
    explicit lobbyUI(ClientChat* clientchat,QWidget *parent = nullptr);
    ~lobbyUI();

    // 현재 입장한 방 이름을 설정하는 함수
    void setRoomName(const QString& name);

    // 채팅 메시지를 textEdit에 추가하는 새로운 공개 함수
    void appendChatMessage(const QString& message);

    QLabel* m_currentRoomNameLabel;

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
