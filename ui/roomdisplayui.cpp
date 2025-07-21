#include "roomdisplayui.h"
#include "ui_roomdisplayui.h"
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include "graphics/mygraphicsitem.h"
#include "mycore/Backend.h"
#include "mycore/roomitem.h"

RoomDisplayUI::RoomDisplayUI(ClientChat* clientChat,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RoomDisplayUI),m_clientChat(clientChat)
{
    ui->setupUi(this);
    ui->UserItemListWidget->setMaximumWidth(0);
    ui->RoomItemListWidget->setMaximumWidth(0);
    scene = new QGraphicsScene(this);
    ui->RoomGraphicsView->setScene(scene);

    //from clientChat
    connect(m_clientChat->getChatHandler(),&ChatHandler::addRoomItemResult,this,&RoomDisplayUI::addRoomItemHandle);
    connect(m_clientChat->getChatHandler(),&ChatHandler::delRoomItemResult,this,&RoomDisplayUI::delRoomItemHandle);
    connect(m_clientChat->getChatHandler(),&ChatHandler::movRoomItemResult,this,&RoomDisplayUI::movRoomItemServerHandle);

    //from GraphicsView item deleted
    connect(ui->RoomGraphicsView,&MyGraphicsView::itemDeleted,this,[=](auto id){
        QJsonObject obj;
        obj["cmd"] = "del_r_item";
        obj["iid"] = id;
        obj["rName"] = Backend::getInstance().getRoom();

        qDebug()<<"del_r_item id : "<<id;

        QJsonDocument doc(obj);
        m_clientChat->sendData(doc);
    });

}

RoomDisplayUI::~RoomDisplayUI()
{
    delete ui;
}

// sliding window
void RoomDisplayUI::on_ItemButton_clicked()
{
    static bool isOpen = false;

    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);

    QPropertyAnimation *anim1 = new QPropertyAnimation(ui->UserItemListWidget, "maximumWidth", this);
    anim1->setDuration(200);
    anim1->setEasingCurve(QEasingCurve::OutCubic);
    QPropertyAnimation *anim2 = new QPropertyAnimation(ui->RoomItemListWidget, "maximumWidth", this);
    anim2->setDuration(200);
    anim2->setEasingCurve(QEasingCurve::OutCubic);

    if (isOpen) {
        anim1->setStartValue(150);
        anim1->setEndValue(0);
        anim2->setStartValue(150);
        anim2->setEndValue(0);
        ui->ItemButton->setText("<<");
    } else {
        anim1->setStartValue(0);
        anim1->setEndValue(150);
        anim2->setStartValue(0);
        anim2->setEndValue(150);
        ui->ItemButton->setText(">>");
    }

    group->addAnimation(anim1);
    group->addAnimation(anim2);

    group->start();
    isOpen = !isOpen;
    if(isOpen) {
        loadProductList();

    }
}

void RoomDisplayUI::loadProductList(){
    ui->UserItemListWidget->clear();
    //헤더
    QListWidgetItem* labelItem = new QListWidgetItem("유저 소지품");
    labelItem->setFlags(Qt::NoItemFlags);
    labelItem->setTextAlignment(Qt::AlignCenter);
    ui->UserItemListWidget->addItem(labelItem);
    //본문
    const auto& hash = Backend::getInstance().getUser()->getProd();
    for(auto it = hash.begin();it!=hash.end();++it){
        QString name = it.key();
        int id = it.value();
        QString displayText = QString("%1 (ID: %2)").arg(name).arg(id);
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, id);
        item->setData(Qt::UserRole+1, name);
        ui->UserItemListWidget->addItem(item);
    }
}
//========================//
// 유저 아이템을 눌렀을때 처리
//========================//
void RoomDisplayUI::on_UserItemListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    //유저가 들고있는 아이템 더블클릭
    int pid = item->data(Qt::UserRole).toInt();
    QJsonObject obj;
    obj["cmd"] = "add_r_item";
    obj["pid"] = pid;
    obj["rName"] = Backend::getInstance().getRoom();

    qDebug()<<"add_r_item id : "<<pid<<Backend::getInstance().getRoom();

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    //데이터를 보내면 server에서 json으로 ret_add_r_image가 와야된다.
}
void RoomDisplayUI::addRoomItemHandle(const QJsonObject& product){
    //일단 오는지만 볼까?
    QJsonObject itemObj = product["item"].toObject();
    //온거 백엔드에 저장
    auto item = RoomItem::fromJson(itemObj);
    QSharedPointer<RoomItem> itemptr=QSharedPointer<RoomItem>::create(item);
    Backend::getInstance().addRoomItem(itemptr);
    //디버그 메시지
    qDebug()<<item.iid<<item.pid<<item.name<<item.x<<item.y<<item.z;
    //왔으니까 roomlist에 저장해놓고 처리하는게 맞나?
    //여기서 보이는거고
    loadRoomItemList();
    //여기서 디스플레이에 띄워야됨.
    displayItem(item.iid);
}

