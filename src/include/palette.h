

#ifndef Palette_H
#define Palette_H

#include <QDialog>
#include <QGridLayout>


#include "qtcolortriangle.h"
#include "colorviewer.h"
#include "screen.h"



QT_BEGIN_NAMESPACE
class QAction;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QMenu;
class QMenuBar;
class QPushButton;
class QTextEdit;
QT_END_NAMESPACE
class PaintArea;


//! [0]
class Palette : public QWidget
{
    Q_OBJECT

public:
    Palette(QWidget *parent = 0);
    ~Palette();
    PaintArea *paintArea;

    QtColorTriangle *colorTriangle;
    ColorViewer *colorView;
    ColorViewer *colorView2;
    QPushButton *pickColor;



Q_SIGNALS:
    void colorChanged(const QColor &col);

public Q_SLOTS:
    void setColor(const QColor &col);

private slots:
   // void pickMode();
    void colorChgd();
    void timerfire();

public slots:
    void pickMode();

private:
    QTimer *timer;


    QGridLayout *layout;

    screen *ecran;
 /*   void createMenu();
    void createHorizontalGroupBox();
    void createverticalGroupBox();
    void createGridGroupBox();
    void createFormGroupBox();*/

    enum { NumGridRows = 3, NumButtons = 4 };


 //   WidgetColorPicker *colorw;


    QMenuBar *menuBar;
    QGroupBox *horizontalGroupBox;
    QGroupBox *verticalGroupBox;
    QGroupBox *gridGroupBox;
    QGroupBox *formGroupBox;
    QTextEdit *smallEditor;
    QTextEdit *bigEditor;
    QLabel *labels[NumGridRows];
    QLineEdit *lineEdits[NumGridRows];
    QPushButton *buttons[NumButtons];
    QDialogButtonBox *buttonBox;

    QMenu *fileMenu;
    QAction *exitAction;
};
//! [0]

#endif









































