#include "global.h"
#include "interfaces.h"
#include "paintarea.h"


#include "ui_gradientdialog.h"


#include "colorviewer.h"

#include <QPainter>
#include <QMouseEvent>
#include <QWidget>
#include <QRubberBand>
#include <QInputDialog>
#include <QPrintDialog>
#include <QColorDialog>
#include <QDir>
#include <QPrinter>
#include "QPoint"

#include <QGradient>
#include <QLinearGradient>

#include "colorviewer.h"
#include "imagetoolwidget.h"

#include <QtImageFilter>



#include <iostream>
#include <math.h>



static const QSize resultSize(200, 200);

#include <QtGui>

PaintArea::PaintArea(QWidget *parent) :
    QWidget(parent),
    theImage(600, 600, QImage::Format_RGB32),  // освной холст
    thickness(3),  //толщина
 /* undoredoimage(600, 600, QImage::Format_RGB32),
    firstcolor(Qt::gray),
    backgroundcolor(Qt::gray)*/
    filterwidget(new ImageToolWidget),
    brushInterface(0), // не помню
    lastPos(0, 0) // для выделения

{
    myint=5;
    rotatint=0;
    modified = false;
    setAttribute(Qt::WA_StaticContents);
    setAttribute(Qt::WA_NoBackground);
    theImage.fill(qRgb(255, 255, 255));
    zoomFactor = 1;
    i=0;
    iforkrivline = -1;
    undoredoimage[0] = theImage;
    imagerotate = theImage;
    fa = 0;
    fab = true;
    a[0]=QPoint(0,0);
    styleall= Qt::SolidLine;
    StyleLineEnd=Qt::RoundCap;
    StyleSoed=Qt::RoundJoin;
    inpaint = Qt::NoBrush;
    sglali = true;
    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    timer2=new QTimer(this);
    connect(timer2,SIGNAL(timeout()),this,SLOT(timerfire2()));
    timer2->start(2);
}


//--------------------------------------------------------------------------------------------------



//--------------Моусэвенты

