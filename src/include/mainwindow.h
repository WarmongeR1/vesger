

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QStringList>
#include "QPushButton"



#include "qtcolortriangle.h"
#include "colorviewer.h"
#include "screen.h"

#include "QGridLayout"
QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QMenu;
class QScrollArea;
class QTextEdit;
class QCheckBox;
class QLabel;
class QErrorMessage;
class QScrollBar;
class QToolBar;
class QUndoStack;
class QUndoView;
QT_END_NAMESPACE

class PaintArea;
class Palette;
class QtColorTriangle;
class ImageToolWidget;

#include <QtGui>

/*namespace Ui {
    class MainWindow;
}*/

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    int toolsselectwin;
    int w,h;
    bool saveFile2(const QByteArray &fileFormat);
    bool maybeSave();
  //  bool sloibool1();



private slots:
    void newimage(); //создает новый файл
    void newfile();  // создает новое окно проги
    void open(); //открывает файл
    void brushColor(); // дает возможность выбора цвета
    void brushWidth(); // дает возможность выбора тощины
    void changeBrush();
    void insertShape();
    void applyFilter();
    void about(); // о проге
    void aboutPlugins(); // о плагинах
    void resizeselectwind();
    void openRecentFile(); // открывает "последние файлы"
    void save(); // севйвит
    void saveAs(); // сейвит как
    void paintpolygonActs();
    void einwindow();  // вкл/выкл панель инструментов
    void zweiwindow(); // вкл/выкл панель цветов
    void vierwindow();
   // void fitToWindow(); // "совместить" - не нужен - не забыть убрать
    void timerfire();
    void invert();
    void SelectInstAct();

    void linemod();
    void Tickmode();
    void lineendmod();
    void jointypemod();
    void Textatcall();

    void innerpainton();
    void innerpaintoff();
    void innerpaintonly();
    void antialiasmod();

    void setValueprozr(int);

public slots:
   // void itemMoved(DiagramItem *movedDiagram, const QPointF &moveStartPosition);

protected:

    virtual void resizeEvent(QResizeEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private:

    void addOp(QPainter::CompositionMode mode, const QString &name);
    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    void createActions();
    void createMenus();
    void createToolBars();
    void createHelpNav();
    void createWidgetForDockWidget();
    void createWidgetForAttribWidget();


    void loadPlugins();
    void populateMenus(QObject *plugin);
    void addToMenu(QObject *plugin, const QStringList &texts, QMenu *menu,const char *member, QActionGroup *actionGroup = 0);

    bool *textbool;

    void zoomperedach();

    QTimer *timer;

    QDockWidget *dockWidget;
    QDockWidget *dockColorWidget;
    QDockWidget *dockSloiWidget;
    QDockWidget *dockAttributes;


    QGridLayout *layout;
    QGridLayout *attriblayout;
    QWidget *widgetinstrument;
    QWidget *widgetsloi;
    QWidget *widgetAttrib;

    PaintArea *paintArea;
    QScrollArea *scrollArea;
    Palette *widgetpalette;
    ImageToolWidget *imgtw;

    QtColorTriangle *colorTriangle;
    ColorViewer *colorView;
    ColorViewer *colorView2;

    QScrollBar *qvertscroll;
    QScrollBar *qgorizscroll;

    QDir pluginsDir;
    QStringList pluginFileNames;
    QString strippedName(const QString &fullFileName);

    QMenu *fileMenu;
    QMenu *brushMenu;
    QMenu *shapesMenu;
    QMenu *filterMenu;
    QMenu *helpMenu;
    QMenu *windowMenu;

    QMenu *recentFilesMenu;
    QMenu *edit;
    QMenu *file;
    QMenu *help;

    QMenu *allocation;
    QMenu *type;
    QMenu *imagemenu;
    QMenu *tools;
    QMenu *configure;

    QToolBar *fileToolBar;
    QToolBar *editToolBar;

    QActionGroup *brushActionGroup;
    QAction *openAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *brushWidthAct;
    QAction *brushColorAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *aboutPluginsAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *removeAct;
    QAction *exitallAct;
    QAction *newwinAct;
    QAction *newAct;
    QAction *saveAct;
    QAction *printAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *zoomSelectAct;
    QAction *normalSizeAct;
    QAction *separatorAct;
    QAction *rotate90leftAct;
    QAction *rotate90rightAct;
    QAction *rotate180Act;
    QAction *resizeAct;
    QAction *resizecanvasAct;
    QAction *clearScreanAct;
    QAction *shortcutsAct;
    QAction *fillImageAct;
    QAction *einwindowAct;
    QAction *zweiwindowAct;
 //   QAction *dreiwindowAct;
    QAction *vierwindowAct;

    QAction *aallAct;
    QAction *aremoveAct;
    QAction *ainsertAct;    

    QAction *dopfilterAct; // доп фильтры

// экшены для инструментов

    QAction *paintgradientAct;
    QAction *paintpolygonAct;
    QAction *paintairBrushAct;
    QAction *paintRandomCharAct;

// кнопки для инструментов
    QPushButton *dockbutton1;
    QPushButton *dockbuttonrectangle;
    QPushButton *dockbuttonOVAL;
    QPushButton *dockbuttonSELECT;
    QPushButton *dockbuttonBezier;
    QPushButton *dockbuttonTEXT;
    QPushButton *dockbuttonLINE;
    QPushButton *dockbuttonRUUBER;
    QPushButton *dockbuttonPOLYGON;
    QPushButton *dockbuttonPIPETTE;
    QPushButton *dockbuttonFillclick;

    QToolBar *toolbarmy;


    QComboBox *TypeLine;
    QComboBox *LineEnd;
    QComboBox *JoinType;
    QPushButton *PaintInFigur;
    QPushButton *NormalFigur;
    QPushButton *OnlyPaint;
    QPushButton *Antialias;
    QPushButton *TextAttrib;
    QSpinBox *Thicknessenter;
    QLabel *TLlabel;
    QLabel *LELabel;
    QLabel *TELabel;    
    QLabel *JTLabel;
    QLabel *KTLabel;

// для статус бара
    QLabel *sb1;
    QLabel *sb2;
    QLabel *sb3;
    QLabel *sb4;
    QLabel *sb5;

    QString curFile;
    QString fileName2;
    QString fileName;
    QList<QAction *> saveAsActs;

/*

    QCheckBox* pchkfirst;
    QCheckBox* pchktwo;
    QRadioButton* pradfirst;
    QRadioButton* pradtwo;
    QLabel *sloinamefirst;
    QLabel *sloinametwo;
    QPushButton*  sloiok;

*/


    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
    bool askOnClose();
    bool ipipip;
    bool ik;  // для панели атрибутов



    QComboBox *operatorComboBox;

};

#endif
