#ifndef STOREUI_H
#define STOREUI_H

#include <QWidget>
#include "mycore/clientchat.h"
#include <QHash>
#include "mycore/product.h"
#include <QListWidgetItem>

namespace Ui {
class storeUI;
}

class storeUI : public QWidget
{
    Q_OBJECT

public:
    explicit storeUI(ClientChat* clientChat,QWidget *parent = nullptr);
    ~storeUI();

    void loadProductList(const QJsonArray& array);
    void loadUserProduct();
    void resetStore();
    QString encodePixmapToBase64(const QPixmap& pixmap);
signals:
    void requestPageChange(int index);

private slots:

    void on_resetButton_clicked();

    void on_storeListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_storeListWidget_itemEntered(QListWidgetItem *item);

private:
    Ui::storeUI *ui;
    ClientChat* m_clientChat;
    QHash<QString,QSharedPointer<Product>> productlist;
};

#endif // STOREUI_H
