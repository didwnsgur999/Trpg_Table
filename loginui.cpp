#include "loginui.h"
#include "ui_loginui.h"
#include "chathandler.h" // ChatHandler 사용을 위해 포함
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox> // 메시지 박스 사용을 위해 포함
#include <QDebug> // qDebug() 사용을 위해 추가

LoginUI::LoginUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginUI)
    , m_clientChat(clientChat)
{
    ui->setupUi(this);

    connect(m_clientChat->getChatHandler(), &ChatHandler::loginResult,
            this, &LoginUI::handleLoginResult);
}

LoginUI::~LoginUI()
{
    delete ui;
}

void LoginUI::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    /* 일단 무조건 로그인 화면으로 넘어가도록 주석 처리한 상황!
    // 사용자 입력 유효성 검사
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "로그인 오류", "아이디 또는 비밀번호를 입력해주세요.");
        return; // 이 경우 함수 종료
    }*/

    // 현재 이 코드는 소켓이 연결되지 않아 프로그램 충돌의 원인이 되어 프로그램 종료되니까
    // 나중에 서버 연결 로직이 로그인 전에 확립되면 다시 활성화할 것!! 잊지 말기!
    /*
    QJsonObject obj;
    obj["cmd"] = "login";
    obj["cName"] = username;
    obj["cPwd"] = password;

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc); // 이 호출이 현재 충돌의 원인
    qDebug() << "로그인 요청 전송: " << doc.toJson(QJsonDocument::Compact);
    */
    // ------------------------------------------------------------------

    // 서버 연결 상태와 관계없이 로비 화면으로 즉시 전환하기 위한 코드
    emit requestPageChange(1);
}

// handleLoginResult 함수는 서버가 응답을 보내야 호출되므로,
// 위에서 즉시 페이지를 전환하는 임시 로직에서는 직접적인 영향없음
// 나중에 서버 인증 로직이 완성되면 handleLoginResult를 통해 페이지를 전환하도록 변경해야 함!!
void LoginUI::handleLoginResult(bool success, const QString& message)
{
    if (success) {
        QMessageBox::information(this, "로그인 성공", "환영합니다, " + ui->usernameLineEdit->text() + "님!");
        emit requestPageChange(1);
    } else {
        QMessageBox::warning(this, "로그인 실패", "로그인 실패: " + message);
    }
}
