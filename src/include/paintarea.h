#ifndef PAINTAREA_H
#define PAINTAREA_H

#include "qtcolortriangle.h"
#include "colorviewer.h"
#include "screen.h"


#include <QColor>
#include <QImage>
#include <QPainterPath>
#include <QWidget>
#include <QRubberBand>
#include <QMimeData>
#include <QWMatrix>

class BrushInterface;
class ColorViewer;
class ImageToolWidget;

class PaintArea : public QWidget
{
    Q_OBJECT

public:
    PaintArea(QWidget *parent = 0);
        QImage theImage;
    bool modified;
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    bool isModified() const { return modified; }    

    Qt::PenStyle styleall; // стиль кисти
    int thickness;   // толщина
    Qt::PenCapStyle StyleLineEnd;
    Qt::PenJoinStyle StyleSoed;
    QFont StyleText;

    int brushWidth() const { return thickness; }
    int scaleprcent() const { return procent; }
    int penWidth() const { return myPenWidth; }    
    int fa; // вернуть отменить
    int mytoolsselectwin;

    bool fab;
    bool sglali;

    QImage image() const { return theImage; }
    QSize sizeHint() const;
    QColor penColor() const { return myPenColor; }
    QColor brushColor() const { return firstcolor; }
    QBrush inpaint;
    int prozracn;


    QPixmap pixm;
    QColor myPenColor;
    QColor colorBack() const;

    QColor firstcolor;  //основной цвет
    QColor backgroundcolor; // фоновый цвет

        QPoint lastPoint;



protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);



    float   zoom_value;
    QImage image2;
    QImage image3;
    QImage imageforpastearea;

    QImage imagerotate;


public slots:
    void setImage(const QImage &theImage);
    void insertShape(const QPainterPath &path);
    void setBrushColor(const QColor &col);
    void setBackColor(const QColor &col);
    void setBrushWidth(int width);
    void setBrush(BrushInterface *brushInterface, const QString &brush);
    void setSloiBool1(const bool &bol);

    void clearImage();

    void filteracept();
    void timerfire2();

    void resizeImage(QImage *theImage, const QSize &newSize);

private slots:

    void painttext();
    void paintoval(const QPoint &endPoint);
    void paintrectangle(const QPoint &endPoint);
    void paintpolygon(const QPoint &endPoint);
    void paintAirBrush(const QPoint &endPoint); // рисование водузной кистью
    void paintRandomChar(const QPoint &endPoint); // рисование рандомными буквами
    void paintRubberORKist(int p,const QPoint &endPoint); // рисование кистью и ластиком прямая линия(при p= 1/2/3)
    void paintbezier(const QPoint &endPoint);

    void paintFillClick();
    void paintFillClick(int x, int y);



    void zoomIn();
    void zoomOut();

    void rotate90left();
    void rotate90right();
    void rotate180();

    void fillImage();
    void print();
    void scaleselect();

    void deleteArea();
    void copyArea();

    void pasteArea();
    void pasteArea(const QPoint &endPoint);



    void cutArea();
    void selectAllArea();
    void removeSelectArea();

    void filterslots();


    void paintgradientActs();
    void paintgradient(int type, int area);


    void undoimage();
    void redoimage();
    void fabimage();


signals:
    void zoomFactorChanged( float );
    void changed(const QMimeData *mimeData = 0);






private:
    QtColorTriangle *colorTriangle;  // как бы палитра
    ColorViewer *colorView;
    ImageToolWidget *filterwidget;
     BrushInterface *brushInterface;

    QTimer *timer2;
    QTimer *timer;

    int i; // счетчик, применяется например для многоульника
    int iforkrivline; // счетчик для кривой линии
    int f; // вернуть отменить

    QImage prretheimage;

    void setupPainter(QPainter &painter);

    bool askOnClose();    
    bool toolButton_line;
    bool scribbling;
    bool sloicurrbool1;

    bool boolforfillclick;

    bool boolforpastearea;

    bool boolnado;

    QPixmap pixmap;

    qreal rotatint;

    float zoomFactor;


    QPoint a[50];  // массив точек для многоугольника
    QPoint b[4];  // массив точек для кривой линии


    QPoint viewpoint;
    QPoint lastPos;
    QPoint pointforpastearea;



    QImage undoredoimage[6]; // массив для вернуть/отменить
    int imageint;

    QRubberBand *rubberBand;  // выделение

    int myint;
    int type;
    int area;
    int myPenWidth;

    int procent;

 //   int thickness;
    QMatrix m;

    QString brush;

    QPainterPath pendingPath;
    QFont mytextset;

    QLine *line;


    QGradient *gradient;
    QLinearGradient *linearGrad;

    QPainter::CompositionMode currentMode() const;
    QPoint imagePos(const QImage &image) const;

    QPainter::CompositionMode mode;




    bool POINTsinrubberband(QPoint a, QPoint b);
    bool POINTsinrubberband(QPoint a);

bool boolforpolygon;
bool boolforpoygon2;






};

#endif
