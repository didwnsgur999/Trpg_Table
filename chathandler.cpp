#include "chathandler.h"

ChatHandler::ChatHandler(QObject* parent)
    : QObject(parent)
{}
//json으로 날라온걸 받았겠지? QByteArray로 끌고온다.
void ChatHandler::getByteData(QByteArray& data){
    qDebug("%d\n",__LINE__);
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return;

    QJsonObject obj=doc.object();
    QString cmd = obj.value("cmd").toString();

    //이 아래가 cmd에 따른 json처리 signal slot으로 UI에 알려야 될 때도 있다.
    //connect는 기본적으로 되어있어야한다.
    //if(cmd==)
    if(cmd=="chat"){
        qDebug()<<obj.value("text").toString();
    }
}
