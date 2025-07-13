#include "lobbyui.h"
#include "ui_lobbyui.h"
#include "clientchat.h"
#include "chathandler.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QAbstractSocket> // 소켓 에러
#include <QMessageBox> // QMessageBox 사용
#include <QVBoxLayout> // 레이아웃을 위해 추가

lobbyUI::lobbyUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::lobbyUI) // ui 멤버변수 초기화
    , m_clientChat(clientChat) // m_clientChat 멤버 변수 초기화
{
    qDebug() << "[Client LobbyUI] 생성자 호출 시작";

    // 1. ui->setupUi(this)를 호출하여 .ui 파일에 정의된 위젯들을 생성
    // 위젯 자체에 레이아웃을 설정하도록
    ui->setupUi(this);
    qDebug() << "[Client LobbyUI] ui->setupUi(this) 호출 완료. 현재 위젯의 레이아웃: " << layout();

    // 2. 현재 방 이름을 표시할 라벨 생성
    m_currentRoomNameLabel = new QLabel("현재 방: 없음", this);
    m_currentRoomNameLabel->setAlignment(Qt::AlignCenter);
    qDebug() << "[Client LobbyUI] m_currentRoomNameLabel 생성 완료.";

    // 3. ui->setupUi가 설정한 레이아웃을 가져와서 m_currentRoomNameLabel을 추가
    // .ui 파일의 최상위 레이아웃이 'verticalLayout'이라는 이름으로 설정되어 있다고 가정
    // 만약 다른 이름이거나 직접 QWidget에 레이아웃이 설정되어 있지 않다면 layout()을 사용해야
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(layout()); // 위젯의 현재 레이아웃을 가져옴
    if (mainLayout) {
        mainLayout->insertWidget(0, m_currentRoomNameLabel); // 맨 위에 삽입
        qDebug() << "[Client LobbyUI] m_currentRoomNameLabel을 기존 레이아웃에 성공적으로 추가했습니다.";
    } else {
        qDebug() << "[Client LobbyUI] 심각 경고: ui->setupUi(this)가 설정한 레이아웃을 찾을 수 없습니다. 새로운 QVBoxLayout을 설정합니다.";
        // 이 경우는 거의 없어야 하지만, 만약을 대비한 fallback (UI가 깨질 수 있음)
        QVBoxLayout* fallbackLayout = new QVBoxLayout(this);
        fallbackLayout->addWidget(m_currentRoomNameLabel);

        setLayout(fallbackLayout);
    }

    // ChatHandler로부터 채팅 메시지 수신 연결
    connect(clientChat->getChatHandler(),&ChatHandler::chatreceived,this,[this](const QString&msg){
        ui->textEdit->append("서버 send" + msg); // 채팅 메시지를 textEdit에 추가
    });

    // ClientChat으로부터 서버 연결 성공/오류 시그널 연결
    connect(m_clientChat, &ClientChat::connectionEstablished, this, &lobbyUI::on_serverConnected);
    connect(m_clientChat, &ClientChat::connectionError, this, &lobbyUI::on_join_server_error);

    // 초기 UI 상태 설정: 연결 전에는 채팅 입력 및 전송 버튼 비활성화
    ui->sendMesgEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
    // 서버 IP/포트 입력 및 연결 버튼은 초기에는 활성화
    ui->ipEdit->setText("127.0.0.1"); // 기본 IP 설정
    ui->portEdit->setText("30800"); // 기본 포트 설정
    qDebug() << "[Client LobbyUI] 생성자 호출 종료";
}

lobbyUI::~lobbyUI()
{
    delete ui; // UI 객체 메모리 해제
}

// 현재 입장한 방 이름 설정 함수
void lobbyUI::setRoomName(const QString &name)
{
    if (m_currentRoomNameLabel) { // m_currentRoomNameLabel이 유효한지 확인
        m_currentRoomNameLabel->setText("현재 방: " + name);
        qDebug() << "[Client LobbyUI] 현재 방 이름 설정: " << name;
    } else {
        qDebug() << "[Client LobbyUI] 오류: m_currentRoomNameLabel이 유효하지 않습니다. 방 이름 설정 실패.";
    }
}

