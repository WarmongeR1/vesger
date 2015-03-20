#include "global.h"
#include "aboutdialog.h"
#include "interfaces.h"
#include "mainwindow.h"
#include "paintarea.h"
#include "plugindialog.h"
#include "palette.h"




#include "colorviewer.h"
#include "qtcolortriangle.h"

#include "imagetoolwidget.h"



#include "ui_resizedialog.h"


#include <QtGui>


#include <QSettings>
#include <QScrollArea>
#include <QTextCodec>
#include <QMessageBox>
#include <QFileDialog>
#include <QColorDialog>
#include <QTimer>
#include <QToolBar>
#include <QInputDialog>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>
#include <QStatusBar>
#include <QPluginLoader>
#include <QStringList>
#include <QListWidget>
#include <QScrollBar>


MainWindow::MainWindow() :
    paintArea(new PaintArea),
    scrollArea(new QScrollArea),
    widgetpalette(new Palette),
    imgtw(new ImageToolWidget),
    qvertscroll(new QScrollBar),
    qgorizscroll(new QScrollBar)
{

    QTextCodec *codec = QTextCodec::codecForName("UTF8");  // назначаем кодировку для всех надписей
    QTextCodec::setCodecForTr(codec); //устанавливаем кодек

    scrollArea->setBackgroundRole(QPalette::Dark); // где нет изображения красим дарком
    scrollArea->setWidget(paintArea);  // добавляем в область скролингов хослст
    setCentralWidget(scrollArea);   // устанавливаем холст(зону скролингов) в центр

    createActions(); // действия менюшек
    createMenus(); // менюшка
    createToolBars(); // тулбар
    createHelpNav(); //пояснения
    createWidgetForDockWidget(); //панель инструментов
    createWidgetForAttribWidget();

    loadPlugins(); // грузим плагины


/* прикручиваем  панель цветов */
    dockColorWidget = new QDockWidget(tr(" "), this);
    dockColorWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea); // устанавливаем куда можно двигать эту панель
    dockColorWidget->setWidget(widgetpalette);  // добавляем в панель виджет
    dockColorWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::RightDockWidgetArea, dockColorWidget);  //устанавливаем первоначальное положение панели

/* панель инструментов */
    dockWidget = new QDockWidget(tr(" "), this);
    dockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea); // устанавливаем куда можно двигать эту панель
    dockWidget->setWidget(widgetinstrument);  // добавляем в панель виджет
    dockWidget->setFixedWidth(101); // делаем фиксированный размер панели ВНИМАНИЕ: ЭТОТ РАЗМЕР ИСПОЛЬЗУЕТСЯ ДЛЯ ОПРЕДЕЛЕНИЯ ПОЛОЖЕНИЯ МЫШИ
    addDockWidget(Qt::LeftDockWidgetArea, dockWidget);  //устанавливаем первоначальное положение панели

    paintArea->setBrushColor(widgetpalette->colorTriangle->color());  // устанавливаем цвет, как и в палитре
    paintArea->setBackColor(widgetpalette->colorTriangle->colorBack()); // устанавливаем фоновый цввет, как и в палитре

    /*Прикручиваем панель атрибутов*/
    dockAttributes = new QDockWidget(tr("Атрибуты"), this);
    dockAttributes->setWidget(widgetAttrib);
    dockAttributes->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, dockAttributes);

    dockAttributes->setVisible(false);
    ik = false; // логическая для первого включения панели атрибутов


    /* Для определения положения мыши и установки цвета*/
    timer=new QTimer(this);  // создаем таймер
    connect(timer,SIGNAL(timeout()),this,SLOT(timerfire()));
    timer->start(2); //стартуем

    setWindowIcon(QPixmap(":/resources/images/icon.png")); // иконка окна
    setWindowTitle(APP_NAME_AND_VER); // устанваливает титл проги, как название проги
    this->showMaximized(); //делает окно развернутым

    imgtw->setPixmapAndResize(QPixmap::fromImage(paintArea->theImage)); //устанавливаем в доп фильтры текущий холст


    setCursor(Qt::CrossCursor);
    dockbutton1->setChecked(true);
    paintArea->mytoolsselectwin=5;
    sb5->setText(tr("<b>Инструмент:</b> Кисть"));
}



