#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mycore/clientchat.h"
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

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void changePage(int index);      // 페이지 전환 슬롯

private:
    Ui::MainWindow *ui;
    ClientChat* m_clientChat; // ClientChat 인스턴스

    LoginUI* m_loginUI;
    LobbyMainUI* m_lobbyMainUI; // 로비 메인 UI (새로 추가)
};
#endif // MAINWINDOW_H
