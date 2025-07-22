#include "lobbymainui.h"
#include "ui_lobbymainui.h"
#include <QDebug> // qDebug() 사용을 위해 추가
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "roomlistui.h"
#include "chatroomui.h"

LobbyMainUI::LobbyMainUI(ClientChat* clientChat, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LobbyMainUI)
    , m_clientChat(clientChat)
{
    // 0711 .cpp 파일과 .ui파일 분리
    ui->setupUi(this); // new 적고 setupUi까지 해줘야 메모리에 올라감

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
    //방 들어갔을때만 enable
    emit enterChatRoom();
    ui->leftTabWidget->setTabEnabled(1,true);
    ui->leftTabWidget->setCurrentIndex(1);

}

// 로비 UI가 활성화될 때 호출될 초기화 함수 구현
void LobbyMainUI::initializeLobby()
{
    m_roomListUI->requestRoomList();
    m_storeUI->resetStore();
    //채팅방 탭도 설정되야됨.

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

    }
}

