#include "mygraphicsview.h"
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsItem>
MyGraphicsView::MyGraphicsView(QWidget *parent)
    :QGraphicsView(parent)
{}

void MyGraphicsView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        qDebug()<<"escape_key enable";
        QList<QGraphicsItem*> selectedItems = scene()->selectedItems();
        for(QGraphicsItem* item: selectedItems){
            int id = item->data(0).toInt();
            emit itemDeleted(id);
        }
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}
