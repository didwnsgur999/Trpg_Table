#include "roomdisplayui.h"
#include "ui_roomdisplayui.h"
#include <QPropertyAnimation>
#include "Backend.h"

RoomDisplayUI::RoomDisplayUI(ClientChat* clientChat,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoomDisplayUI),m_clientChat(clientChat)
{
    ui->setupUi(this);
    ui->ItemListWidget->setMaximumWidth(0);
    scene = new QGraphicsScene(this);
}

RoomDisplayUI::~RoomDisplayUI()
{
    delete ui;
}


// sliding window
void RoomDisplayUI::on_ItemButton_clicked()
{
    static bool isOpen = false;

    QPropertyAnimation *anim = new QPropertyAnimation(ui->ItemListWidget, "maximumWidth", this);
    anim->setDuration(200);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    if (isOpen) {
        anim->setStartValue(150);
        anim->setEndValue(0);
        ui->ItemButton->setText("<<");
    } else {
        anim->setStartValue(0);
        anim->setEndValue(150);
        ui->ItemButton->setText(">>");
    }

    anim->start();
    isOpen = !isOpen;
    if(isOpen) loadProductList();
}

void RoomDisplayUI::loadProductList(){
    ui->ItemListWidget->clear();

    const auto& hash = Backend::getInstance().getUser()->getProd();
    for(auto it = hash.begin();it!=hash.end();++it){
        QString name = it.key();
        int id = it.value();
        QString displayText = QString("%1 (ID: %2)").arg(name).arg(id);
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, id);
        item->setData(Qt::UserRole+1, name);
        ui->ItemListWidget->addItem(item);
    }
}

void RoomDisplayUI::on_ItemListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    //request image for display to server
    int pid = item->data(Qt::UserRole).toInt();
    QJsonObject obj;
    obj["cmd"] = "req_image";
    obj["pid"] = pid;

    qDebug()<<"req_image : "<<pid;

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}

