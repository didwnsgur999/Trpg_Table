#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QWidget>

namespace Ui {
class Example;
}

class Example : public QWidget
{
    Q_OBJECT

public:
    explicit Example(QWidget *parent = nullptr);
    ~Example();

private:
    Ui::Example *ui;
};

#endif // EXAMPLE_H