//--------------------------------------------------------------------------------------------------
void PaintArea::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::RightButton) {rubberBand->hide();}
    if (event->button() == Qt::LeftButton) {

        image2 = theImage;
        lastPoint = event->pos();
        scribbling = true;
       switch (mytoolsselectwin) {
          case 1 :  fab = true; removeSelectArea(); painttext();   break;
          case 2 :  fab = true; removeSelectArea();  break;
          case 3 :  fab = true; removeSelectArea();  break;
          case 4 :  fab = true; removeSelectArea();  break;
          case 5:   fab = true; removeSelectArea();  break;
          case 6:   fab = true; removeSelectArea();  break;
          case 7 :  fab = false;
                boolforpoygon2 = false;

              if (i!=0)
              {
                  paintpolygon(event->pos());
              }
              else
              {
                  a[0] = lastPoint; i=1;
              }

              break;
          case 8 :
                 if (!rubberBand)
                    rubberBand = new QRubberBand(QRubberBand::Rectangle, this);

                rubberBand->setGeometry(1,1,1,1);
                rubberBand->show();
                break;
          case 9 :  fab = false;
              if (iforkrivline==-1)
              {
                  image3 = theImage;
                  b[3] = event->pos();
                  iforkrivline+=1;
              }
              if (iforkrivline!=-1)
              theImage=image3; /*{ paintpolygon(event->pos()); }*/  break;
          case 10:  fab = true; removeSelectArea(); break;
          case 11 : fab = true; removeSelectArea(); break;
          case 12 :  break;
          case 13 :  break;
          case 14 : paintFillClick();
          case 34 : fab = true;
              lastPoint = event->pos();
              pointforpastearea.setX(lastPoint.x()-rubberBand->pos().x());
              pointforpastearea.setY(lastPoint.y()-rubberBand->pos().y());
              break;
       default :       break; }
            }

    viewpoint = event->pos();
    if (!pendingPath.isEmpty()) {
        QPainter painter(&theImage);
        setupPainter(painter);

        const QRectF boundingRect = pendingPath.boundingRect();
        QLinearGradient gradient(boundingRect.topRight(),
                                 boundingRect.bottomLeft());
        gradient.setColorAt(0.0, QColor(firstcolor.red(), firstcolor.green(),
                                        firstcolor.blue(), 63));
        gradient.setColorAt(1.0, QColor(firstcolor.red(), firstcolor.green(),
                                        firstcolor.blue(), 191));
        painter.setBrush(gradient);
        painter.translate(event->pos() - boundingRect.center());
        painter.drawPath(pendingPath);

        pendingPath = QPainterPath();
            modified = true;
                               // mytoolsselectwin = 10;
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        update();   //обновляем холст
    } else {
        if (brushInterface) {
            QPainter painter(&theImage);
            setupPainter(painter);
            const QRect rect = brushInterface->mousePress(brush, painter,
                                                          event->pos());
            update(rect);
        }
        lastPos = event->pos();
    }
}
//--------------------------------------------------------------------------------------------------
void PaintArea::mouseMoveEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {rubberBand->hide();}
    viewpoint = event->pos();
    if ((event->buttons() & Qt::LeftButton) && scribbling && lastPos != QPoint(-1, -1))
    {

   switch (mytoolsselectwin)
        {
           case 2:  theImage.fill(qRgb(255, 255, 255));
                    theImage = image2;
                    paintoval(event->pos());
                    break;
           case 3:  theImage.fill(qRgb(255, 255, 255));
                    theImage = image2;

                    paintRubberORKist(3,event->pos());
                    break;
           case 4 : theImage.fill(qRgb(255, 255, 255));
                    theImage = image2;
                    paintrectangle(event->pos());
                    break;
           case 5 : paintRubberORKist(1,event->pos()); break;
           case 6 : paintRubberORKist(2,event->pos()); break;
           case 7 : theImage.fill(qRgb(255, 255, 255));
                    theImage = image2;
                    boolforpoygon2 = false;
                    paintpolygon(event->pos());
                    break; /* Многоугольник */

           case 8 : rubberBand->setGeometry(QRect(lastPoint,event->pos()).normalized()); break;    /*выделение*/
           case 9 : theImage.fill(qRgb(255, 255, 255));
                    theImage = image2;
                    theImage = image3;
                    paintbezier(event->pos());
                    break; /* Кривая линия */
           case 10 : paintAirBrush(event->pos()); break;
           case 11 : paintRandomChar(event->pos()); break;
           case 12 : break;
           case 13 : break;
           case 14 :
           case 34 :
               theImage.fill(qRgb(255, 255, 255));
               theImage = image2;

               pasteArea(event->pos());
               break;
        default :       break;

        }

        if (brushInterface) {
            QPainter painter(&theImage);
            setupPainter(painter);
            const QRect rect = brushInterface->mouseMove(brush, painter, lastPos,
                                                         event->pos());
            update(rect);
            modified = true;
        }
      //  lastPos = event->pos();
    }

}
//--------------------------------------------------------------------------------------------------
void PaintArea::mouseReleaseEvent(QMouseEvent *event)
{
        f = 5;
    if (event->button() == Qt::RightButton) {rubberBand->hide();}
    if (event->button() == Qt::LeftButton && scribbling && lastPos != QPoint(-1, -1))
    {

       scribbling = false;
    switch (mytoolsselectwin)
    {
/*  */              case 2 :  image2 = theImage;  break;
/*  */              case 3 :  image2 = theImage; theImage = image2; break;
/*  */              case 4 :  image2 = theImage;  break;
/* Кисть */         case 5 :  image2 = theImage; paintRubberORKist(1,event->pos()); break;
/*  */              case 6 :  paintRubberORKist(2,event->pos()); break;
/* Многоугольник */ case 7 :  image2 = theImage;
                            if (boolforpolygon)
                            {
                                boolforpoygon2 = true;
                                paintpolygon(event->pos());
                                i+=1;
                            }
                            break;
                   case 8 :  image2 = theImage; break;
                   case 9 :
                        image2 = theImage;
                        paintbezier(event->pos());
                        iforkrivline +=1;
                        if (iforkrivline == 3) iforkrivline = -1;
                        break;
/*  */              case 10:  paintAirBrush(event->pos()); break;
                    case 11 : paintRandomChar(event->pos()); break;
                    case 12 :  break;
                    case 13 :  break;
                    case 14 :
                    case 34 : image2 = theImage; break;
                    default :       break;
    }

image2 = theImage;
if (fab)
{
        while (f>=1)
        {
            undoredoimage[f]=undoredoimage[f-1];
            --f;
        }
    undoredoimage[0] = theImage;
    imagerotate = undoredoimage[3];
}

        if (brushInterface)
        {
            QPainter painter(&theImage);
            setupPainter(painter);
            QRect rect = brushInterface->mouseRelease(brush, painter,event->pos());
            update(rect);
            modified = true;
        }
    }
}







//-------------------------------------Инструменты--------------------------------------------------

