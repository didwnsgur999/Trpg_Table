#ifndef PRODUCT_H
#define PRODUCT_H
#include "Info.h"
#include <QString>
#include <QJsonObject>
#include <QSharedPointer> // shared_ptr, make_shared
#include <QPixmap>
class Product : public Info{
public:
    //기본생성자가 있어야
    Product();
    Product(int id, QString name, int price,int cnt);

    //Product를 Json으로 변경하는 함수
    QJsonObject toJson() const ;

    //Json을 Product로 변경하는 함수.
    //외부에서 객체 없이 부르는 경우 사용해야 하므로 static이 붙는다.
    static QSharedPointer<Product> fromJson(const QJsonObject& obj);

    //getter / setter

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; }
    void setName(QString name) { m_name = name; }
    QString getName() const { return m_name; }
    void setPrice(int price) { m_price = price; }
    int getPrice() const { return m_price; }
    void setCnt(int cnt) { m_cnt = cnt; }
    int getCnt() const { return m_cnt; }
    void setImage(const QPixmap& image) {m_image = image;}
    QPixmap getImage() const {return m_image;}

private:
    int m_id;
    QString m_name;
    int m_price;
    int m_cnt;
    QPixmap m_image;
};
#endif // PRODUCT_H
