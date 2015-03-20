

#include "QLayout"

#include "palette.h"
//#include "WidgetColorPicker.h"
#include "paintarea.h"

#include <QColorDialog>
#include <QTimer>

Palette::Palette(QWidget *parent)
    : QWidget(parent)
{


    ecran=0;
    colorTriangle = new QtColorTriangle(this);
    paintArea = new PaintArea;

    connect(colorTriangle, SIGNAL(colorChanged(QColor)), this, SLOT(colorChgd()));

    colorView = new ColorViewer(this);
    colorView->setColor(colorTriangle->color());

    colorView2 = new ColorViewer(this);
    colorView2->setColor(colorTriangle->colorBack());

    paintArea->setBrushColor(colorTriangle->color());
    connect(colorTriangle, SIGNAL(colorChanged(QColor)), colorView, SLOT(changeColor(QColor)));
    connect(colorTriangle, SIGNAL(colorChangedBack(QColor)), colorView2, SLOT(changeColor(QColor)));
    layout = new QGridLayout;

    layout->addWidget(  colorTriangle,  0, 0);
    layout->addWidget(  colorView,      1, 0);
    layout->addWidget(  colorView2,     2, 0);



    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(250);
    layout->addLayout(mainLayout , 5, 0);

    setLayout(layout);


    timer=new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerfire()));
    timer->start(2);
}



void Palette::timerfire()
{
//я настолько ленив, что мне лень сделать по другому =)
// Работает - трогай
    colorView2->setColor(colorTriangle->colorBack());

}


Palette::~Palette()
{
}

void Palette::pickMode()
{
    if(ecran!=0)
        delete ecran;
    ecran = new screen;
        connect( ecran, SIGNAL(colorSelected(QColor)), colorTriangle, SLOT(setColor(QColor)) );
        connect( ecran, SIGNAL(colorSelectedBack(QColor)), colorTriangle, SLOT(setColorBack(QColor)) );
}

void Palette::setColor(const QColor &col)
{
    colorView->setColor(col);
    colorTriangle->setColor(col);
}


void Palette::colorChgd()
{
    emit colorChanged(colorView->color());

}
