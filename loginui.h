#ifndef LOGINUI_H
#define LOGINUI_H

#include <QWidget>
#include <QSharedPointer> // QSharedPointer 사용을 위해 포함
#include "clientchat.h"   // ClientChat 클래스 사용을 위해 포함
#include <QTcpSocket>     // 소켓 에러 관련하여 안전하게 명시적 처리

class ClientChat; // 전방 선언

namespace Ui {
class LoginUI;
}

class LoginUI : public QWidget
{
    Q_OBJECT

public:
    explicit LoginUI(ClientChat* clientChat, QWidget *parent = nullptr);
    ~LoginUI();

signals:
    // 로그인 성공 시 메인 윈도우에 로비 화면으로 전환을 요청하는 시그널
    void requestPageChange(int index); // 페이지 전환 요청 시그널
    void loginSuccess(); // 로그인 성공 시 로비로 전환하기 위한 시그널
    void showStatusMessage(const QString& message, bool isError = false); // 상태 메시지 표시를 위함

private slots:
    // 로그인 버튼 클릭 시 슬롯
    void on_loginButton_clicked();

    // 서버로부터 로그인 결과를 받았을 때 처리하는 슬롯
    void handleLoginResult(bool success, const QString& message,const QJsonObject& cus);
    void handleRegisterResult(bool success, const QString& message);

    void on_serverConnectionError(QAbstractSocket::SocketError socketError); // 서버 연결 오류 슬롯
    void on_serverConnectionEstablished(); // 서버 연결 성공

    void on_registerButton_clicked();

private:
    Ui::LoginUI *ui;
    ClientChat* m_clientChat; // ClientChat 인스턴스 (서버 통신용)

    void attemptConnectToServer(); // 서버 연결 시도 함수 (생성자에서 호출)
};

#endif // LOGINUI_H