//--------------------------------------------------------------------------------------------------
void PaintArea::painttext()
{
    //Рисуем текст
    QPainter painter(&theImage);

    painter.setFont(StyleText);
    bool ok;
painter.setRenderHint(QPainter::Antialiasing,sglali);  // вкл сглаживание


    painter.setPen(QPen(firstcolor, thickness, styleall, StyleLineEnd, StyleSoed));
        if (!rubberBand->isHidden())
        {
            if (POINTsinrubberband(lastPoint))
            {

                QString text = QInputDialog::getText(this, tr("Ввод текста"),
                                                     tr("Введите текст:"), QLineEdit::Normal,
                                                     QDir::home().dirName(), &ok);
                if (ok && !text.isEmpty())
                {
                    painter.drawText(lastPoint,text);
                    fab = true;
                        modified = true;
                }

            }
        }
        else
        {
            QString text = QInputDialog::getText(this, tr("Ввод текста"),
                                                 tr("Введите текст:"), QLineEdit::Normal,
                                                 QDir::home().dirName(), &ok);
        if (ok && !text.isEmpty())
            {
            painter.drawText(lastPoint,text);
            fab = true;
                modified = true;
            }
        }


    update();   //обновляем холст



}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintoval(const QPoint &endPoint)
{

    //Рисуем овал
    QPainter painter(&theImage);
    painter.setRenderHint(QPainter::Antialiasing,sglali);  // вкл сглаживание
    painter.setPen(QPen(firstcolor, thickness, styleall, StyleLineEnd, StyleSoed));
    painter.setBrush(inpaint);
    if (!rubberBand->isHidden())
    {
            if (POINTsinrubberband(lastPoint))
        {
            painter.drawEllipse(lastPoint,((endPoint.x()-lastPoint.x())),((endPoint.y()-lastPoint.y())));
            modified = true;
        }
    }
    else
    {
        painter.drawEllipse(lastPoint,((endPoint.x()-lastPoint.x())),((endPoint.y()-lastPoint.y())));

        modified = true;
    }
    update();   //обновляем холст
}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintrectangle(const QPoint &endPoint)
{
    //Рисуем четырехугольник
    QPainter painter(&theImage);

    painter.setRenderHint(QPainter::Antialiasing,sglali);  // вкл сглаживание
    QPoint pt(lastPoint);
    QSize  size((endPoint.x()-lastPoint.x()),(endPoint.y()-lastPoint.y()));
    QRect r(pt,size);
    painter.setPen(QPen(firstcolor, thickness, styleall, StyleLineEnd,
                        StyleSoed));
    painter.setBrush(inpaint);
    if (!rubberBand->isHidden())
    {
        if (POINTsinrubberband(lastPoint, endPoint))
        {
            painter.drawPoint(lastPoint);
            painter.drawRect(r);
            modified = true;
        }
    }
    else
    {
        painter.drawRect(r);
        modified = true;
    }

    update();   //обновляем холст

}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintpolygon(const QPoint &endPoint)
{
//рисуем многоугольник
    QPainter painter(&theImage);
    painter.setPen(QPen(firstcolor, thickness, styleall, StyleLineEnd,StyleSoed));
    painter.setBrush(inpaint);
    painter.setRenderHint(QPainter::Antialiasing,sglali);  // вкл сглаживание

    fab = true;
    boolforpolygon = true;

    if (!rubberBand->isHidden())
    {
        if (POINTsinrubberband(a[i-1], endPoint))
          {
              a[i] = endPoint;
              fab = false;
              painter.drawLine(a[i-1], a[i]);
              boolforpolygon = true;
              modified = true;
          }
        else
        {
            boolforpolygon = false;
        }
    }
    else
    {
        a[i] = endPoint;
        fab = false;
        painter.drawLine(a[i-1],endPoint);
        modified = true;
    }


bool bool50=false;
if (i==50)    bool50=true;

if (i>2)
{
    for (int k=0;k<i;k++)
        {
           if ((a[k]==a[i] or bool50) and boolforpoygon2)
            {
               boolnado = true;
                painter.drawPolygon(a,i+1);
                for (int j=0;j<i;j++)
                    a[j] = QPoint(0,0);
                i = -1;
                fab = true;
            }
        }
}
    update();   //обновляем холст
}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintRubberORKist(int p,const QPoint &endPoint) //Рисование ластиком и кистью или прямой
{
    QPainter painter(&theImage);
    firstcolor.setAlpha(prozracn); // прозрачность устанавливаем
    painter.setRenderHint(QPainter::Antialiasing,sglali);  // вкл сглаживание
    switch(p)
    {
    case 1:  painter.setPen(QPen(firstcolor, thickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));  break;
    case 2:  painter.setPen(QPen(backgroundcolor, thickness, Qt::SolidLine, Qt::RoundCap,Qt::RoundJoin)); break;
    case 3:  painter.setPen(QPen(firstcolor, thickness, styleall, StyleLineEnd, StyleSoed)); break;
    default: break;
    }

    if (!rubberBand->isHidden())
    {
        if (POINTsinrubberband(lastPoint, endPoint))
        {
            painter.drawLine(lastPoint, endPoint);
            modified = true;
        }
    }
    else
    {
        painter.drawLine(lastPoint, endPoint);
        modified = true;
    }
     if (p!=3)  lastPoint = endPoint;
   update();   //обновляем холст
}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintbezier(const QPoint &endPoint)
{
//Рисуем кривую линию
    QPainter painter(&theImage);
    if (iforkrivline<3)
    b[iforkrivline] = endPoint;

    painter.setRenderHint(QPainter::Antialiasing,sglali);  // вкл сглаживание
    painter.setPen(QPen(firstcolor, thickness, styleall, StyleLineEnd,StyleSoed));


    QPainterPath path;
    int  ok=0;
    
    switch (iforkrivline)
    {
        case 0 :

              // не 100% надобность такой же вещи у кривой линии(см полигон)
            if  (!POINTsinrubberband(b[3])) i=-1;
            if (!rubberBand->isHidden())
            {
                if (POINTsinrubberband(b[0], b[3]))
                {
                    fab = false;
                    painter.drawLine(b[0], b[3]);
                }
            }
            else
            {fab = false;
                painter.drawLine(b[0],b[3]);}


            break;
        case 1 :
            if (!rubberBand->isHidden())
             {
                 b[2]=b[1];
                    for (int f=-1;f<4;f++)
                     {
                                 if(POINTsinrubberband(b[f]))
                                 {
                                     ok = ok+1;
                                 }
                                 else
                                 {
                                     ok = ok-1;
                                 }
                     }
                     if (ok==3)
                     {
                         b[2]=b[1];
                         path.moveTo(b[0]);
                         path.cubicTo(b[1],b[2],b[3]);
                         painter.drawPath(path);
                         fab = false;
                         ok=0;
                     }
             }
             else
             {
                 b[2]=b[1];
                 path.moveTo(b[0]);
                 path.cubicTo(b[1],b[2],b[3]);
                 painter.drawPath(path);
                 fab = false;
             }
            break;
        case 2 :
            if (!rubberBand->isHidden())
             {
                    for (int f=-1;f<4;f++)
                     {
                             if(POINTsinrubberband(b[f]))
                                 {
                                     ok = ok+1;
                                 }
                                 else
                                 {
                                     ok = ok-1;
                                 }
                     }
                     if (ok==3)
                     {
                         path.moveTo(b[0]);
                         path.cubicTo(b[1],b[2],b[3]);
                         painter.drawPath(path);
                         fab = true;
                         ok=0;
                     }
             }
             else
             {
                 path.moveTo(b[0]);
                 path.cubicTo(b[1],b[2],b[3]);
                 painter.drawPath(path);
                 fab = true;
             }

            break;
        default :       break;
    }

        modified = true;
        update();   //обновляем холст



}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintAirBrush(const QPoint &endPoint)
{

//Рисуем воздушной кистью

    QPainter painter(&theImage);


        int numSteps = 2 + (lastPoint - endPoint).manhattanLength() / 2;
painter.setRenderHint(QPainter::Antialiasing,sglali);  // вкл сглаживание
        painter.setBrush(QBrush(firstcolor, Qt::Dense6Pattern));
        painter.setPen(Qt::NoPen);

        for (int i = 0; i < numSteps; ++i) {
            int x = endPoint.x() + i * (lastPoint.x() - endPoint.x()) / (numSteps - 1);
            int y = endPoint.y() + i * (lastPoint.y() - endPoint.y()) / (numSteps - 1);

            if (!rubberBand->isHidden())
            {
                if (POINTsinrubberband(lastPoint, endPoint))
                {
                    painter.drawEllipse(x - (thickness / 2), y - (thickness / 2),
                                        thickness, thickness);
                }
            }
            else
            {
                painter.drawEllipse(x - (thickness / 2), y - (thickness / 2),thickness, thickness);
                modified = true;
            }
        }
    lastPoint = endPoint;
    update();   //обновляем холст
}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintRandomChar(const QPoint &endPoint)
{

//Рисуем рандомными буквами
    QPainter painter(&theImage);

    painter.setPen(QPen(firstcolor, thickness, styleall, StyleLineEnd, StyleSoed));
    int rad = (thickness / 2) + 2;
    painter.setRenderHint(QPainter::Antialiasing,sglali);  // вкл сглаживание
    QRect boundingRect = QRect(endPoint, lastPoint).normalized()
                                              .adjusted(-rad, -rad, +rad, +rad);
    QChar ch('A' + (qrand() % 26));

    QFont biggerFont = painter.font();
    biggerFont.setBold(true);
    biggerFont.setPointSize(biggerFont.pointSize() + thickness);
    painter.setFont(biggerFont);

        if (!rubberBand->isHidden())
        {
            if (POINTsinrubberband(lastPoint))
            {
                 painter.drawText(lastPoint, QString(ch));
                 modified = true;
            }
        }
        else
        {
            painter.drawText(lastPoint, QString(ch));
            modified = true;
        }

    QFontMetrics metrics(painter.font());
    boundingRect = metrics.boundingRect(ch);
    boundingRect.translate(lastPoint);
    boundingRect.adjust(-10, -10, +10, +10);

    lastPoint = endPoint;
    update();   //обновляем холст
}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintgradientActs()
{
   QDialog dialog2; // объявляет диалог градентов и вызывает
   Ui::GradientDialog ui;
   ui.setupUi(&dialog2);
   dialog2.setWindowTitle(tr("Заливка градиентом - %1").arg(APP_NAME_AND_VER)); // устанавливаем в титл диалога с градиентами надпись "Заливка градиентов - %НАМЕПРОГАВЕРСИЯ%

   if (rubberBand)
   rubberBand->hide(); //если есть выделение то убираем нах

   QStringList  gradienttypelst;  // делаем строковый лист
   gradienttypelst << tr("Линейный") << tr("Радиальный") << tr("Конический");  // пункты листа
   ui.gradienttype->addItems(gradienttypelst);  // запихиваем лист в диалог

   if (dialog2.exec()) { // если вышли из диалога, то определеям какой пункт там, и заливаем
        type = ui.gradienttype->currentIndex();
        paintgradient(type,area);  // area вообще не юзается

   }
}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintgradient(int type, int area)
{
    QPainter painter(&theImage);
    area = 5;
//в зависимости от выбранного пункта градиентаз заливаем
switch (type)
    {
    case 0 :    {
                         QLinearGradient gradient(0, 0, width(), height());
                         gradient.setColorAt(0, firstcolor);
                         gradient.setColorAt(1,  backgroundcolor);
                         painter.setBrush(gradient);
                         break;
                }
     case 1 :   {
                        QPointF         ptCenter(rect().center());
                        QRadialGradient gradient(ptCenter, width() / 2, ptCenter);
                        gradient.setColorAt(0, firstcolor);
                        gradient.setColorAt(1, backgroundcolor);
                        painter.setBrush(gradient);
                        break;
                }
     case 2 :   {
                        QConicalGradient gradient(width() / 2, height() / 2, 0);
                        gradient.setColorAt(0, firstcolor);
                        gradient.setColorAt(0.5, backgroundcolor);
                        gradient.setColorAt(1, firstcolor);
                        painter.setBrush(gradient);
                        break;
                }
    default :       break;
    }
    painter.drawRect(rect());
    modified = true;
    update();   //обновляем холст


}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintFillClick(int x, int y)
{
  /*  while (boolforfillclick)
    {
        switch (theImage.pixel(lastPoint) or theImage.pixel(x,y))
            case theImage.pixel(lastPoint.x()+1,lastPoint.y()) :

            theImage.setPixel(lastPoint.x()+1,lastPoint.y(),theImage.pixel(lastPoint));
        boolforfillclick = true;
                paintFillClick(lastPoint.x()+1,lastPoint.y());


                break;

            case theImage.pixel(lastPoint.x(),lastPoint.y()+1) :

                theImage.setPixel(lastPoint.x(),lastPoint.y()+1,theImage.pixel(lastPoint));
         boolforfillclick = true;

                paintFillClick(lastPoint.x(),lastPoint.y()+1);


                break;

            case theImage.pixel(lastPoint.x()-1,lastPoint.y()) :
                theImage.setPixel(lastPoint.x()-1,lastPoint.y(),theImage.pixel(lastPoint));
                paintFillClick(lastPoint.x()-1,lastPoint.y());
                boolforfillclick = true;

                break;
            case theImage.pixel(lastPoint.x(),lastPoint.y()-1) :
                theImage.setPixel(lastPoint.x(),lastPoint.y()-1,theImage.pixel(lastPoint));

                boolforfillclick = true;
                paintFillClick(lastPoint.x(),lastPoint.y()-1);
                break;
    default : boolforfillclick = false;

    }*/

}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintFillClick()
{
    paintFillClick(lastPoint.x(),lastPoint.y());
}
//--------------------------------------------------------------------------------------------------


