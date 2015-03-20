

#include <QtGui>
#include <QtCore/QVector>
#include "qtimagefilter.h"
//#include "ui_imagetoolwidget.h"

#include "ui_filterdialog.h"

class PaintArea;

class ImageToolWidget : public QWidget {
    Q_OBJECT
    public:
        ImageToolWidget(QWidget *parent = 0);
        virtual ~ImageToolWidget();
        void setPixmapAndResize(const QPixmap &pixmap);
        QImage imgToFilter;

        QPixmap mypixmap;

        bool kilops;
        bool kilops2;
    public slots:
        void loadImage();
        void reloadImage();
        void filterImage();
        void acept();
        void filterIndexChanged(int index);

    private:
//PaintArea *paintArea;
        QString m_currentFilename;
        QVector<QtImageFilter*>  m_imageFilters;
        Ui::Form ui;


protected:
        PaintArea *paintArea;
};