// 채팅 메시지를 textEdit에 추가하는 새로운 공개 함수
void lobbyUI::appendChatMessage(const QString& message)
{
    if (ui && ui->textEdit) {
        ui->textEdit->append(message);
        qDebug() << "[Client LobbyUI] 채팅 메시지 추가: " << message;
    } else {
        qDebug() << "[Client LobbyUI] 오류: ui 또는 textEdit이 유효하지 않아 메시지를 추가할 수 없습니다.";
    }
}


void lobbyUI::on_join_server_clicked()
{
    qDebug()<<"onjoinserver_clicked";
    QString ipNum = ui->ipEdit->text(); // IP주소 가져오기
    int portNum = ui->portEdit->text().toInt(); // 포트 번호 가져오기

    // ClientChat의 makeSocket 시그니처 변경 반영(lobbyUI 안쓰도록)
    m_clientChat->makeSocket(ipNum,portNum); // 서버 연결 시도
    ui->textEdit->append(QString("[시스템] 서버 연결 시도 중 IP: %1 PORT: %2").arg(ipNum).arg(portNum));

    // 연결 시도 후 UI 상태 변경: 연결 버튼 및 IP/Port 입력 비활성화
    ui->join_server->setEnabled(false);
    ui->ipEdit->setEnabled(false);
    ui->portEdit->setEnabled(false);

    ui->textEdit->append("[시스템] 서버 연결 시도 중..."); // 상태 메시지 출력
}

// 서버 연결 오류 발생 시 호출되는 슬롯
void lobbyUI::on_join_server_error(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError); // 사용하지 않는 인수는 Q_UNUSED 매크로로 표시
    QString errorMsg = m_clientChat->getSocketErrorString();

    ui->textEdit->append("[시스템] 연결 실패! 오류: " + m_clientChat->getSocketErrorString()); // 에러 메시지 출력
    QMessageBox::critical(this, "연결 오류", QString("서버에 연결할 수 없습니다: %1").arg(errorMsg));

    // 연결 실패 시 채팅 입력/전송 버튼 비활성화 유지
    ui->sendMesgEdit->setEnabled(false);
    ui->sendButton->setEnabled(false);
    // 연결 관련 UI 요소는 다시 활성화하여 재시도 가능하게 함
    ui->join_server->setEnabled(true);
    ui->ipEdit->setEnabled(true);
    ui->portEdit->setEnabled(true);
}

// 서버 연결 성공 시 호출되는 새로운 슬롯
void lobbyUI::on_serverConnected()
{
    ui->textEdit->append("[시스템] 서버에 성공적으로 연결되었습니다!"); // 성공 메시지 출력
    // 연결 성공 시 채팅 입력 및 전송 버튼 활성화
    ui->sendMesgEdit->setEnabled(true);
    ui->sendButton->setEnabled(true);
    // 연결 관련 UI 요소는 계속 비활성화하여 불필요한 재연결 시도 방지
    ui->join_server->setEnabled(false);
    ui->ipEdit->setEnabled(false);
    ui->portEdit->setEnabled(false);
}


void lobbyUI::on_goto_prod_clicked()
{
    emit requestPageChange(0); // 페이지 전환 시그널 발생
}

void lobbyUI::on_sendButton_clicked()
{
    QString message = ui->sendMesgEdit->text(); // 입력된 메시지 가져오기
    if (message.isEmpty()) {
        return; // 빈 메시지는 전송하지 않음
    }

    QJsonObject obj; // JSON 객체 생성
    obj["cmd"] = "chat"; // 명령어 설정
    obj["text"] = message; // 메시지 텍스트 설정
    // m_currentRoomNameLabel이 유효한지 확인하고 텍스트 가져오기
    QString currentRoomName = "없음";
    if (m_currentRoomNameLabel) { // m_currentRoomNameLabel 포인터가 유효한지 확인
        currentRoomName = m_currentRoomNameLabel->text().replace("현재 방: ", "");
    }
    obj["rName"] = currentRoomName; // 수정: m_currentRoomNameLabel에서 직접 가져온 값 사용

    QJsonDocument doc(obj); // JSON 문서 생성
    m_clientChat->sendData(doc); // 서버로 데이터 전송

    ui->sendMesgEdit->clear(); // 입력 필드 초기화
    ui->textEdit->append("나: " + message); // 내가 보낸 메시지도 채팅창에 표시
}