//----------------------Настройка инструментов

//--------------------------------------------------------------------------------------------------
void PaintArea::setBackColor(const QColor &col)
{
    backgroundcolor = col;  // принимаем фоновый цвет
}
//--------------------------------------------------------------------------------------------------
void PaintArea::setBrushWidth(int width)
{
    thickness = width; // принимаем толщину
}
//--------------------------------------------------------------------------------------------------
void PaintArea::setBrush(BrushInterface *brushInterface, const QString &brush)
{
    this->brushInterface = brushInterface;
    this->brush = brush;
}
//--------------------------------------------------------------------------------------------------
void PaintArea::setBrushColor(const QColor &col)
{
    firstcolor = col;  // принимаем основной цвет
}



//--------------------------------------------------------------------------------------------------
void PaintArea::timerfire2()
{

if (filterwidget->kilops)
    {
       filteracept();
       filterwidget->kilops = false;
    }

if (filterwidget->kilops2)
    {
        filterwidget->setPixmapAndResize(QPixmap::fromImage(theImage));
        filterwidget->kilops2 = false;
    }

   /* if (rubberBand->size().height()<=1)
        rubberBand->setHidden(true);  // если юзверь выделением тыкнул в холст,то считать, что его нет "не видно" - используется эта проверка при рисовании фигур
*/

}
//--------------------------------------------------------------------------------------------------
void PaintArea::setImage(const QImage &image)
{
    theImage = image.convertToFormat(QImage::Format_RGB32); // конвертируем полученное изображение в формат холста
    update();   //обновляем холст
    updateGeometry();
}
//--------------------------------------------------------------------------------------------------
void PaintArea::insertShape(const QPainterPath &path)
{
    pendingPath = path;
#ifndef QT_NO_CURSOR
    setCursor(Qt::CrossCursor);
#endif
}
//--------------------------------------------------------------------------------------------------
void PaintArea::setSloiBool1(const bool &bol)
{
    sloicurrbool1 = bol;  // принимаем булевскую переменную текущего слоя
}
//--------------------------------------------------------------------------------------------------
QSize PaintArea::sizeHint() const
{
    return theImage.size(); // возвращаем текущий размер хослта, нужен для статус бара
}
//--------------------------------------------------------------------------------------------------
void PaintArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
  //  painter.drawImage(QRect(0,0,width(),height()),theImage);
    painter.drawImage(QRect(0,0,theImage.size().width(),theImage.size().height()),theImage);

}
//--------------------------------------------------------------------------------------------------


