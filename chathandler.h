#ifndef CHATHANDLER_H
#define CHATHANDLER_H

#include <QObject>
#include <QString>
#include <QJsonObject>

class ChatHandler : public QObject
{
    Q_OBJECT
public:
    explicit ChatHandler(QObject* parent = nullptr);
    void getByteData(QByteArray& data);

private:

};

#endif // CHATHANDLER_H
