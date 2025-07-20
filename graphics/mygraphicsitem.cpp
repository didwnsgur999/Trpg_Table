#include "mygraphicsitem.h"
#include <qDebug>
MyGraphicsItem::MyGraphicsItem(const QPixmap& pixmap, int iid,int pid, int z)
    :QGraphicsPixmapItem(pixmap),m_iid(iid),m_pid(pid),m_z(z)
{
    connect(&m_timer,&QTimer::timeout,this,[=](){
        emit positionChanged(m_iid,m_newpos.x(),m_newpos.y(),m_z);
        //서버와 동기화.
        m_timer.stop();
    });
}

QVariant MyGraphicsItem::itemChange(GraphicsItemChange change, const QVariant& value){
    //끼워넣기 change들중 position이 변경완료되었을때 값만 보낸다.
    if(change == ItemPositionHasChanged){
        m_newpos = value.toPointF();
        qDebug()<<"item"<<m_iid<<"moved to:"<<m_newpos;
        if(!m_timer.isActive()){
            m_timer.start(100);
        }
    }
    //끼워넣기
    return QGraphicsPixmapItem::itemChange(change,value);
}
void MyGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    //원래 해야되는거 하고
    QGraphicsPixmapItem::mouseReleaseEvent(event);
    if(m_timer.isActive()){
        m_timer.stop();
    }
    m_newpos = this->pos();
    emit positionChanged(m_iid,m_newpos.x(),m_newpos.y(),m_z);
    //서버와 동기화.
}
void MyGraphicsItem::stopTimer(){
    if(m_timer.isActive()){
        m_timer.stop();
    }
}
