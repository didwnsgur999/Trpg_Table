#include "loginui.h"
#include "ui_loginui.h"
#include "mycore/chathandler.h" // ChatHandler 사용을 위해 포함
#include "mycore/Backend.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox> // 메시지 박스 사용을 위해 포함
#include <QDebug> // qDebug() 사용을 위해 추가
#include <QTcpSocket> // SocketError 위해
#include <QLabel>
#include <QMediaPlayer> // 배경음악
#include <QAudioOutput>

LoginUI::LoginUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginUI)
    , m_clientChat(clientChat)
{
    qDebug() << "[Client LoginUI] 생성자 호출 시작.";
    ui->setupUi(this);

    // qml 연결
    background = new QQuickWidget(this);
    background->setSource(QUrl("qrc:qml/qml/StartScreen.qml"));
    background->setResizeMode(QQuickWidget::SizeRootObjectToView);
    background->setClearColor(Qt::transparent);
    background->setGeometry(this->rect());
    background->lower();  // 배경이니까 맨 아래로

    // 배경음악 (반복재생) -> 로그인 이후에도 계속 하고 싶으면 메인으로 ㄱ
    // (단, 그러면 소리 끄고 키고 조절하고 까지 필요할 거 같아서 걍 여기에 둠)
    QAudioOutput* audioOutput = new QAudioOutput(this);
    audioOutput->setVolume(0.4);
    QMediaPlayer* player = new QMediaPlayer(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl("qrc:/sound/sound/bgm.mp3"));
    player->setLoops(QMediaPlayer::Infinite);
    player->play();

    // ChatHandler의 로그인 결과 시그널 연결
    connect(m_clientChat->getChatHandler(), &ChatHandler::loginResult,
            this, &LoginUI::handleLoginResult);
    connect(m_clientChat->getChatHandler(), &ChatHandler::registerResult,
            this, &LoginUI::handleRegisterResult);
    // ClientChat의 서버 연결 성공/오류 시그널을 LoginUI 슬롯과 연결
    connect(m_clientChat, &ClientChat::connectionEstablished, this, &LoginUI::on_serverConnectionEstablished);
    connect(m_clientChat, &ClientChat::connectionError, this, &LoginUI::on_serverConnectionError);

    // showStatusMessage 시그널을 ui->connectionStatusLabel 업데이트에 연결
    connect(this, &LoginUI::showStatusMessage, this, [this](const QString& message, bool isError){
        ui->connectionStatusLabel->setText(message); // <-- connectionStatusLabel 사용
        // 오류 메시지일 경우 텍스트 색상 변경 등 추가 UI 처리 가능
        if (isError) {
            ui->connectionStatusLabel->setStyleSheet("color: red;");
        } else {
            ui->connectionStatusLabel->setStyleSheet("color: black;");
        }
    });

    // 초기 UI 상태 설정: 연결 전에는 로그인 관련 UI 비활성화
    // server 포트관련 일단 숨겨놓기
    ui->serverIpLineEdit->hide();
    ui->serverPortLineEdit->hide();
    ui->serverIpLabel->hide();
    ui->serverPortLabel->hide();
    ui->ServerConnectButton->hide();

    ui->usernameLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);
    ui->loginButton->setEnabled(false);
    ui->registerButton->setEnabled(false); // registerButton 활성화/비활성화 추가

    // 기본값 설정 - 서버 IP 및 포트
    //ui->serverIpLineEdit->setText("192.168.2.18");
    ui->serverIpLineEdit->setText("127.0.0.1");
    ui->serverPortLineEdit->setText("30800");

    // 초기 상태 메시지
    emit showStatusMessage("서버 연결 시도중...");

    // 프로그램 실행하자마자 바로 서버 연결 시도
    attemptConnectToServer();
    qDebug() << "[Client LoginUI] 생성자 호출 종료.";
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

    qDebug() << "[Client LoginUI] attemptConnectToServer - 서버 연결 시도 IP:" << ip << " Port:" << port;
    emit showStatusMessage("서버 연결 시도 중..."); // <-- showStatusMessage 시그널 사용
    m_clientChat->makeSocket(ip, port);
}

// 서버 연결 성공 시 호출될 슬롯
void LoginUI::on_serverConnectionEstablished()
{
    qDebug() << "[Client LoginUI] on_serverConnectionEstablished() - 서버 연결 성공";
    emit showStatusMessage(" 환영합니다! ", false); // <-- showStatusMessage 시그널 사용

    // 연결 성공 시 로그인 관련 UI 활성화
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginButton->setEnabled(true);
    ui->registerButton->setEnabled(true); // registerButton 활성화
    ui->serverIpLineEdit->setEnabled(false); // serverIpLineEdit 비활성화
    ui->serverPortLineEdit->setEnabled(false); // serverPortLineEdit 비활성화
    ui->ServerConnectButton->setEnabled(false); // serverbutton 비활성화
}

