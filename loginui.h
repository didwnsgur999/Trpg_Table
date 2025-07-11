#ifndef LOGINUI_H
#define LOGINUI_H

#include <QWidget>
#include <QSharedPointer> // QSharedPointer 사용을 위해 포함
#include "clientchat.h"   // ClientChat 클래스 사용을 위해 포함

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
    void requestPageChange(int index);

private slots:
    // 로그인 버튼 클릭 시 슬롯
    void on_loginButton_clicked();
    // (선택 사항) 회원가입 버튼 클릭 시 슬롯 (추후 구현)
    // void on_registerButton_clicked();

    // 서버로부터 로그인 결과를 받았을 때 처리하는 슬롯
    void handleLoginResult(bool success, const QString& message);


private:
    Ui::LoginUI *ui;
    ClientChat* m_clientChat; // ClientChat 인스턴스 (서버 통신용)
};

#endif // LOGINUI_H
