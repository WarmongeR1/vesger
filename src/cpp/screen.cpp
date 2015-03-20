
#include "QLabel"
#include "screen.h"



#include "paintarea.h"

#include <QApplication>
#include <QPixmap>
#include <QDesktopWidget>

screen::screen()
{
    label = new QLabel(this);
    label->setPixmap(QPixmap::grabWindow(QApplication::desktop()->winId()));
    label->move(0,0);
   // label->

 //   paintArea = new PaintArea;
  /*  label->*/

    showFullScreen();
}


void screen::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit colorSelected( label->pixmap()->toImage().pixel( event->pos() ) );
    }
    else
    {
        emit colorSelectedBack( label->pixmap()->toImage().pixel( event->pos() ) );
    }
    close();
}