void RoomDisplayUI::loadRoomItemList(){
    ui->RoomItemListWidget->clear();
    //헤더
    QListWidgetItem* labelItem = new QListWidgetItem("방 현재 물품");
    labelItem->setFlags(Qt::NoItemFlags);
    labelItem->setTextAlignment(Qt::AlignCenter);
    ui->RoomItemListWidget->addItem(labelItem);
    //본문
    const auto& roomItemList = Backend::getInstance().getRoomItems();
    for(auto i:roomItemList){
        QString name = i->name;
        int id = i->iid; int x = i->x; int y = i->y; int z = i->z;
        QString displayText = QString("%1 (%2,%3,%4)").arg(name).arg(x).arg(y).arg(z);
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, id);
        item->setData(Qt::UserRole+1, name);
        item->setData(Qt::UserRole+2,x);
        item->setData(Qt::UserRole+3,y);
        item->setData(Qt::UserRole+4,z);
        ui->RoomItemListWidget->addItem(item);
    }
}
//iid로 roomItem찾고 그걸로 product에서 image가져오고 그걸로처리
void RoomDisplayUI::displayItem(int iid){
    auto roomItem = Backend::getInstance().searchRoomItem(iid);
    auto prod = Backend::getInstance().searchProductId(roomItem->pid);

    auto *item = new MyGraphicsItem(prod->getImage().scaled(150,150),roomItem->iid,roomItem->pid,roomItem->z);
    connect(item,&MyGraphicsItem::positionChanged,this,&RoomDisplayUI::movRoomItemHandle);
    item->setFlag(QGraphicsItem::ItemIsMovable);
    item->setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
    item->setFlag(QGraphicsItem::ItemIsSelectable);
    scene->addItem(item);

    item->setData(0, roomItem->iid);
    item->setData(1,roomItem->pid);
    item->setData(2,roomItem->x);
    item->setData(3,roomItem->y);
    item->setData(4,roomItem->z);
    item->setPos(QPointF(roomItem->x, roomItem->y));
    item->setZValue(roomItem->z);
}
//========================//
// 유저 아이템을 눌렀을때 처리끝
//========================//

//========================//
//더블클릭 = 제거? -> 서버에다가 제거 요청 알리고 대답왔을때 실제 제거는 다른 핸들러에서 한다.
//========================//
void RoomDisplayUI::on_RoomItemListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    //erase room item sequence
    qDebug()<<"erase room item"<<item->data(Qt::UserRole).toInt();
    //send del message
    int iid = item->data(Qt::UserRole).toInt();
    QJsonObject obj;
    obj["cmd"] = "del_r_item";
    obj["iid"] = iid;
    obj["rName"] = Backend::getInstance().getRoom();

    qDebug()<<"del_r_item id : "<<iid;

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
}

void RoomDisplayUI::delRoomItemHandle(const QJsonObject& item){
    int iid = item["iid"].toInt();
    qDebug()<<iid;
    //delete sequence. backend roomitem delete, ui->roomitem erase
    Backend::getInstance().delRoomItem(iid);
    QList<QGraphicsItem*> items = scene->items();
    for (QGraphicsItem* i : items) {
        if (i->data(0)==iid) {
            scene->removeItem(i);
            delete i;
            qDebug() << "Graphics item removed from scene";
            break;
        }
    }
    loadRoomItemList();
}

//====================//
//방 아이템 눌렸을때 처리 -> 이거는 제품 포커스로 쓰자.
//====================//
// void RoomDisplayUI::on_RoomItemListWidget_itemClicked(QListWidgetItem *item)
// {

// }

//====================//
//방 아이템 이동시 처리
//====================//
void RoomDisplayUI::movRoomItemHandle(int id, int newx,int newy, int z){
    qDebug() << "Item moved. ID:" << id << "x:" <<newx << "y:" <<newy << "z:" << z;

    //서버 전송.
    QJsonObject obj;
    obj["cmd"] = "mov_r_item";
    obj["iid"] = id;
    obj["newx"] = newx;
    obj["newy"] = newy;
    obj["z"] = z;
    obj["rName"] = Backend::getInstance().getRoom();

    QJsonDocument doc(obj);
    m_clientChat->sendData(doc);
    //이후 server에서 받은 ret_mov_r_item로 변경해야됨.
}

void RoomDisplayUI::movRoomItemServerHandle(const QJsonObject& item){
    //여기서 iid, finx, finy, finz얻어서 처리.
    //변경해야하는것 -> item iid로 찾아서 위치변경 finx, finy, finz로 변경해서 처리
    //

    int iid = item["iid"].toInt();
    int finx = item["finx"].toInt();
    int finy = item["finy"].toInt();
    int finz = item["finz"].toInt();
    qDebug()<<iid<<finx<<finy<<finz;
    auto myitem = Backend::getInstance().searchRoomItem(iid);
    if(myitem){
        //백엔드 교체
        myitem->x = finx;
        myitem->y = finy;
        myitem->z = finz;
        //scene item 찾아서 교체
        auto curItems = scene->items();
        for(auto i:curItems){
            MyGraphicsItem* myitem = dynamic_cast<MyGraphicsItem*>(i);
            if(myitem && myitem->getIid()==iid){
                myitem->stopTimer();
                myitem->setPos(QPointF(finx,finy));
                myitem->setZValue(finz);
                break;
            }
        }
        //방 리스트 최신화 - 하나만 + 헤더 있으니까 +1해야됨.
        for(int i=1; i<ui->RoomItemListWidget->count()+1; i++){
            QListWidgetItem* listItem = ui->RoomItemListWidget->item(i);
            if(listItem->data(Qt::UserRole).toInt()==iid){
                QString newText = QString("%1 (%2,%3,%4)").arg(listItem->data(Qt::UserRole+1).toString()).arg(finx).arg(finy).arg(finz);
                listItem->setText(newText);
                listItem->setData(Qt::UserRole+2,finx);
                listItem->setData(Qt::UserRole+3,finy);
                listItem->setData(Qt::UserRole+4,finz);
                break;
            }
        }
    }
}