// 서버 연결 오류 발생 시 호출될 슬롯
void LoginUI::on_serverConnectionError(QAbstractSocket::SocketError socketError)
{
    qDebug()<<"here";
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
    default: // 모든 다른 오류를 처리
        errorMessage = "알 수 없는 서버 오류 발생: " + m_clientChat->getSocketErrorString();
        break;
    }
    emit showStatusMessage("연결 실패: " + errorMessage, true); // <-- showStatusMessage 시그널 사용

    QMessageBox::critical(this, "서버 연결 오류", errorMessage);
    qDebug() << "[Client LoginUI] 서버 연결 오류: " << errorMessage;

    // 연결 실패 시 로그인 관련 UI 비활성화 유지
    ui->usernameLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);
    ui->loginButton->setEnabled(false);
    ui->registerButton->setEnabled(false);

    // server관련 활성화
    ui->serverIpLineEdit->setEnabled(true);
    ui->serverPortLineEdit->setEnabled(true);
    ui->ServerConnectButton->setEnabled(true);
    ui->serverIpLineEdit->show();
    ui->serverPortLineEdit->show();
    ui->serverIpLabel->show();
    ui->serverPortLabel->show();
    ui->ServerConnectButton->show();
    ui->connectionStatusLabel->show();
}

void LoginUI::on_loginButton_clicked()
{
    // 서버 미연결 시 로그인 시도 불가
    if(!m_clientChat->isConnected()){
        QMessageBox::warning(this, "로그인 오류", "서버에 연결 하세요");
        qDebug() << "[Client LoginUI] 오류: 서버 미연결, 로그인 시도 불가.";
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
}

void LoginUI::handleLoginResult(bool success, const QString& message,const QJsonObject& cus)
{
    //qDebug() << "[Client LoginUI] handleLoginResult 호출됨. 성공: " << success << ", 메시지: " << message;
    if (success) {
        //여기에서 username이랑 password 정리해서 만들어야됨.
        int id = cus["id"].toInt();
        QString name = cus["name"].toString();
        QString pwd = cus["pwd"].toString();
        QJsonObject prod = cus["myproduct"].toObject();
        Backend::getInstance().userInit(id,name,pwd,prod);
        qDebug()<<"id : "<<Backend::getInstance().getUser()->getId();
        QMessageBox::information(this, "로그인 성공", "환영합니다, " + ui->usernameLineEdit->text() + "님!");
        emit loginSuccess(); // 로그인 성공 시 시그널 방출
        emit requestPageChange(1); // 로그인 성공 시 로비 메인 UI (인덱스 1)로 전환
    } else {
        QMessageBox::warning(this, "로그인 실패", "로그인 실패: " + message);
        // 로그인 실패 시 다시 입력할 수 있도록 활성화
        ui->usernameLineEdit->setEnabled(true);
        ui->passwordLineEdit->setEnabled(true);
        ui->loginButton->setEnabled(true);
        ui->registerButton->setEnabled(true); // registerButton 활성화
        emit showStatusMessage(QString("로그인 실패: %1").arg(message), true);
    }
}

void LoginUI::on_registerButton_clicked()
{
    qDebug() << "[Client LOGINUI] 회원가입 버튼 클릭.";
    // 서버 미연결 시 로그인 시도 불가
    if(!m_clientChat->isConnected()){
        QMessageBox::warning(this, "로그인 오류", "서버에 연결 하세요");
        qDebug() << "[Client LoginUI] 오류: 서버 미연결, 로그인 시도 불가.";
        return;
    }

    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    QJsonObject obj;
    obj["cmd"] = "add_c";
    obj["cName"] = username;
    obj["cPwd"] = password;

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    qDebug() << "[Client LoginUI] 회원가입 요청 전송: " << doc.toJson(QJsonDocument::Compact);
}
void LoginUI::handleRegisterResult(bool success, const QString& message,const QJsonObject& cus){
    if (success) {
        //여기에서 username이랑 password 정리해서 만들어야됨.
        int id = cus["id"].toInt();
        QString name = cus["name"].toString();
        QString pwd = cus["pwd"].toString();
        Backend::getInstance().userInit(id,name,pwd,QJsonObject());
        QMessageBox::information(this, "회원가입 성공", "환영합니다, " + ui->usernameLineEdit->text() + "님!");
        qDebug()<<"id : "<<Backend::getInstance().getUser()->getId();
        emit loginSuccess(); // 로그인 성공 시 시그널 방출
        emit requestPageChange(1); // 로그인 성공 시 로비 메인 UI (인덱스 1)로 전환
    } else {
        QMessageBox::warning(this, "회원가입 실패", "로그인 실패: " + message);
        emit showStatusMessage(QString("회원가입 실패: %1").arg(message), true);
    }
}

void LoginUI::on_ServerConnectButton_clicked()
{
    attemptConnectToServer();
}

// qml 연결
void LoginUI::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (background) {
        background->setGeometry(this->rect());
    }
}