//--------------------------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event)
{

    //дабы не мучало мозг окошечко с "сохранить или послать тебя на"
  /*  if (paintArea->modified)
    {
    if (askOnClose()) {
        event->accept();
    } else {
        event->ignore();
        }
    }
    else
    {
    event->accept();
    }*/
}
//--------------------------------------------------------------------------------------------------
bool MainWindow::askOnClose()
{
    // проверяет условие, изменяли или нет хост (искусственная переменная) - см рисование фигур (paintarea.cpp)
    if (paintArea->modified)
    {
    int r = QMessageBox::question(
        this, tr("Подтвердите"),
        tr("Сохранить перед выходом?"),
        QMessageBox::Yes |
        QMessageBox::Cancel | QMessageBox::No);
    if (r == QMessageBox::Yes) {
      return saveFile2("png");
       } else if (r == QMessageBox::No) {
         return true;
     }
   }
         return false;


}
//--------------------------------------------------------------------------------------------------
void MainWindow::timerfire()
{
//я настолько ленив, что мне лень сделать по другому =)
// Работает - не трогай

    QPoint p=QCursor::pos();
    int w,h;
    w = p.x()-pos().x()-8;
    if (!dockWidget->isFloating())  // если виджет инструментов не отцеплен, то
        w = p.x()-pos().x()-109+(qgorizscroll->value()); // считаем положение мыши по горизонтали
        h = p.y()-pos().y()-94+(qvertscroll->value()); // считаем по вертикали
    sb4->setText(QString().sprintf("%d,%d",w,h) ); // устанавливаем в статус бар положение мыши
    sb3->setText(QString( "(%1, %2)" ).arg( paintArea->size().width()).arg( paintArea->size().height() ) ); //устанавливаем в статус бар размер холста

    //устанавливаем цвет, как и в палитре
    paintArea->setBrushColor(widgetpalette->colorView->color()); //основной цвет
    paintArea->setBackColor(widgetpalette->colorTriangle->colorBack());  //фоновый цвет

    widgetpalette->colorView2->setColor(paintArea->colorBack());  //устанавливаем цвет в палитре, как и текущий цвет (можно цвет поменять в палитре(через меню), и он в панели не поменяется без этой строчки


    // для титла
    if (paintArea->isModified())  // проверяет на изменчивость холста
    {
        setWindowTitle(tr("%1 [Изменен*] - %2").arg(strippedName(curFile)).arg(tr("%1").arg(APP_NAME_AND_VER)));
        //если изменен, то пишем строку аля "%NAMEFILE% [Изменен*] - %ВЕРСИЯПРОГИ%
    }
    else
    {
        if (!ipipip)
        setWindowTitle(tr("%1").arg(APP_NAME_AND_VER));
        //если нифига нет, то просто название проги и версию
    }


    if (!ik) // т.к. по другому бажит то так. Первый раз включает панель атрибутов
    {
        dockAttributes->setVisible(true);
        ik=true;
    }

    if (paintArea->mytoolsselectwin == 34) dockbuttonSELECT->setChecked(false);
    switch (paintArea->fa)
    {
             case 0 :  /*undoAct->setEnabled(false);*/ redoAct->setEnabled(false);  break;
             case 5:   undoAct->setEnabled(false);  break;

             default :    redoAct->setEnabled(true);  undoAct->setEnabled(true);  break;
    }

    // может породить баги
    /* если цвет меняют, то заливка не меняется
       эти 3 строки это исправляют*/
    if (PaintInFigur->isChecked()) innerpainton();
    if (NormalFigur->isChecked()) innerpaintoff();
    if (OnlyPaint->isChecked()) innerpaintonly();

}
//--------------------------------------------------------------------------------------------------
void MainWindow::brushColor()
{
    const QColor newColor = QColorDialog::getColor(paintArea->brushColor()); //вызывает диалог выбора цвета
    if (newColor.isValid()) //если цвет был выбран
    {
        widgetpalette->colorTriangle->setColor(newColor);  // то заливаем в освной цвет
    }
}
//--------------------------------------------------------------------------------------------------
void MainWindow::brushWidth()
{
    bool ok;
    const int newWidth = QInputDialog::getInteger(this, APP_NAME_AND_VER,
                                                  tr("Введите толщину линии:"),
                                                  paintArea->brushWidth(),
                                                  1, 50, 1, &ok); //вызывает диалог ввода толщины
    if (ok)
        paintArea->setBrushWidth(newWidth); // если чел ввел, то отправляем толщину в холст
}
//--------------------------------------------------------------------------------------------------
void MainWindow::changeBrush()
{
    QAction *action = qobject_cast<QAction *>(sender());
    BrushInterface *iBrush = qobject_cast<BrushInterface *>(action->parent());
    const QString brush = action->text();

    paintArea->setBrush(iBrush, brush);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::insertShape()
{
    QAction *action = qobject_cast<QAction *>(sender());
    ShapeInterface *iShape = qobject_cast<ShapeInterface *>(action->parent());

    const QPainterPath path = iShape->generateShape(action->text(), this);
    if (!path.isEmpty())
        paintArea->insertShape(path);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::applyFilter()
{
    QAction *action = qobject_cast<QAction *>(sender());
    FilterInterface *iFilter =
            qobject_cast<FilterInterface *>(action->parent());

    const QImage image = iFilter->filterImage(action->text(), paintArea->image(),
                                              this);
    paintArea->setImage(image);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::about()
{
    CAboutForm *about = new CAboutForm;  // подцепляем "О программе" и показывает
    about->setupUi(about);  //подцепляем
    about->show(); //показываем
    about->activateWindow(); // делаем главным
}
//--------------------------------------------------------------------------------------------------
void MainWindow::aboutPlugins()
{
    PluginDialog dialog(pluginsDir.path(), pluginFileNames, this);
    dialog.exec();
}
//--------------------------------------------------------------------------------------------------
void MainWindow::resizeEvent(QResizeEvent *event)
{
   QSize sz = event->size();
   sb2->setText(
        QString( "(%1, %2)" ).arg( sz.width() ).arg( sz.height() ) );
   paintArea->update();
}

void MainWindow::addOp(QPainter::CompositionMode mode, const QString &name)
{
    operatorComboBox->addItem(name, mode);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::invert()
{
    paintArea->theImage.invertPixels(QImage::InvertRgb); //инвертируем пикселя
    paintArea->update(); // обновляем холст
}
//--------------------------------------------------------------------------------------------------
void MainWindow::resizeselectwind()
{

    // Окошко с вводом данных для изменения размера холста
   QDialog dialog;
   Ui::ResizeDialog ui;
   ui.setupUi(&dialog);

   ui.widthSpinBox->setValue(paintArea->size().width());
   ui.heightSpinBox->setValue(paintArea->size().height());

   dialog.setWindowTitle(tr("Изменение размера холста - %1").arg(APP_NAME_AND_VER));
   if (dialog.exec() and (ui.widthSpinBox->value()==ui.heightSpinBox->value() ))
   {
            QSize newSize(ui.widthSpinBox->value(),ui.heightSpinBox->value());
            paintArea->resize(newSize);
            QImage loadedImage;
            loadedImage.size() = newSize;
            paintArea->resizeImage(&loadedImage, newSize);
            QPainter p(&loadedImage);
            p.drawImage(0,0,paintArea->theImage);
            paintArea->theImage = loadedImage;
            paintArea->update();
   }
}
//--------------------------------------------------------------------------------------------------





//--------------------------               Меню           ---------------------------------------


void MainWindow::createToolBars()
{

    fileToolBar = addToolBar(tr("File"));  //создаем тулбар
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

 //  Вернуть, когда реализую
    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(removeAct);

}
//--------------------------------------------------------------------------------------------------
void MainWindow::createActions()
{


    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        recentFileActs[i]->setStatusTip(tr("Открыть одно из недавних изображений"));
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }


    paintRandomCharAct = new QAction(tr("&Случайные буквы"), this);
    paintRandomCharAct->setStatusTip(tr("Рисование случайными буквами"));
    connect(paintRandomCharAct, SIGNAL(triggered()), this, SLOT(SelectInstAct()));

    paintairBrushAct = new QAction(tr("&Воздушная кисть"), this);
    paintairBrushAct->setStatusTip(tr("Рисование воздушной кистью"));
    connect(paintairBrushAct, SIGNAL(triggered()), this, SLOT(SelectInstAct()));

    paintgradientAct = new QAction(tr("&Залить градиентом"), this);
    paintgradientAct->setStatusTip(tr("Залить весь хослт градиентом"));
    connect(paintgradientAct, SIGNAL(triggered()), paintArea, SLOT(paintgradientActs()));

    paintpolygonAct = new QAction(tr("&Многоугольник"), this);
    paintpolygonAct->setStatusTip(tr("Нарисовать многоугольник"));





 //  einwindowAct = new QAction(QIcon(":/resources/images/open.png"), tr("&Открыть..."),this);
    einwindowAct = new QAction(tr("&Инстументы"), this);
    einwindowAct->setShortcut(tr("Ctrl+Alt+1"));
    einwindowAct->setStatusTip(tr("Показать/скрыть панель с инструментами"));
    connect(einwindowAct, SIGNAL(triggered()), this, SLOT(einwindow()));

    zweiwindowAct = new QAction(tr("&Палитра"), this);
    zweiwindowAct->setShortcut(tr("Ctrl+Alt+2"));
    zweiwindowAct->setStatusTip(tr("Показать/скрыть панель с выбором цвета"));
    connect(zweiwindowAct, SIGNAL(triggered()), this, SLOT(zweiwindow()));



    vierwindowAct = new QAction(tr("&Атрибуты"), this);
    vierwindowAct->setShortcut(tr("Ctrl+Alt+3"));
    vierwindowAct->setStatusTip(tr("Показать/скрыть панель атрбитов"));
    connect(vierwindowAct, SIGNAL(triggered()), this, SLOT(vierwindow()));

    aallAct = new QAction(tr("&Выделить все"), this);
    aallAct->setStatusTip(tr("Выделить все"));
    aallAct->setShortcut(tr("Ctrl+A"));
    connect(aallAct, SIGNAL(triggered()), paintArea, SLOT(selectAllArea()));
    connect(aallAct, SIGNAL(triggered()), this, SLOT(SelectInstAct()));

    aremoveAct = new QAction(tr("&Снять выделение"), this);
    aremoveAct->setStatusTip(tr("Снять выделение"));
    connect(aremoveAct, SIGNAL(triggered()), paintArea, SLOT(removeSelectArea()));

    ainsertAct = new QAction(tr("&Инвертировать"), this);
    ainsertAct->setStatusTip(tr("Инвертироват изображение"));
    connect(ainsertAct, SIGNAL(triggered()), this, SLOT(invert()));

    dopfilterAct = new QAction(tr("&Дополнительные фильтры"), this);
    dopfilterAct->setStatusTip(tr("Еще несколько фильтров"));
    connect(dopfilterAct, SIGNAL(triggered()), paintArea, SLOT(filterslots()));





    rotate90leftAct = new QAction(tr("&Повернуть на 90 влево"), this);
    rotate90leftAct->setStatusTip(tr("Повернуть на 90 против часовой стрелкиу"));
    connect(rotate90leftAct, SIGNAL(triggered()), paintArea, SLOT(rotate90left()));

    rotate90rightAct = new QAction(tr("&Повернуть на 90 вправо"), this);
    rotate90rightAct->setStatusTip(tr("Повернуть на 90 по часовой стрелке"));
    connect(rotate90rightAct, SIGNAL(triggered()), paintArea, SLOT(rotate90right()));

    rotate180Act = new QAction(tr("&Повернуть на 180"), this);
    rotate180Act->setStatusTip(tr("Повернуть изображение на 180 градусов"));
    connect(rotate180Act, SIGNAL(triggered()),  paintArea, SLOT(rotate180()));



    resizeAct = new QAction(tr("&Изображение"), this);
    resizeAct->setStatusTip(tr("Изменить размер изображения"));
    connect(resizeAct, SIGNAL(triggered()), this, SLOT(close()));

    resizecanvasAct = new QAction(tr("&Изменить размер холста"), this);
    resizecanvasAct->setStatusTip(tr("Изменить размер холста"));
    connect(resizecanvasAct, SIGNAL(triggered()), this, SLOT(resizeselectwind()));

    clearScreanAct = new QAction(tr("&Очисить холст"), this);
    clearScreanAct->setStatusTip(tr("Очисить холст"));
    connect(clearScreanAct, SIGNAL(triggered()), paintArea, SLOT(clearImage()));



    shortcutsAct = new QAction(tr("&Выход"), this);
    shortcutsAct->setStatusTip(tr("Закрыть программу"));
    connect(shortcutsAct, SIGNAL(triggered()), this, SLOT(close()));


    separatorAct = new QAction(tr("&Увеличить"), this);
    separatorAct->setStatusTip(tr("Увеличить размер изображения"));
    connect(separatorAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("&Увеличить на 25%"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setStatusTip(tr("Масштаб изображения: Увеличить размер изображения"));
    connect(zoomInAct, SIGNAL(triggered()), paintArea, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("&Уменьшить на 25%"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setStatusTip(tr("Масштаб изображения:Уменьшить размер изобарежния"));
    connect(zoomOutAct, SIGNAL(triggered()), paintArea, SLOT(zoomOut()));

    zoomSelectAct = new QAction(tr("&Ввести процент масштаба"), this);
    zoomSelectAct->setShortcut(tr("Ctrl+M"));
    zoomSelectAct->setStatusTip(tr("Масштаб изображения:ввести процент масштабирования"));
    connect(zoomSelectAct, SIGNAL(triggered()), paintArea, SLOT(scaleselect()));

    normalSizeAct = new QAction(tr("&Оригинальный размер"), this);
    normalSizeAct->setStatusTip(tr("Показать оригинальный размер изображения"));
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(close()));

    printAct = new QAction(tr("&Печать "), this);
    printAct->setShortcuts(QKeySequence::Print);
    printAct->setStatusTip(tr("Напечатать изображение"));
    connect(printAct, SIGNAL(triggered()), paintArea, SLOT(print()));

    exitallAct = new QAction(tr("Закрыть &все"), this);
    exitallAct->setShortcut(tr("Ctrl+W"));
    exitallAct->setStatusTip(tr("Закрыть все окна программы"));
    connect(exitallAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    saveAct = new QAction(QIcon(":/resources/images/save.png"), tr("&Сохранить"),this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Сохранить изображение на компьютер"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    newAct = new QAction(QIcon(":/resources/images/new.png"), tr("&Новый"),this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Создать новое изображение"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newimage()));

    newwinAct = new QAction(tr("Новое &окно"), this);
    newwinAct->setStatusTip(tr("Открытие нового окна программы"));
    connect(newwinAct, SIGNAL(triggered()), this, SLOT(newfile()));

    undoAct = new QAction(tr("&Отменить"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Отменить действие"));
    connect(undoAct, SIGNAL(triggered()), paintArea, SLOT(undoimage()));

    redoAct = new QAction(tr("&Вернуть"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Вернуть, то что отменили"));
    connect(redoAct, SIGNAL(triggered()), paintArea, SLOT(redoimage()));

    cutAct = new QAction(QIcon(":/resources/images/cut.png"), tr("&Вырезать"), this);
    cutAct->setStatusTip(tr("Вырезать выделенную область"));
    cutAct->setShortcuts(QKeySequence::Cut);
    connect(cutAct, SIGNAL(triggered()), paintArea, SLOT(cutArea()));


    copyAct = new QAction(QIcon(":/resources/images/copy.png"), tr("&Копировать"),this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Скопировать выделенную область"));
    connect(copyAct, SIGNAL(triggered()), paintArea, SLOT(copyArea()/*copy()*/));

    pasteAct = new QAction(QIcon(":/resources/images/paste.png"), tr("&Вставить"),this);
    pasteAct->setShortcut(QKeySequence::Paste);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Вставить изображение из буфера обмена"));
    connect(pasteAct, SIGNAL(triggered()), paintArea, SLOT(pasteArea()));

    removeAct = new QAction(QIcon(":/resources/images/delete.png"), tr("&Удалить"),this);
    removeAct->setShortcut(tr("Ctrl+Del"));
    removeAct->setStatusTip(tr("Удалить выделенную область"));
    connect(removeAct, SIGNAL(triggered()), paintArea, SLOT(deleteArea()));

    openAct = new QAction(QIcon(":/resources/images/open.png"), tr("&Открыть..."),this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Открыть выбранное изображение "));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAsAct = new QAction(tr("&Сохранить как..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("В&ыход"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    brushColorAct = new QAction(tr("&Палитра"), this);
    brushColorAct->setStatusTip(tr("Выбрать цвет для всех инструментов"));
    connect(brushColorAct, SIGNAL(triggered()), this, SLOT(brushColor()));

    brushWidthAct = new QAction(tr("&Толщина"), this);
    brushWidthAct->setStatusTip(tr("Выбрать толщину для фигур"));
    connect(brushWidthAct, SIGNAL(triggered()), this, SLOT(brushWidth()));

    brushActionGroup = new QActionGroup(this);

    aboutAct = new QAction(tr("&О программе"), this);
    aboutAct->setStatusTip(tr("Краткая информация о программе и авторах"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("О &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    aboutPluginsAct = new QAction(tr("О плагинах"), this);
    connect(aboutPluginsAct, SIGNAL(triggered()), this, SLOT(aboutPlugins()));

    fillImageAct = new QAction(tr("&Заливка"), this);
    fillImageAct->setStatusTip(tr("Залить весь холст одним цветом **"));
    connect(fillImageAct, SIGNAL(triggered()), paintArea, SLOT(fillImage()));



}
//--------------------------------------------------------------------------------------------------
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Файл"));
        fileMenu->addAction(newAct);
        fileMenu->addAction(newwinAct);
        fileMenu->addAction(openAct);
        fileMenu->addAction(saveAct);
        fileMenu->addAction(saveAsAct);
        separatorAct = fileMenu->addSeparator();

        for (int i = 0; i < MaxRecentFiles; ++i)
            fileMenu->addAction(recentFileActs[i]);
        updateRecentFileActions();
        fileMenu->addSeparator();
        fileMenu->addAction(printAct);
        fileMenu->addSeparator();
        fileMenu->addAction(exitallAct);
        fileMenu->addSeparator();
        fileMenu->addAction(exitAct);


    edit = menuBar()->addMenu(tr("&Правка"));
        edit->addAction(undoAct);
        edit->addAction(redoAct);
        edit->addSeparator();
        edit->addAction(cutAct);
        edit->addAction(copyAct);
        edit->addAction(pasteAct);
        edit->addAction(removeAct);
        edit->addSeparator();
        edit->addAction(aallAct);
        edit->addAction(aremoveAct);


    menuBar()->addSeparator();

    type = menuBar()->addMenu(tr("&Изображение"));
        type->addAction(zoomInAct);
        type->addAction(zoomOutAct);
        type->addAction(zoomSelectAct);
        type->addSeparator();
        type->addAction(rotate90leftAct);
        type->addAction(rotate90rightAct);
        type->addAction(rotate180Act);
        type->addSeparator();
        type->addAction(resizecanvasAct);
        type->addSeparator();
        type->addAction(clearScreanAct);


    tools = menuBar()->addMenu(tr("&Инструменты"));
        tools->addAction(brushColorAct);
        tools->addAction(brushWidthAct);
        tools->addSeparator();
        tools->addAction(fillImageAct);
        tools->addAction(paintgradientAct);
        tools->addSeparator();
        tools->addAction(paintairBrushAct);
        tools->addAction(paintRandomCharAct);
        tools->addSeparator();

    windowMenu = menuBar()->addMenu(tr("Панели/окна"));
        windowMenu->addAction(einwindowAct);
        windowMenu->addAction(zweiwindowAct);
        windowMenu->addAction(vierwindowAct);

    filterMenu = menuBar()->addMenu(tr("&Фильтры"));
        filterMenu->addAction(dopfilterAct);
        filterMenu->addSeparator();

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Помощь"));
        helpMenu->addAction(aboutAct);
        helpMenu->addAction(aboutQtAct);
        helpMenu->addAction(aboutPluginsAct);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::createHelpNav()
{


    sb1 = new QLabel(statusBar());
    sb2 = new QLabel(statusBar());
    sb3 = new QLabel(statusBar());
    sb4 = new QLabel(statusBar());
    sb5 = new QLabel(statusBar());

    statusBar()->setSizeGripEnabled(false);
    statusBar()->addWidget(sb1, 2);
    statusBar()->addWidget(sb2, 1);
    statusBar()->addWidget(sb3, 1);
    statusBar()->addWidget(sb4, 1);
    statusBar()->addWidget(sb5, 1);




    sb2->setToolTip(tr("Размер окна"));
    sb3->setToolTip(tr("Размер холста"));
    sb4->setToolTip(tr("Положение курсора"));
    sb5->setToolTip(tr("Текущий инструмент"));


    sb1->setText(tr("Здравствуй. Я готов к работе, нарисуй что-нибудь."));
    sb5->setText(tr("<b>Инструмент:</b> Кисть"));


    sb2->setToolTip(tr("Размер окна"));
    sb3->setToolTip(tr("Размер холста"));
    sb4->setToolTip(tr("Положение курсора"));
    sb5->setToolTip(tr("Текущий инструмент"));

    /*painta1Act->setToolTip(tr("Написать текст"));
    painta2Act->setToolTip(tr("Нарисовать овал"));
    painta3Act->setToolTip(tr("Нарисовать прямую линию"));
    painta4Act->setToolTip(tr("Рисовать карандашом"));
    painta5Act->setToolTip(tr("Нарисовать прямоугольник"));*/
    openAct->setToolTip(tr("Открыть выбранное изображение "));
    aallAct->setToolTip(tr("Выделить все"));
    aremoveAct->setToolTip(tr("Снять выделение"));
    ainsertAct->setToolTip(tr("Инвертироват изображение"));
    rotate90leftAct->setToolTip(tr("Повернуть на 90 против часовой стрелкиу"));
    rotate90rightAct->setToolTip(tr("Повернуть на 90 по часовой стрелке"));
    rotate180Act->setToolTip(tr("Повернуть изображение на 180 градусов"));
    resizeAct->setToolTip(tr("Изменить размер изображения"));
    resizecanvasAct->setToolTip(tr("Изменить размер холста"));
    clearScreanAct->setToolTip(tr("Очисить холст"));
    shortcutsAct->setToolTip(tr("Закрыть программу"));
    zoomInAct->setShortcut(tr("Ctrl+G"));
    zoomOutAct->setShortcut(tr("Ctrl+H"));
    zoomSelectAct->setShortcut(tr("Ctrl+L"));
    normalSizeAct->setToolTip(tr("Показать оригинальный размер изображения"));
    //fitToWindowAct->setToolTip(tr("Размернуть изображение до размера холста"));
    printAct->setToolTip(tr("Напечатать изображение"));
    exitallAct->setToolTip(tr("Закрыть все окна программы"));
    saveAct->setToolTip(tr("Сохранить изображение на компьютер"));
    newAct->setToolTip(tr("Создать новое изображение"));
    newwinAct->setToolTip(tr("Открытие нового окна программы"));
    saveAsAct->setToolTip(tr("Сохранить изображение с вводом нового имени"));
    undoAct->setToolTip(tr("Отменить действие"));
    redoAct->setToolTip(tr("Вернуть, то что отменили"));
    cutAct->setToolTip(tr("Вырезать выделенную область"));
    copyAct->setToolTip(tr("Скопировать выделенную область"));
    pasteAct->setToolTip(tr("Вставить изображение из буфера обмена"));
    removeAct->setToolTip(tr("Удалить выделенную область"));
    saveAsAct->setToolTip(tr("Ввести название файла и сохранить"));
    exitAct->setToolTip(tr("Выход"));
    brushColorAct->setToolTip(tr("Выбрать цвет для всех инструментов"));
    brushWidthAct->setToolTip(tr("Выбрать толщину для фигур"));
    aboutAct->setToolTip(tr("Краткая информация о программе и авторах"));
    aboutQtAct->setToolTip(tr("Информация о том, на чем это сделано"));
    fillImageAct->setToolTip(tr("Залить весь холст одним цветом **"));



}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
void MainWindow::loadPlugins()
{
   foreach (QObject *plugin, QPluginLoader::staticInstances())
        populateMenus(plugin);


    pluginsDir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();
        if (plugin) {
            populateMenus(plugin);
            pluginFileNames += fileName;

        }

    }

  //  brushMenu->setEnabled(!brushActionGroup->actions().isEmpty());
  //  shapesMenu->setEnabled(!shapesMenu->actions().isEmpty());
   // tools->setEnabled(!tools->actions().isEmpty());
    tools->setEnabled(!tools->actions().isEmpty());
   filterMenu->setEnabled(!filterMenu->actions().isEmpty());
}
//--------------------------------------------------------------------------------------------------
void MainWindow::populateMenus(QObject *plugin)
{
 /*   BrushInterface *iBrush = qobject_cast<BrushInterface *>(plugin);
    if (iBrush)
        addToMenu(plugin, iBrush->brushes(), brushMenu, SLOT(changeBrush()),
                  brushActionGroup);

    ShapeInterface *iShape = qobject_cast<ShapeInterface *>(plugin);
    if (iShape)
        addToMenu(plugin, iShape->shapes(), shapesMenu, SLOT(insertShape()));*/

 /*   BrushInterface *iBrush = qobject_cast<BrushInterface *>(plugin);
        if (iBrush)
            addToMenu(plugin, iBrush->brushes(), tools, SLOT(changeBrush()),
                      brushActionGroup);*/

    ShapeInterface *iShape = qobject_cast<ShapeInterface *>(plugin);
    if (iShape)
        addToMenu(plugin, iShape->shapes(), tools, SLOT(insertShape()));

    FilterInterface *iFilter = qobject_cast<FilterInterface *>(plugin);
    if (iFilter)
        addToMenu(plugin, iFilter->filters(), filterMenu, SLOT(applyFilter()));

}
//--------------------------------------------------------------------------------------------------
void MainWindow::addToMenu(QObject *plugin, const QStringList &texts,
                           QMenu *menu, const char *member,
                           QActionGroup *actionGroup)
{
    foreach (QString text, texts) {
        QAction *action = new QAction(text, plugin);
        connect(action, SIGNAL(triggered()), this, member);
        menu->addAction(action);

        if (actionGroup) {
            action->setCheckable(true);
            actionGroup->addAction(action);
        }
    }
}




//--------------------------         Работа с файлами       ---------------------------------------


void MainWindow::newfile()
{
    MainWindow *other = new MainWindow;  // создем еще одно окно с прогой
    other->show(); // показываем
}
//--------------------------------------------------------------------------------------------------
void MainWindow::save()
{
    if (curFile.isEmpty())
          saveAs();
    else
       saveFile(curFile);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::saveAs()
{
   saveFile2("png");
}
//--------------------------------------------------------------------------------------------------
void MainWindow::openRecentFile()
{

    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
     //   PaintArea->clearImage();
        loadFile(action->data().toString());
        paintArea->openImage(action->data().toString());
    }
     paintArea->adjustSize();
}
//--------------------------------------------------------------------------------------------------
void MainWindow::loadFile(const QString &fileName)
{
  /*  QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, APP_NAME_AND_VER,
                             tr("Не могу прочитать %1:\nПричина:%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }*/



    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Изображение загружено"), 2000);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, APP_NAME_AND_VER,
                             tr("Не могу сохранить %1\nПричина:%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

paintArea->saveImage(fileName, "png");

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("Изображение загружено"), 2000);
}
//--------------------------------------------------------------------------------------------------
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    if (curFile.isEmpty())
        setWindowTitle(tr("%1").arg(APP_NAME_AND_VER));
    else
        setWindowTitle(tr("%1 - %2").arg(strippedName(curFile))
                                    .arg(tr("%1").arg(APP_NAME_AND_VER)));

    QSettings settings("WarmongeR", "Recent Files");
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}
//--------------------------------------------------------------------------------------------------
void MainWindow::updateRecentFileActions()
{
    QSettings settings("WarmongeR", "Recent Files");
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}
//--------------------------------------------------------------------------------------------------
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
//--------------------------------------------------------------------------------------------------
bool MainWindow::maybeSave()
{
    //функция проверки на изменение хослста, если что-то изменилось, то спрашиваем сейвить или нет
    if (paintArea->isModified()) {
       QMessageBox::StandardButton ret;
       ret = QMessageBox::warning(this, tr("Внимание!"),
                          tr("Изображение было изменено.\n"
                             "Вы хотите сохранить изменения?"),
                          QMessageBox::Save | QMessageBox::No
                          | QMessageBox::Cancel);
        if (ret == QMessageBox::Save) {
           // return saveFile("png");
        } else if (ret == QMessageBox::Cancel) {
            return false;
        }
    }
    return true;
}
//--------------------------------------------------------------------------------------------------
bool MainWindow::saveFile2(const QByteArray &fileFormat)
{

QString initialPath = QDir::currentPath() + tr("/введите название.") + fileFormat;

QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить как"),initialPath,
    tr("Изображение PNG (*.%1);;Изображение JPEG (*.jpg);;Изображение BMP (*.bmp);;Все файлы (*)")
    .arg(QString(fileFormat)));

    if (fileName.isEmpty()) {
        return false;
    } else {
        setCurrentFile(fileName);
        return paintArea->saveImage(fileName, fileFormat);
    }
}
//--------------------------------------------------------------------------------------------------
void MainWindow::open()
{
   /* const QString*/ fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Открыть изображение"),
                                                          QDir::currentPath(),
                                                           tr("Изображение PNG (*.png);;Изображение JPEG (*.jpg);;Изображение BMP (*.bmp);;Все файлы (*)"));  // добавляем возможные варианты для загрузки файла(png,jpg,bmp)

    if (!fileName.isEmpty()) {
                loadFile(fileName);
        if (!paintArea->openImage(fileName)) {
            QMessageBox::information(this, APP_NAME_AND_VER,
                                     tr("Не могу загрузить %1.").arg(fileName));
            return;
        }
        paintArea->adjustSize();
        setWindowTitle(tr("%1 - %2").arg(strippedName(fileName))
                                    .arg(tr("%1").arg(APP_NAME_AND_VER))); // устанавливаем в титл название файла и название проги с версией
        fileName2 = fileName;
            setCurrentFile(fileName);  // название файла заливаем в fileName
      //  paintArea->modified = true;
        ipipip = true;
    }

}
//--------------------------------------------------------------------------------------------------
void MainWindow::newimage()
{

    if (askOnClose()) {
        ipipip=false;
        paintArea->clearImage();

    }
}
//--------------------------------------------------------------------------------------------------




//--------------------------               Панели           ---------------------------------------


void MainWindow::einwindow()
{
dockWidget->setVisible(!widgetinstrument->isVisible()); //вкл/выкл панель инструментов
}
//--------------------------------------------------------------------------------------------------
void MainWindow::zweiwindow()
{
dockColorWidget->setVisible(!widgetpalette->isVisible()); // вкл выкл панель цветов
}
//--------------------------------------------------------------------------------------------------
void MainWindow::vierwindow()
{
dockAttributes->setVisible(!widgetAttrib->isVisible()); // вкл выкл панель слоев
}
//--------------------------------------------------------------------------------------------------
void MainWindow::createWidgetForDockWidget()
{
    widgetinstrument = new QWidget;


    //объявляем кнопки
    dockbutton1 = new QPushButton();
    dockbuttonrectangle = new QPushButton();
    dockbuttonOVAL = new QPushButton();
    dockbuttonSELECT = new QPushButton();
    dockbuttonBezier = new QPushButton();
    dockbuttonTEXT = new QPushButton();
    dockbuttonLINE = new QPushButton();
    dockbuttonRUUBER = new QPushButton();
    dockbuttonPOLYGON = new QPushButton();
    dockbuttonPIPETTE = new QPushButton();
    dockbuttonFillclick = new QPushButton();

    // делаем их нажимающимися и залипающими
    dockbutton1->setCheckable(true);
    dockbuttonrectangle->setCheckable(true);
    dockbuttonOVAL->setCheckable(true);
    dockbuttonSELECT->setCheckable(true);
    dockbuttonBezier->setCheckable(true);
    dockbuttonTEXT->setCheckable(true);
    dockbuttonLINE->setCheckable(true);
    dockbuttonRUUBER->setCheckable(true);
    dockbuttonPOLYGON->setCheckable(true);
    dockbuttonPIPETTE->setCheckable(true);
    dockbuttonFillclick->setCheckable(true);

    // пояснение к кнопкам
    dockbutton1->setToolTip(tr("<b>Инструмент:</b> кисть"));
    dockbuttonrectangle->setToolTip(tr("<b>Инструмент:</b> прямоугольник"));
    dockbuttonOVAL->setToolTip(tr("<b>Инструмент:</b> овал"));
    dockbuttonSELECT->setToolTip(tr("<b>Инструмент:</b> прямоугольное выделение"));
    dockbuttonBezier->setToolTip(tr("<b>Инструмент:</b> "));
    dockbuttonTEXT->setToolTip(tr("<b>Инструмент:</b> текст"));
    dockbuttonLINE->setToolTip(tr("<b>Инструмент:</b> прямая линия"));
    dockbuttonRUUBER->setToolTip(tr("<b>Инструмент:</b> ластик"));
    dockbuttonPOLYGON->setToolTip(tr("<b>Инструмент:</b> многоугольник"));
    dockbuttonPIPETTE->setToolTip(tr("<b>Инструмент:</b> пипетка"));
    dockbuttonFillclick->setToolTip(tr("<b>Инструмент:</b> заливка"));

    //устанавливаем "все не нажаты"
    dockbutton1->setChecked(false);
    dockbuttonrectangle->setChecked(false);
    dockbuttonOVAL->setChecked(false);
    dockbuttonSELECT->setChecked(false);
    dockbuttonBezier->setChecked(false);
    dockbuttonTEXT->setChecked(false);
    dockbuttonLINE->setChecked(false);
    dockbuttonRUUBER->setChecked(false);
    dockbuttonPOLYGON->setChecked(false);
    dockbuttonPIPETTE->setChecked(false);
    dockbuttonFillclick->setChecked(false);

    //добавляем иконки к кнопкам
    dockbutton1->setIcon(QIcon(QString::fromUtf8(":/resources/images/Brush.png")));
    dockbuttonrectangle->setIcon(QIcon(QString::fromUtf8(":/resources/images/rectangle.png")));
    dockbuttonOVAL->setIcon(QIcon(QString::fromUtf8(":/resources/images/oval.png")));
    dockbuttonSELECT->setIcon(QIcon(QString::fromUtf8(":/resources/images/select.png")));
    dockbuttonBezier->setIcon(QIcon(QString::fromUtf8(":/resources/images/bezier.png")));
    dockbuttonTEXT->setIcon(QIcon(QString::fromUtf8(":/resources/images/textpointer.png")));
    dockbuttonLINE->setIcon(QIcon(QString::fromUtf8(":/resources/images/linepointer.png")));
    dockbuttonRUUBER->setIcon(QIcon(QString::fromUtf8(":/resources/images/Eraser.png")));
    dockbuttonPOLYGON->setIcon(QIcon(QString::fromUtf8(":/resources/images/polygon.png")));
    dockbuttonPIPETTE->setIcon(QIcon(QString::fromUtf8(":/resources/images/pipette.png")));
    dockbuttonFillclick->setIcon(QIcon(QString::fromUtf8(":/resources/images/paintcan.png")));

    //прописываем сигналы и слоты для кнопок
/*    connect(dockbuttonTEXT, SIGNAL(clicked()), this, SLOT(painta1Acts()));
    connect(dockbuttonOVAL, SIGNAL(clicked()), this, SLOT(painta2Acts()));
    connect(dockbuttonLINE, SIGNAL(clicked()), this, SLOT(painta3Acts()));
    connect(dockbuttonrectangle, SIGNAL(clicked()), this, SLOT(painta5Acts()));
    connect(dockbutton1, SIGNAL(clicked()), this, SLOT(painta4Acts()));
    connect(dockbuttonRUUBER, SIGNAL(clicked()), this, SLOT(paintrubberActs()));
    connect(dockbuttonPOLYGON, SIGNAL(clicked()), this, SLOT(paintpolygonActs()));
    connect(dockbuttonSELECT, SIGNAL(clicked()), this, SLOT(paintselectActs()));
    connect(dockbuttonBezier, SIGNAL(clicked()), this, SLOT(paintbezierActs()));
    connect(dockbuttonPIPETTE, SIGNAL(clicked()), this, SLOT(paintPIPETTEActs()));*/
  //  connect(dockbuttonSELECT, SIGNAL(clicked()), this, SLOT(paintselectActs()));


   // connect(dockbuttonTEXT, SIGNAL(clicked()),this, SLOT(paintvariantpaint( )));
    connect(dockbuttonTEXT, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbuttonOVAL, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbuttonLINE, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbuttonrectangle, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbutton1, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbuttonRUUBER, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbuttonPOLYGON, SIGNAL(clicked()), this, SLOT(paintpolygonActs()));
    connect(dockbuttonSELECT, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbuttonBezier, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbuttonPIPETTE, SIGNAL(clicked()), this, SLOT(SelectInstAct()));
    connect(dockbuttonFillclick, SIGNAL(clicked()), this, SLOT(SelectInstAct()));




    //делаем первый столбец инструментов
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(dockbuttonSELECT);
    vbox->addWidget(dockbuttonRUUBER);
    vbox->addWidget(dockbuttonTEXT);
    vbox->addWidget(dockbuttonBezier);
    vbox->addWidget(dockbuttonrectangle);
    vbox->addWidget(dockbuttonOVAL);
    vbox->addStretch(1);

    //делаем второй столбец инструментов
    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(dockbuttonFillclick);
    vbox2->addWidget(dockbuttonPIPETTE);
    vbox2->addWidget(dockbutton1);
    vbox2->addWidget(dockbuttonLINE);
    vbox2->addWidget(dockbuttonPOLYGON);
    vbox2->addStretch(1);


    layout = new QGridLayout;  // делаем лайоут для столбцом инструментов
    layout->addLayout(vbox,0,0,Qt::AlignLeft);
    layout->addLayout(vbox2,0,1,Qt::AlignLeft);

    widgetinstrument->setLayout(layout);  //запихиваем готовое в виджет
}
//--------------------------------------------------------------------------------------------------
void MainWindow::createWidgetForAttribWidget()
{
   widgetAttrib = new QWidget;

   TypeLine= new QComboBox();
   LineEnd= new QComboBox ();
   JoinType= new QComboBox();
   PaintInFigur= new QPushButton(tr("Закрашивать фигуру"));
   NormalFigur= new QPushButton (tr("Обычная фигура"));
   OnlyPaint= new QPushButton(tr("Только заливка"));
   TextAttrib = new QPushButton(tr("Аттрибуты текста"));
   Antialias = new QPushButton(tr("Сглаживание"));
   Thicknessenter =new QSpinBox ();
   TLlabel= new QLabel(tr("Тип Линии ")) ;
   LELabel = new QLabel(tr("Конец линии")) ;
   TELabel = new QLabel  (tr("Толщина"));
   JTLabel= new QLabel  (tr("Соединение линий"));
   KTLabel= new QLabel  (tr("Прозрачность"));

   QStringList TypeLineList;
   QStringList LineEndList;
   QStringList JoinTypeList;
   TypeLineList<<tr("обычная")<<tr("пунктир")<<tr("точечная")<<tr("Штрихпунктир")<<tr("Штрихпунктир(2)");
   TypeLine->addItems(TypeLineList);
   LineEndList<<tr("Круглый")<<tr("Квадратный");
   LineEnd->addItems(LineEndList);
   JoinTypeList<<tr("Квадратное")<<tr("Острое")<<tr("Круглое");
   JoinType->addItems(JoinTypeList);
   Thicknessenter->setValue(3);


    PaintInFigur->setCheckable(true);
    NormalFigur->setCheckable(true);
    OnlyPaint->setCheckable(true);
    Antialias->setCheckable(true);

    PaintInFigur->setChecked(false);
    NormalFigur->setChecked(true);
    OnlyPaint->setChecked(false);
    Antialias->setChecked(true);






    QSlider *slider = new QSlider(Qt::Horizontal);

    slider->setRange(0, 255);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setValueprozr(int)));

    slider->setValue(255);

    connect(TypeLine,SIGNAL(activated(int)),this,SLOT(linemod()));
    connect(Thicknessenter,SIGNAL(valueChanged (int)),this,SLOT(Tickmode()));
    connect(LineEnd,SIGNAL(activated(int)),this,SLOT(lineendmod()));
    connect(JoinType,SIGNAL(activated(int)),this,SLOT(jointypemod()));
    connect(TextAttrib,SIGNAL(clicked(bool)),this,SLOT(Textatcall()));
    connect(PaintInFigur,SIGNAL(clicked(bool)),this,SLOT(innerpainton()));
    connect(NormalFigur,SIGNAL(clicked(bool)),this,SLOT(innerpaintoff()));
    connect(OnlyPaint,SIGNAL(clicked(bool)),this,SLOT(innerpaintonly()));
    connect(Antialias,SIGNAL(clicked(bool)),this,SLOT(antialiasmod()));



    QGridLayout *GBox1 = new QGridLayout;

    GBox1->addWidget(TLlabel,0,0,Qt::AlignLeft);
    GBox1->addWidget(LELabel,1,0,Qt::AlignLeft);
    GBox1->addWidget(JTLabel,0,2,Qt::AlignLeft);
    GBox1->addWidget(TELabel,1,2,Qt::AlignLeft);


    GBox1->addWidget(TypeLine,0,1,Qt::AlignLeft);
    GBox1->addWidget(LineEnd,1,1,Qt::AlignLeft);
    GBox1->addWidget(JoinType,0,3,Qt::AlignLeft);
    GBox1->addWidget(Thicknessenter,1,3,Qt::AlignLeft);


    GBox1->addWidget(TextAttrib,1,4);
    GBox1->addWidget(Antialias,1,5);

    GBox1->addWidget(KTLabel,0,4);

    GBox1->addWidget(slider,0,5);

    QHBoxLayout * CBoxH = new QHBoxLayout;

    CBoxH->addWidget(PaintInFigur);
    CBoxH->addWidget(NormalFigur);
    CBoxH->addWidget(OnlyPaint);

    CBoxH->addStretch(1);


    QGridLayout *GBox2 = new QGridLayout;

    GBox2->addLayout(CBoxH,0,0);


    attriblayout = new QGridLayout;
    attriblayout->addLayout(GBox1,0,0);
    attriblayout->addLayout(GBox2,0,1);
    widgetAttrib->setLayout(attriblayout);

}
//--------------------------------------------------------------------------------------------------
void MainWindow::linemod()
{
    if (TypeLine->currentText()==tr("обычная"))  paintArea->styleall=Qt::SolidLine;
    if (TypeLine->currentText()==tr("пунктир"))  paintArea->styleall=Qt::DashLine;
    if (TypeLine->currentText()==tr("точечная"))  paintArea->styleall=Qt::DotLine;
    if (TypeLine->currentText()==tr("Штрихпунктир"))  paintArea->styleall=Qt::DashDotLine;
    if (TypeLine->currentText()==tr("Штрихпунктир(2)"))  paintArea->styleall=Qt::DashDotDotLine;
   // if (TypeLine->currentText()==tr("Случайная"))  paintArea->styleall=Qt::CustomDashLine;
}
//--------------------------------------------------------------------------------------------------
void MainWindow::lineendmod()
{
    if(LineEnd->currentText()==tr("Круглый") ) paintArea->StyleLineEnd=Qt::RoundCap;
 if(LineEnd->currentText()==tr("Квадратный")) paintArea->StyleLineEnd=Qt::SquareCap;

}
//--------------------------------------------------------------------------------------------------
void MainWindow::jointypemod()
{
    if (JoinType->currentText()==tr("Квадратное"))  paintArea->StyleSoed=Qt::BevelJoin;
    if (JoinType->currentText()==tr("Острое"))  paintArea->StyleSoed=Qt::MiterJoin;
    if (JoinType->currentText()==tr("Круглое"))  paintArea->StyleSoed=Qt::RoundJoin;
}
//--------------------------------------------------------------------------------------------------
void MainWindow::Tickmode()
{
paintArea->thickness=Thicknessenter->value();
}
//--------------------------------------------------------------------------------------------------
void MainWindow::Textatcall()
{
    QFont Settablefont;
    bool ok;
    Settablefont = QFontDialog::getFont(&ok, QFont("Times", 12), this);
    paintArea->StyleText=Settablefont ;
};
//--------------------------------------------------------------------------------------------------
void MainWindow::innerpainton()
{
  QBrush  brushmod(paintArea->backgroundcolor);
        paintArea->inpaint=brushmod;
        NormalFigur->setChecked(false);
        OnlyPaint->setChecked(false);

};
//--------------------------------------------------------------------------------------------------
void MainWindow::innerpaintoff()
{
    QBrush  brushmod(Qt::NoBrush);
  paintArea->inpaint=brushmod;
  PaintInFigur->setChecked(false);
  OnlyPaint->setChecked(false);
};
//--------------------------------------------------------------------------------------------------
void MainWindow::innerpaintonly()
{
    QBrush  brushmod(paintArea->firstcolor);
    paintArea->inpaint=brushmod;
    PaintInFigur->setChecked(false);
    NormalFigur->setChecked(false);
};
//--------------------------------------------------------------------------------------------------
void MainWindow::antialiasmod()
{
paintArea->sglali=!(paintArea->sglali);
};
//--------------------------------------------------------------------------------------------------

void MainWindow::setValueprozr(int a)
{
paintArea->prozracn=a;
}


// ---------------------------------            Слои ----------------------



//--------------------------               Инструменты           -----------------------------------




//--------------------------------------------------------------------------------------------------
void MainWindow::SelectInstAct()
{

  // через switch and case не получилось
    // убрал ветки илс, сделал более читаемым
    setCursor(Qt::CrossCursor);

    dockbutton1->setChecked(false);
    dockbuttonrectangle->setChecked(false);
    dockbuttonOVAL->setChecked(false);
    dockbuttonSELECT->setChecked(false);
    dockbuttonBezier->setChecked(false);
    dockbuttonLINE->setChecked(false);
    dockbuttonRUUBER->setChecked(false);
    dockbuttonPOLYGON->setChecked(false);
    dockbuttonPIPETTE->setChecked(false);
    dockbuttonFillclick->setChecked(false);

    if (sender()==paintairBrushAct)
    {
         paintArea->mytoolsselectwin=10; sb5->setText(tr("<b>Инструмент:</b> Воздушная кисть"));
    }

    if (sender()==paintRandomCharAct)
    {
         paintArea->mytoolsselectwin=11; sb5->setText(tr("<b>Инструмент:</b> Случайные буквы"));
    }

    if (sender()==dockbutton1)
    {
        dockbutton1->setChecked(true) ; paintArea->mytoolsselectwin=5;
        sb5->setText(tr("<b>Инструмент:</b> Кисть"));
        sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> кликните по холсту и сдвиньте мышь."));
    }


   if (sender()==dockbuttonrectangle)
    {
       dockbuttonrectangle->setChecked(true);
       paintArea->mytoolsselectwin=4; sb5->setText(tr("<b>Инструмент:</b> Прямоугольник"));
       sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> зажмите левую кнопку мыши передвиньте мышь, отпустите. Будет нарисован прямоугольник."));
    }

   if (sender()==dockbuttonOVAL)
    {
        dockbuttonOVAL->setChecked(true);
        paintArea->mytoolsselectwin=2; sb5->setText(tr("<b>Инструмент:</b> Овал"));
        sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> зажмите левую кнопку мыши передвиньте мышь, отпустите. Будет нарисован овал. "));
    }

   if (sender()==dockbuttonSELECT)
    {
        dockbuttonSELECT->setChecked(true);
        paintArea->mytoolsselectwin=8; sb5->setText(tr("<b>Инструмент:</b> Прямоугольное выделение"));
        sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> кликните по холсту и зажмите, а потом ведите. Отпустите, выделится область"));
    }

   if (sender()==dockbuttonBezier)
    {
        dockbuttonBezier->setChecked(true);
        paintArea->mytoolsselectwin=9; sb5->setText(tr("<b>Инструмент:</b> Кривая линия"));
        sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> проведите прямую, а потом кликните еще 2 раза по разным точкам и увидете кривую"));
    }

   if (sender()==dockbuttonTEXT)
    {   dockbuttonTEXT->setChecked(true);
        paintArea->mytoolsselectwin=1; sb5->setText(tr("<b>Инструмент:</b> Текст"));
        sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> кликните по хослту появится диалог ввода текста."));
    }

   if ( sender()==dockbuttonLINE)
    {
        dockbuttonLINE->setChecked(true);
        paintArea->mytoolsselectwin=3; sb5->setText(tr("<b>Инструмент:</b> Прямая линия"));
        sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> кликните по холсту сдвиньте мышь, отпустите, нарисуется прямая линия"));
    }

   if (sender()==dockbuttonRUUBER)
    {
        dockbuttonRUUBER->setChecked(true);
        paintArea->mytoolsselectwin=6; sb5->setText(tr("<b>Инструмент:</b> Ластик"));
        sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> кликните по холсту и сдвиньте мышь, и ластик стерет нарисованное"));
    }

   if (sender()==dockbuttonPIPETTE)
    {
       dockbuttonPIPETTE->setChecked(true);
       paintArea->mytoolsselectwin=90; sb5->setText(tr("<b>Инструмент:</b> Пипетка"));widgetpalette->pickMode();
       sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> Кликните в любую точку и цвет этой точки установится в палитру"));
    }

   if (sender()==dockbuttonFillclick)
    {
       dockbuttonFillclick->setChecked(true);
       paintArea->mytoolsselectwin=14; sb5->setText(tr("<b>Инструмент:</b> Заливка"));
       sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> Кликните в любую область и область закрасится фоновым цветом"));
    }


























}
//--------------------------------------------------------------------------------------------------
void MainWindow::paintpolygonActs()
{
    dockbutton1->setChecked(false);
    dockbuttonrectangle->setChecked(false);
    dockbuttonOVAL->setChecked(false);
    dockbuttonSELECT->setChecked(false);
    dockbuttonBezier->setChecked(false);
    dockbuttonTEXT->setChecked(false);
    dockbuttonLINE->setChecked(false);
    dockbuttonRUUBER->setChecked(false);
    dockbuttonPIPETTE->setChecked(false);

    paintArea->mytoolsselectwin=7;
    sb5->setText(tr("<b>Инструмент:</b> Многоугольник"));
    sb5->setToolTip(tr("<b>Текущий инструмент <br> Инструкция:</b> кликайте по холсту и многоугольник будет рисоваться, если кликните 2 раза в одну точку многоугольник закончится"));
}
//--------------------------------------------------------------------------------------------------
