#ifndef LOBBYMAINUI_H
#define LOBBYMAINUI_H

#include <QWidget>
#include <QSplitter> // 2:1 분할을 위해 QSplitter 사용
#include <QHBoxLayout> // 레이아웃을 위해 포함
#include <QVBoxLayout> // 레이아웃을 위해 포함
#include <QLabel> // 환영합니다 텍스트 등을 위해 포함
#include <QPushButton> // 버튼을 위해 포함
#include <QStackedWidget> // 맵/상점, 채팅방 목록/채팅방 전환을 위해 포함
#include <QListWidget> // 채팅방 목록 표시
#include <QLineEdit> // 채팅방 이름 입력 필드
#include <QJsonArray> // 채팅방 목록 데이터

#include "clientchat.h" // ClientChat을 전달하기 위해 필요
#include "lobbyui.h" // 기존 lobbyUI (채팅방 역할) 포함
#include "chathandler.h" // ChatHandler 시그널 받으려고

class LobbyMainUI : public QWidget
{
    Q_OBJECT

public:
    // ClientChat* clientChat 인자를 받아 ClientChat 인스턴스를 공유
    explicit LobbyMainUI(ClientChat* clientChat, QWidget *parent = nullptr);
    ~LobbyMainUI();

signals:
    // (향후 필요 시) 메인 윈도우에 페이지 전환을 요청하는 시그널
    void requestPageChange(int index);

private slots:
    // 왼쪽 패널: 맵과 상점 전환을 위한 슬롯
    void on_goToShopButton_clicked(); // 맵에서 상점으로 가는 버튼
    void on_goToMapButton_clicked();  // 상점에서 맵으로 돌아가는 버튼

    // 오른쪽 패널: 채팅방 생성 버튼 슬롯
    void on_createChatRoomButton_clicked();
    // 오른쪽 패널: 채팅방 입장 버튼 (임시)
    void on_enterChatRoomButton_clicked();
    // 오른쪽 패널: 채팅방 나가기 버튼 (임시)
    void on_exitChatRoomButton_clicked();

    // 채팅방 목록 및 생성 관련 추가 슬롯
    void requestRoomList(); // 서버에 채팅방 목록 요청
    void updateRoomList(const QJsonArray& roomList); // 서버로부터 받은 목록으로 UI 업데이트
    void handleRoomCreationResult(bool success, const QString& message); // 방 생성 결과 처리
    void handleRoomJoinResult(bool success, const QString& message); // 방 입장 결과 처리
    void handleRoomLeaveResult(bool success, const QString& message); // 방 나가기 결과 처리

private:
    ClientChat* m_clientChat; // ClientChat 인스턴스 (서버 통신용)

    // 레이아웃 관련 멤버 변수
    QSplitter* m_mainSplitter;

    // 왼쪽 패널 (맵/상점)
    QWidget* m_leftPaneWidget;
    QStackedWidget* m_leftStackedWidget; // 맵과 상점을 전환할 스택 위젯
    QWidget* m_mapDisplayWidget; // 맵을 표시할 임시 위젯
    QWidget* m_shopWidget;  // 상점을 표시할 임시 위젯
    QPushButton* m_leftShopButton; // 왼쪽 맵 페이지에서 상점 가는 버튼
    QPushButton* m_leftMapButton; // 왼쪽 상점 페이지에서 맵 가는 버튼


    // 오른쪽 패널 (채팅)
    QWidget* m_rightPaneWidget;
    QLabel* m_welcomeLabel; // "환영합니다!" 라벨
    QPushButton* m_createChatRoomButton; // "채팅방 생성" 버튼
    QStackedWidget* m_rightStackedWidget; // 채팅방 목록과 개별 채팅방 전환용 스택 위젯
    QWidget* m_chatRoomListWidget; // 모든 채팅방 목록을 표시할 임시 위젯
    lobbyUI* m_currentChatRoomUI; // 기존 lobbyUI를 개별 채팅방으로 사용
    QPushButton* m_enterChatRoomButton; // 채팅방 목록에서 입장 버튼
    QPushButton* m_exitChatRoomButton; // 채팅방에서 나가기 버튼

    // 채팅방 목록 및 생성 관련 추가 UI 요소
    QListWidget* m_roomListWidget; // 채팅방 목록 표시
    QPushButton* m_refreshRoomListButton; // 채팅방 목록 새로고침
    QLineEdit* m_createRoomNameLineEdit; // 방 이름 입력 필드
    QPushButton* m_createRoomButton; // 방 생성 버튼
};

#endif // LOBBYMAINUI_H
