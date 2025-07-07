#include "loginui.h"
#include "ui_loginui.h"
#include "chathandler.h" // ChatHandler 사용을 위해 포함
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox> // 메시지 박스 사용을 위해 포함
#include <QDebug> // qDebug() 사용을 위해 추가
#include <QTcpSocket> // SocketError 위해

LoginUI::LoginUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginUI)
    , m_clientChat(clientChat)
{
    ui->setupUi(this);

    // ChatHandler의 로그인 결과 시그널 연결
    connect(m_clientChat->getChatHandler(), &ChatHandler::loginResult,
            this, &LoginUI::handleLoginResult);

    // ClientChat의 서버 연결 성공/오류 시그널을 LoginUI 슬롯과 연결
    connect(m_clientChat, &ClientChat::connectionEstablished, this, &LoginUI::on_serverConnectionEstablished);
    connect(m_clientChat, &ClientChat::connectionError, this, &LoginUI::on_serverConnectionError);

    // UI에 서버 IP 및 Port 입력 필드 및 연결 상태 표시 필드 추가
    // 초기에는 로그인 관련 UI 비활성화(서버연결 후 활성화되어야 함)
    ui->usernameLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);
    ui->loginButton->setEnabled(false);
    ui->registerButton->setEnabled(false);

    // 기본값 설정 - 서버 IP 및 포트
    ui->serverIpLineEdit->setText("127.0.0.1");
    ui->serverPortLineEdit->setText("30800");

    // 초기 상태 메시지
    ui->connectionStatusLabel->setText("서버 연결 시도중...");

    // 프로그램 실행하자마자 바로 서버 연결 시도
    attemptConnectToServer();
}

LoginUI::~LoginUI()
{
    delete ui;
}

// 서버 연결 시도 함수
void LoginUI::attemptConnectToServer()
{
    QString ip = ui->serverIpLineEdit->text();
    int port = ui->serverPortLineEdit->text().toInt();

    qDebug() << "attemptConnectToServer - 서버 연결 시도 ";
    ui->connectionStatusLabel->setText("서버 연결 시도 중...");
    m_clientChat->makeSocket(ip, port);
}

// 서버 연결 성공 시 호출될 슬롯
void LoginUI::on_serverConnectionEstablished()
{
    qDebug() << "on_serverConnectionEstablished() - 서버 연결 성공";
    ui->connectionStatusLabel->setText("서버 연결 성공!");
    ui->connectionStatusLabel->setStyleSheet("color: green");
    // 서버 정상 연결 시 녹색 텍스트 제공

    // 연결 성공 시 로그인 관련 UI 활성화
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginButton->setEnabled(true);
    ui->registerButton->setEnabled(true);
    ui->serverIpLineEdit->setEnabled(false); // 서버 연결관련 비활성화
    ui->serverPortLineEdit->setEnabled(false);
}

// 서버 연결 오류 발생 시 호출될 슬롯
void LoginUI::on_serverConnectionError(QAbstractSocket::SocketError socketError)
{
    QString errorMessage;
    switch(socketError){
    case QAbstractSocket::ConnectionRefusedError:
        errorMessage = "서버 연결 거부! 서버 확인 바람";
        break;
    case QAbstractSocket::HostNotFoundError:
        errorMessage = "서버 주소 못찾음! IP주소 확인 바람";
        break;
    case QAbstractSocket::SocketTimeoutError:
        errorMessage = "서버 연결 시간 초과";
        break;
    case QAbstractSocket::NetworkError:
        errorMessage = "네트워크 오류 발생";
        break;
    case QAbstractSocket::UnknownSocketError:
        errorMessage = "알 수 없는 서버 오류 발생" + m_clientChat->getSocketErrorString();
        break;
    }
    ui->connectionStatusLabel->setText("연결 실패" + errorMessage);
    ui->connectionStatusLabel->setStyleSheet("color: red;"); // 실패 시 빨간 텍스트

    QMessageBox::critical(this, "서버 연결 오류", errorMessage);
    qDebug() << "서버 연결 오류: " << errorMessage;

    // 연결 실패 시 로그인 관련 UI 비활성화 유지
    ui->usernameLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);
    ui->loginButton->setEnabled(false);
    ui->registerButton->setEnabled(false);
    ui->serverIpLineEdit->setEnabled(true);
    ui->serverPortLineEdit->setEnabled(true);
}

void LoginUI::on_loginButton_clicked()
{
    // 서버 미연결 시 로그인 시도 불가
    if(!m_clientChat->isConnected()){
        QMessageBox::warning(this, "로그인 오류", "서버에 연결 하세요");
        return;
    }

    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    QJsonObject obj;
    obj["cmd"] = "login";
    obj["cName"] = username;
    obj["cPwd"] = password;

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "로그인 요청 전송: " << doc.toJson(QJsonDocument::Compact);

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
    // emit requestPageChange(1);
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
