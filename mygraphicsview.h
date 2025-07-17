#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QGraphicsView>

class MyGraphicsView :public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyGraphicsView(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
signals:
    void itemDeleted(const int& id);
};


#endif // MYGRAPHICSVIEW_H
