#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory.h>
#include "productUI.h"
#include "lobbyui.h"
#include "clientchat.h"

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
    MainWindow( QWidget *parent = nullptr);
    ~MainWindow();

    ClientChat* getClientChat() { return m_clientChat;}

private slots:
    void on_ProductButton_clicked();

private:
    Ui::MainWindow *ui;
    ClientChat* m_clientChat;

    ProductUI* m_productUI;
    lobbyUI* m_lobbyUI;
};
#endif // MAINWINDOW_H
