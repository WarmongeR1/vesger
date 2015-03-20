
#ifndef SCREEN_H
#define SCREEN_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

class PaintArea;

class screen : public QWidget
{

    Q_OBJECT

public:
    screen();

private:
    QLabel *label;
    PaintArea *paintArea;

protected:
    virtual void mousePressEvent(QMouseEvent *event);

signals:
    void colorSelected(QColor color);
    void colorSelectedBack(QColor color);
};

#endif // SCREEN_H
