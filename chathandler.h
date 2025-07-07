#ifndef CHATHANDLER_H
#define CHATHANDLER_H

#include <QObject>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug> // 디버깅용

class ChatHandler : public QObject
{
    Q_OBJECT
public:
    explicit ChatHandler(QObject* parent = nullptr);

    void getByteData(QByteArray& data);

signals:
    void chatreceived(const QString& msg);
    // 새로 추가된 시그널: 로그인 결과 (성공 여부, 메시지)
    void loginResult(bool success, const QString& message);

private:
};

#endif // CHATHANDLER_H
