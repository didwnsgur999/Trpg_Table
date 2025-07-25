#ifndef MYGRAPHICSITEM_H
#define MYGRAPHICSITEM_H

#include <QGraphicsPixmapItem>
#include <QTimer>

class MyGraphicsItem:public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    MyGraphicsItem(const QPixmap& pixmap, int iid,int pid, int z);
    int getIid(){return m_iid;}
    void stopTimer();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void positionChanged(int iid,int newX, int newY, int z);

private:
    int m_iid;
    int m_pid;
    int m_z;
    QPointF m_newpos;
    QTimer m_timer;
};

#endif // MYGRAPHICSITEM_H
