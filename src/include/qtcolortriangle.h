

#ifndef QTCOLORTRIANGLE_H
#define QTCOLORTRIANGLE_H
#include <QImage>
#include <QWidget>

class QPointF;
struct Vertex;

#if defined(Q_WS_WIN)
#  if !defined(QT_QTCOLORTRIANGLE_EXPORT) && !defined(QT_QTCOLORTRIANGLE_IMPORT)
#    define QT_QTCOLORTRIANGLE_EXPORT
#  elif defined(QT_QTCOLORTRIANGLE_IMPORT)
#    if defined(QT_QTCOLORTRIANGLE_EXPORT)
#      undef QT_QTCOLORTRIANGLE_EXPORT
#    endif
#    define QT_QTCOLORTRIANGLE_EXPORT __declspec(dllimport)
#  elif defined(QT_QTCOLORTRIANGLE_EXPORT)
#    undef QT_QTCOLORTRIANGLE_EXPORT
#    define QT_QTCOLORTRIANGLE_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTCOLORTRIANGLE_EXPORT
#endif

class QT_QTCOLORTRIANGLE_EXPORT QtColorTriangle : public QWidget
{
    Q_OBJECT

public:
    QtColorTriangle(QWidget *parent = 0);
    ~QtColorTriangle();

    QSize sizeHint() const;
    int heightForWidth(int w) const;

    void polish();
    QColor color() const;
    QColor colorBack() const;

Q_SIGNALS:
    void colorChanged(const QColor &col);
    void colorChangedBack(const QColor &col);

public Q_SLOTS:
    void setColor(const QColor &col);
    void setColorBack(const QColor &col);

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent *);
    void drawTrigon(QImage *p, const QPointF &a, const QPointF &b,
                    const QPointF &c, const QColor &color);

private:
    double radiusAt(const QPointF &pos, const QRect &rect) const;
    double angleAt(const QPointF &pos, const QRect &rect) const;
    QPointF movePointToTriangle(double x, double y, const Vertex &a,
                                    const Vertex &b, const Vertex &c) const;

    QPointF pointFromColor(const QColor &col) const;
    QColor colorFromPoint(const QPointF &p) const;

    void genBackground();

    QImage bg;
    double a, b, c;
    QPointF pa, pb, pc, pd;



    int curHue;

    bool mustGenerateBackground;
    int penWidth;
    int ellipseSize;

    int outerRadius;
    QPointF selectorPos;

    enum SelectionMode {
        Idle,
        SelectingHue,
        SelectingSatValue
    } selMode;

    QColor curColor;
    QColor curColorBack;
};

#endif