//---------------------------       Повороты--------------------------------------------------------
void PaintArea::rotate90left()
{
    removeSelectArea();
    QImage imagerotate(theImage.size(), QImage::Format_RGB32);
    QPainter p(&imagerotate);
    QSize size = QSize(this->size().height(),this->size().width());

    QImage image;
    image = theImage;
    this->resizeImage(&image,size);
    theImage = image;
    imagerotate.fill(qRgb(255, 255, 255));

    p.translate(size.width()/2,size.height()/2);
    p.rotate(-90);
    p.translate(-size.width()/2,-size.height()/2);
    p.drawImage(0, 0, theImage);
    theImage = imagerotate;
    update();   //обновляем холст
    fabimage();
}
//--------------------------------------------------------------------------------------------------
void PaintArea::rotate90right()
{
    removeSelectArea();
    QImage imagerotate(theImage.size(), QImage::Format_RGB32);
    QPainter p(&imagerotate);
    QSize size = QSize(this->size().height(),this->size().width());

    imagerotate.fill(qRgb(255, 255, 255));
    p.translate(size.width()/2,size.height()/2);
    p.rotate(90);
    p.translate(-size.width()/2,-size.height()/2);
    p.drawImage(0, 0, theImage);
    theImage = imagerotate;
    update();   //обновляем холст
    fabimage();
}
//--------------------------------------------------------------------------------------------------
void PaintArea::rotate180()
{
    removeSelectArea();
    theImage = theImage.mirrored(false,true);
    update();   //обновляем холст
    fabimage();
}
//--------------------------------------------------------------------------------------------------


