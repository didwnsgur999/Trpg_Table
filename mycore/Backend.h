#ifndef BACKEND_H
#define BACKEND_H
#include <QObject>
#include <QVector>
#include <memory>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include "product.h"
#include "customer.h"
#include "roomitem.h"
#include <QSharedPointer>

class Backend : QObject
{
    Q_OBJECT
public:
    //backend도 싱글턴으로 만든다.
    static Backend& getInstance() {
        static Backend instance;
        return instance;
    }
    //유저 처리
    void userInit(int,QString,QString,const QJsonObject&);
    QSharedPointer<Customer> getUser() { return m_user; }


    //방 처리
    void setRoom(QString roomName){
        m_roomname = roomName;
    }
    QString getRoom() {return m_roomname;}
    void addRoomItem(QSharedPointer<RoomItem> item) {m_roomItemList.push_back(item);}
    void delRoomItem(int iid);
    QSharedPointer<RoomItem> searchRoomItem(int iid);
    const QVector<QSharedPointer<RoomItem>>& getRoomItems() const;
    void clearRoomItem();
    void setRoomItems(const QJsonArray& RoomItems);

    //제품 처리
    void setProductFromJsonArr(const QJsonArray& array);
    const QVector<QSharedPointer<Product>>& getProducts() const;
    void addProduct(QSharedPointer<Product> prod);
    QSharedPointer<Product> searchProductId(int id);



    template<typename T>
    bool saveJson(const QString& filename, const QVector<QSharedPointer<T>>& data) {
        QJsonArray array;
        for (const auto& d : data)
            array.append(d->toJson());

        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly))
            return false;

        QJsonDocument doc(array);
        file.write(doc.toJson());
        return true;
    }

    template<typename T>
    bool loadJson(const QString& filename, QVector<QSharedPointer<T>>& dest,
                  std::function<QSharedPointer<T>(const QJsonObject&)> factory) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly))
            return false;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonArray array = doc.array();

        for (const auto& val : array)
            dest.push_back(factory(val.toObject()));

        return true;
    }
private:
    Backend(QObject* parent = nullptr);
    Backend(const Backend& ) {}
    Backend& operator=(const Backend& ) {}
    ~Backend() {}
    QSharedPointer<Customer> m_user;
    QString m_roomname;
    QVector<QSharedPointer<Product>> m_productList;
    QVector<QSharedPointer<RoomItem>> m_roomItemList;
};

#endif // BACKEND_H
