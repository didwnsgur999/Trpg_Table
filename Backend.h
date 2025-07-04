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

    void addProduct(QSharedPointer<Product> prod);

    const QVector<QSharedPointer<Product>>& getProducts() const;

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
    QVector<QSharedPointer<Product>> productList;
};

#endif // BACKEND_H