//-------------------------------- Масштаб---------------------------------------------------------


void PaintArea::scaleselect()
{
    bool ok;
    const int newprocent = QInputDialog::getInteger(this, APP_NAME,
                                                  tr("Введите процент масштабирования:"),
                                                  150 /*начальное значение*/,
                                                  1, 250, 1, &ok);
    if (ok)
    {
        removeSelectArea();
        int w,h;
        w = theImage.size().width()*newprocent/100; // баг
        h = theImage.size().height()*newprocent/100;
        QSize size(w,h);
        theImage = theImage.scaled(size,Qt::IgnoreAspectRatio);
        resize(size);
        update();   //обновляем холст
        fabimage();
    }
}
//--------------------------------------------------------------------------------------------------
void PaintArea::zoomIn()
{
    removeSelectArea();
    int w,h;
    w = theImage.size().width()+(theImage.size().width()*1/4);
    h = theImage.size().height()+(theImage.size().height()*1/4);
    QSize size(w,h);
    theImage = theImage.scaled(size,Qt::IgnoreAspectRatio);
    resize(size);
    update();   //обновляем холст
    fabimage();
}
//--------------------------------------------------------------------------------------------------
void PaintArea::zoomOut()
{
    removeSelectArea();
    int w,h;
    w = theImage.size().width()-(theImage.size().width()*1/4);
    h = theImage.size().height()-(theImage.size().height()*1/4);
    QSize size(w,h);
    theImage = theImage.scaled(size,Qt::IgnoreAspectRatio);
    resize(size);

    update();   //обновляем холст
    fabimage();
}
//--------------------------------------------------------------------------------------------------






