#ifndef PRODUCTUI_H
#define PRODUCTUI_H

#include <QWidget>
class Backend;
class ClientChat;

namespace Ui {
class ProductUI;
}

class ProductUI : public QWidget
{
    Q_OBJECT

public:
    explicit ProductUI(ClientChat* clientChat,QWidget *parent = nullptr);
    ~ProductUI();

private slots:
    void on_pushButton_clicked();

    void on_goto_ui_clicked();

signals:
    void requestPageChange(int index);

private:
    Ui::ProductUI *ui;
    ClientChat* m_clientChat;
};

#endif // PRODUCTUI_H
