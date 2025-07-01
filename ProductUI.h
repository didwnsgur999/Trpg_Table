#ifndef PRODUCTUI_H
#define PRODUCTUI_H

#include <QWidget>
class Backend;

namespace Ui {
class ProductUI;
}

class ProductUI : public QWidget
{
    Q_OBJECT

public:
    explicit ProductUI(Backend* backend,QWidget *parent = nullptr);
    ~ProductUI();

private slots:
    void on_pushButton_clicked();

private:
    Backend* m_backend;
    Ui::ProductUI *ui;

};

#endif // PRODUCTUI_H
