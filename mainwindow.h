#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "clientchat.h"
#include "productUI.h"
// #include "lobbyui.h"     // 기존 lobbyUI는 이제 LobbyMainUI 안에서 사용되므로 여기서는 제거
#include "loginui.h"
#include "lobbymainui.h" // 새로 추가된 LobbyMainUI 포함

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void changePage(int index);      // 페이지 전환 슬롯

private slots:
    void on_ProductButton_clicked(); // 관리자 UI 관련 버튼 슬롯

private:
    Ui::MainWindow *ui;
    ClientChat* m_clientChat; // ClientChat 인스턴스

    LoginUI* m_loginUI;
    ProductUI* m_productUI;
    // lobbyUI* m_lobbyUI; // 기존 lobbyUI는 이제 LobbyMainUI의 자식으로 관리됩니다.
    LobbyMainUI* m_lobbyMainUI; // 로비 메인 UI (새로 추가)
};
#endif // MAINWINDOW_H
