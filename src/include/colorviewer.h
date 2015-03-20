
#ifndef COLORVIEWER_H
#define COLORVIEWER_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>

class PaintArea;

class ColorViewer : public QWidget
{
    Q_OBJECT

public:
    ColorViewer(QWidget *parent = 0);
    ~ColorViewer();


    void setPen(const QPen &pen);
    QPen pen() const;

    void setColor(const QColor &color);
    QColor color() const;



    QColor actualColor;


public slots:
    void changeColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event);
        PaintArea *paintArea;

private:

    QPen actualPen;
    QBrush actualBrush;





};

#endif // COLORVIEWER_H