//---------------------------- Действия с выделением------------------------------------------------
void PaintArea::deleteArea()
{
    QPainter painter(&theImage);
    painter.setPen(QPen(backgroundcolor));
    painter.setBrush(QBrush(backgroundcolor));

    if (!rubberBand->isHidden())
    {
        if (mytoolsselectwin==55)
        { // удаляем все нах, если выделено все
            painter.drawRect(0,0,theImage.size().width(),theImage.size().height());
        }
        else
        { // удаляем просто выделенную область
            QPoint pt(lastPoint);
            QSize  size(((rubberBand->pos().x()+rubberBand->size().width())-lastPoint.x()),( ( rubberBand->pos().y()+rubberBand->size().height())    -lastPoint.y()));
            QRect r(pt,size);
            painter.drawRect(r);
        }
        modified = true;
        rubberBand->hide();
    }
    else
    {   // удаляем скопированную область
        QRect ra(rubberBand->pos().x(),rubberBand->pos().y(),rubberBand->size().width(),rubberBand->size().height());
        painter.drawRect(ra);
    }
    update();   //обновляем холст
}
//--------------------------------------------------------------------------------------------------
void PaintArea::copyArea()
{
    if (rubberBand)  // если юзверь вообще что-то выделелил
    {
        prretheimage = theImage.copy (rubberBand->pos().x(),rubberBand->pos().y(),rubberBand->size().width(),rubberBand->size().height()); // запиъиваем в спомогательный куимейдж выделенную област
        rubberBand->hide(); // скрываем выделение (ЭФФЕКТ)
    }
}
//--------------------------------------------------------------------------------------------------
void PaintArea::pasteArea() // процедура для первого этапа вставки - помещение вспомогательного в начало координат
{
    if ((mytoolsselectwin == 34) or (mytoolsselectwin != 34))  // при =! запускает первый раз, а =34, если несколько раз Ctrl+v жмакаешь
    {
        imageforpastearea = theImage;  // сохраняем первоначальное изображение
        QPainter painter(&theImage);
        painter.drawImage(0,0,prretheimage); // рисуем из 0.0 сохраненное копированием изображение
        mytoolsselectwin=34; // включаем инструмент движения копированной области
        rubberBand->setGeometry(0,0,prretheimage.size().width(),prretheimage.size().height()); // помешаем нарисованное в выделение - ЭФФЕКТ (УЖЕ НЕСЕТ практическую ценность)
        rubberBand->show(); // показываем выделение
    }
    update();   //обновляем холст
}
//--------------------------------------------------------------------------------------------------
void PaintArea::pasteArea(const QPoint &endPoint) // собственно для перемешения по холсту
{

    if (!rubberBand->isHidden()) /*проверяем выделение есть(см начало моус эвентов
          применяется для определения, какую кнопку юзверь нажал, если левую, то все норм,
       иначе перемещение заканчивается       */
    {
        theImage=imageforpastearea; // возвращаем первоначальный холст

        QPoint mypoint((endPoint.x()-pointforpastearea.x()),(endPoint.y()-pointforpastearea.y()));
 /*
 "свистелка-перделка" - если при первок клике по выделению он выбрал относительно выделения координату 0;0
 то будет передвигатся по меньшей мере странно
 такой подход реализует возможность, что юзверь тыкнул в произвольно место выделения и будет перемещатся
 более красиво
 */

        QPainter pa(&theImage);
        pa.drawImage(mypoint,prretheimage); // рисуем в текущее положение мыши копию
        rubberBand->setGeometry(mypoint.x(),mypoint.y(),prretheimage.size().width(),prretheimage.size().height()); // помешаем нарисованное в выделение - ЭФФЕКТ (УЖЕ НЕСЕТ практическую ценность)
    }
    update();   //обновляем холст
}
//--------------------------------------------------------------------------------------------------
void PaintArea::cutArea()
{
    copyArea(); // копируем выделение
    deleteArea(); // удаляем выделение

}
//--------------------------------------------------------------------------------------------------
void PaintArea::selectAllArea()
{
    mytoolsselectwin=55;  // что бы можно было удалять
    viewpoint.setX(0);
    viewpoint.setY(0);
    if (rubberBand) // если до этого уже было выделение - убираем
       rubberBand->hide();

     rubberBand = new QRubberBand(QRubberBand::Rectangle, this); // создаем выделение по новой
     rubberBand->setGeometry(QRect(0,0,theImage.size().width(),theImage.size().height()));  //устанавливаем размер максимальный
     rubberBand->show();
}
//--------------------------------------------------------------------------------------------------
void PaintArea::removeSelectArea()
{
   // theImage = image3;
    rubberBand->hide(); // скрываем выделение
    image3 = theImage;
    image2 = theImage;
    i=0;
    iforkrivline = -1;
}
//--------------------------------------------------------------------------------------------------





//--------------------------------------------------------------------------------------------------
void PaintArea::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *printDialog = new QPrintDialog(&printer, this);

    if (printDialog->exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = theImage.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(theImage.rect());
        painter.drawImage(0, 0, theImage);
    }
