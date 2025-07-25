#include "lobbymainui.h"
#include "ui_lobbymainui.h"
#include <QDebug> // qDebug() 사용을 위해 추가
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>

#include "roomlistui.h"
#include "chatroomui.h"

LobbyMainUI::LobbyMainUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LobbyMainUI)
    , m_clientChat(clientChat)
{
    // 0711 .cpp 파일과 .ui파일 분리
    ui->setupUi(this); // new 적고 setupUi까지 해줘야 메모리에 올라감
    ui->mainSplitter->setSizes({800,480});

    m_roomListUI = new RoomListUI(m_clientChat, this);
    m_chatRoomUI = new ChatRoomUI(m_clientChat, this);
    m_storeUI = new storeUI(m_clientChat,this);
    m_roomDisplayUI = new RoomDisplayUI(m_clientChat,this);
    // 스택 위젯에 UI 페이지 추가
    ui->rightStackedWidget->addWidget(m_roomListUI);
    ui->rightStackedWidget->addWidget(m_chatRoomUI);
    ui->leftTabWidget->addTab(m_storeUI,tr("상점"));
    ui->leftTabWidget->addTab(m_roomDisplayUI,tr("방"));

    // 찻 화면은 룸리스트 보여주는 거
    ui->rightStackedWidget->setCurrentIndex(0);
    ui->leftTabWidget->setCurrentIndex(0);

    // 로비메인유아이에서 시그널 연결
    connect(m_roomListUI, &RoomListUI::createRoomRequested, this, &LobbyMainUI::changePage);
    connect(m_roomListUI, &RoomListUI::joinRoomRequested, this, &LobbyMainUI::changePage);
    // 로비메인유아이에서 roomleave 신호 받음 -> roomdisplayui 다 지움.
    connect(this,&LobbyMainUI::requestLeaveRoom,m_roomDisplayUI,&RoomDisplayUI::leaveRoom);
    connect(this,&LobbyMainUI::enterChatRoom,m_roomDisplayUI,&RoomDisplayUI::enterRoom);
    //오른쪽 room exit처리.
    connect(m_chatRoomUI, &ChatRoomUI::requestLeaveRoom, [=](){
        ui->rightStackedWidget->setCurrentIndex(0);
        ui->leftTabWidget->setCurrentIndex(0);
        ui->leftTabWidget->setTabEnabled(1,false);
        //roomdisplay 지우기.
        emit requestLeaveRoom();
    });
    //방 초대 왔을때 처리.
    connect(m_clientChat->getChatHandler(),&ChatHandler::inviteReceived,[=](QString rName){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this,
                                      tr("%1 초대").arg(rName),
                                      tr("방 %1 조대를 수락하시겠습니까?").arg(rName),
                                      QMessageBox::Yes | QMessageBox::No);
        QJsonObject obj;
        if (reply == QMessageBox::Yes) {
            emit acceptInvite(rName);
        } else {
            //reject
            QMessageBox::information(this,tr("초대 거부"),tr("초대를 거부하였습니다"));
        }
    });
    //방 나가야하는지 체크
    connect(this,&LobbyMainUI::acceptInvite,m_chatRoomUI,&ChatRoomUI::InviteHandle);
    //입장하기
    connect(this,&LobbyMainUI::acceptInvite,m_roomListUI,&RoomListUI::InviteHandle);
    //밴된 사실 공지 밴처리는 leaveRoom으로 했을거임
    connect(m_clientChat->getChatHandler(),&ChatHandler::bannedreceived,[=](){
        QMessageBox::warning(this,tr("강퇴"),tr("당신은 강퇴되었습니다."));
    });
}

LobbyMainUI::~LobbyMainUI()
{
    delete ui;
}
//오른쪽 페이지 변경 처리.
void LobbyMainUI::changePage(const QString& roomName)
{
    ui->rightStackedWidget->setCurrentIndex(1);
    m_chatRoomUI->setRoomName(roomName);
    //방 들어갔을때만 왼쪽 채팅방 그림탭 enable
    emit enterChatRoom();
    ui->leftTabWidget->setTabEnabled(1,true);
    ui->leftTabWidget->setCurrentIndex(1);
}

// 로비 UI가 활성화될 때 호출될 초기화 함수 구현
void LobbyMainUI::initializeLobby()
{
    m_roomListUI->requestRoomList();
    m_storeUI->resetStore();
    //채팅방에 들어갔을때만 방 클릭할수 있도록
    ui->leftTabWidget->setTabEnabled(1,false);
    ui->leftTabWidget->setTabToolTip(1, "채팅방 입장 시에만 사용할 수 있습니다");
}
//왼쪽 위젯 변경시 처리.
void LobbyMainUI::on_leftTabWidget_currentChanged(int index)
{
    if(!m_firstTabChangeIgnored){
        m_firstStackChangeIgnored = true;
        return;
    }
    if(index==0){
        m_storeUI->resetStore();
    }else{
        //디버그중
    }
}

