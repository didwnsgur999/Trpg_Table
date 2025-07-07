#include "lobbyui.h"
#include "ui_lobbyui.h"
#include "clientchat.h"
#include "chathandler.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QAbstractSocket> // 소켓 에러
#include <QMessageBox> // QMessageBox 사용

lobbyUI::lobbyUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::lobbyUI) // ui 멤버변수 초기화
    , m_clientChat(clientChat) // m_clientChat 멤버 변수 초기화
{
    ui->setupUi(this); // UI 설정

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
    ui->ipEdit->setEnabled(true);
    ui->portEdit->setEnabled(true);
    ui->join_server->setEnabled(true); // 'on_join_server_clicked'
}

lobbyUI::~lobbyUI()
{
    delete ui; // UI 객체 메모리 해제
}

void lobbyUI::on_join_server_clicked()
{
    qDebug()<<"onjoinserver_clicked";
    QString ipNum = ui->ipEdit->text(); // IP주소 가져오기
    int portNum = ui->portEdit->text().toInt(); // 포트 번호 가져오기

    // ClientChat의 makeSocket 시그니처 변경 반영(lobbyUI 안쓰도록)
    m_clientChat->makeSocket(ipNum,portNum); // 서버 연결 시도

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
    ui->textEdit->append("[시스템] 연결 실패! 오류: " + m_clientChat->getSocketErrorString()); // 에러 메시지 출력
    QMessageBox::critical(this, "연결 오류", "서버에 연결할 수 없습니다: " + m_clientChat->getSocketErrorString());

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

    QJsonDocument doc(obj); // JSON 문서 생성
    m_clientChat->sendData(doc); // 서버로 데이터 전송

    ui->sendMesgEdit->clear(); // 입력 필드 초기화
    ui->textEdit->append("나: " + message); // 내가 보낸 메시지도 채팅창에 표시
}

/* 비번 관련해서 잠시 삭제
// 이 함수는 LoginUI에서 처리되어야 하므로 lobbyUI에서는 사용되지 않음.
// 만약 이 코드가 LoginUI에 있었다면, ClientChat::sendData를 통해 로그인 요청을 보냄.
void lobbyUI::on_loginButton_clicked()
{
    QString name = ui->nameEdit->text();
    //QString pwd = ui->pwdEdit->text(); // 비밀번호는 주석 처리되어 있음
    QJsonObject obj;
    obj["cmd"]="login";
    obj["cName"]=name;
    //obj["cPwd"]=pwd; // 비밀번호 필드도 주석 처리되어 있음
    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}
*/