#endif // QT_NO_PRINTER
}
//--------------------------------------------------------------------------------------------------
void PaintArea::fillImage()
{


   if (!rubberBand) mytoolsselectwin=55;
QPainter painter(&theImage);

    QColor color = QColorDialog::getColor(Qt::green, this);
         if (color.isValid()) {
        //     theImage.fill(color.rgb());

             painter.setPen(QPen(color));
             painter.setBrush(QBrush(color));


              if (!rubberBand->isHidden())
               {
                  QPoint pt(lastPoint);
                  QSize  size((viewpoint.x()-lastPoint.x()),(viewpoint.y()-lastPoint.y()));
                  QRect r(pt,size);
                  painter.drawRect(r);
                   modified = true;
               }
              else
              {
                  theImage.fill(color.rgb());
              }
               update();   //обновляем холст

         }

}
//--------------------------------------------------------------------------------------------------
void PaintArea::clearImage()
{
    /*if (sloicurrbool1)
    {
        sloi1.fill(qRgb(255, 255, 255));
       // this->clearImage();
    }
    else
    {
        sloi2.fill(qRgb(255, 255, 255));
    }*/

    theImage.fill(qRgb(255, 255, 255));  // заливаем холст белым
    rubberBand->hide(); // убираем выделение
    modified = false; // устанавливаем переменную "изменилось ли что-то на холсте" на фолс
    update();   //обновляем холст // обновляем холст

}
//--------------------------------------------------------------------------------------------------
bool PaintArea::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;

    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    theImage = loadedImage;
    modified = false;
    update();   //обновляем холст
    return true;
}
//--------------------------------------------------------------------------------------------------
bool PaintArea::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = theImage;
    resizeImage(&visibleImage, size());

    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;        
        return true;
    } else {
        return false;
    }
}
//--------------------------------------------------------------------------------------------------
void PaintArea::resizeImage(QImage *theImage, const QSize &newSize)
{


    if (theImage->size() == newSize)
        return;

    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *theImage);
    *theImage = newImage;
}



//--------------------------------------------------------------------------------------------------
void PaintArea::setupPainter(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(firstcolor, thickness, styleall, StyleLineEnd,
                   StyleSoed));
}
//--------------------------------------------------------------------------------------------------
QColor PaintArea::colorBack() const
{
    return backgroundcolor;  //возвращаем текущий фоновый цвет
}
//--------------------------------------------------------------------------------------------------
void PaintArea::filterslots()
{
    filterwidget->setPixmapAndResize(QPixmap::fromImage(theImage)); //делаем превью в доп фильтры
    filterwidget->show(); // показываем виджет доп фильтров
}
//--------------------------------------------------------------------------------------------------
void PaintArea::filteracept()
{
   // setImage(filterwidget->imgToFilter);
    setImage(filterwidget->mypixmap.toImage());

//filterwidget->kilops = false;

   /* void PaintArea::setImage(const QImage &image)
    {
        theImage = image.convertToFormat(QImage::Format_RGB32);
        update();   //обновляем холст
        updateGeometry();
    }*/

}
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
void PaintArea::undoimage()
{
    fa = fa+1;
    theImage = undoredoimage[fa];
    update();
}
//--------------------------------------------------------------------------------------------------
void PaintArea::redoimage()
{
    fa = fa-1;
    theImage = undoredoimage[fa];
    update();
}
//--------------------------------------------------------------------------------------------------
void PaintArea::fabimage()
{
    f = 5;
    fab = true;
    if (fab)
    {
            while (f>=1)
            {
                undoredoimage[f]=undoredoimage[f-1];
                --f;
            }
        undoredoimage[0] = theImage;
        imagerotate = undoredoimage[3];
    }
}

//--------------------------------------------------------------------------------------------------
bool PaintArea::POINTsinrubberband(QPoint a, QPoint b)
{
    if
    (
    (a.x()>rubberBand->pos().x()) &&
    (a.y()>rubberBand->pos().y()) &&
    (a.x()<(rubberBand->pos().x()+rubberBand->size().width())) &&
    (a.y()<(rubberBand->pos().y()+rubberBand->size().height())) &&
    (b.x()>rubberBand->pos().x()) &&
    (b.y()>rubberBand->pos().y()) &&
    (b.x()<(rubberBand->pos().x()+rubberBand->size().width())) &&
    (b.y()<(rubberBand->pos().y()+rubberBand->size().height()))
    ) return true;
    return false;
}
//--------------------------------------------------------------------------------------------------
bool PaintArea::POINTsinrubberband(QPoint a)
{
    if
    (
    (a.x()>rubberBand->pos().x()) &&
    (a.y()>rubberBand->pos().y()) &&
    (a.x()<(rubberBand->pos().x()+rubberBand->size().width())) &&
    (a.y()<(rubberBand->pos().y()+rubberBand->size().height()))
    ) return true;
    return false;
}
//--------------------------------------------------------------------------------------------------
