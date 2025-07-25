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
        //타이머를 통해 100ms당 1회 위치 송신
        if(!m_timer.isActive()){
            m_timer.start(100);
        }
    }
    //끼워넣기
    return QGraphicsPixmapItem::itemChange(change,value);
}
//마우스 드래그 종료시 위치 동기화
void MyGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    //원래 해야되는 동작을 실행
    QGraphicsPixmapItem::mouseReleaseEvent(event);
    if(m_timer.isActive()){
        m_timer.stop();
    }
    m_newpos = this->pos();
    //서버와 동기화.
    emit positionChanged(m_iid,m_newpos.x(),m_newpos.y(),m_z);
}
//서버 동기화시 타이머 멈춤
void MyGraphicsItem::stopTimer(){
    if(m_timer.isActive()){
        m_timer.stop();
    }
}
