#ifndef ROOMITEM_H
#define ROOMITEM_H

#include <QString>
#include <QJsonObject>
struct RoomItem
{
    int iid;
    int pid;
    QString name;
    int x;
    int y;
    int z;
    QJsonObject toJson() const {
        QJsonObject obj;
        obj["iid"]=iid;
        obj["pid"]=pid;
        obj["name"]=name;
        obj["x"]=x;
        obj["y"]=y;
        obj["z"]=z;
        return obj;
    }
    static RoomItem fromJson(const QJsonObject& obj){
        RoomItem roomItem;
        roomItem.iid = obj["iid"].toInt();
        roomItem.pid = obj["pid"].toInt();
        roomItem.name = obj["name"].toString();
        roomItem.x = obj["x"].toInt();
        roomItem.y = obj["y"].toInt();
        roomItem.z = obj["z"].toInt();
        return roomItem;
    }
};

#endif // ROOMITEM_H
