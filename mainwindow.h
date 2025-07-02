#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory.h>
#include "productUI.h"
#include "lobbyui.h"

class Backend;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( Backend* backend, QWidget *parent = nullptr);
    ~MainWindow();

    //소유권을 줘야되서 move로 변경해줘야한다.
    void setProductUI(std::unique_ptr<ProductUI> productUi) {m_productUI = std::move(productUi);}
    ProductUI* getProductUI() {return m_productUI.get();}
    void setLobbyUI(std::unique_ptr<lobbyUI> lobbyUi) {m_lobbyUI = std::move(lobbyUi);}
    lobbyUI* getLobbyUI() { return m_lobbyUI.get();}

private slots:
    void on_ProductButton_clicked();

private:
    Backend* m_backend;
    Ui::MainWindow *ui;
    //====================================================//
    //UI widget들이 이곳에 들어있다. => unique ptr로 설정될 예정.
    //====================================================//
    std::unique_ptr<ProductUI> m_productUI;
    std::unique_ptr<lobbyUI> m_lobbyUI;
};
#endif // MAINWINDOW_H
