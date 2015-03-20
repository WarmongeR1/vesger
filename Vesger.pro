TARGET = Vesger
TEMPLATE = app
INCLUDEPATH += src/include
QT += core
include(./src/cpp/filter/qtimagefilters.pri)
DEPENDPATH += src/mainwindow \
    src/cpp/dialogs \
    src/ui \
    src/include \
    src/cpp \
    plugins
HEADERS += mainwindow.h \
    paintarea.h \
    aboutdialog.h \
    plugindialog.h \
    global.h \
    screen.h \
    qtcolortriangle.h \
    palette.h \
    interfaces.h \
    colorviewer.h \
    imagetoolwidget.h \
    mirrorfilter.h
SOURCES += main.cpp \
    mainwindow.cpp \
    plugindialog.cpp \
    about.cpp \
    paintarea.cpp \
    palette.cpp \
    screen.cpp \
    qtcolortriangle.cpp \
    colorviewer.cpp \
    imagetoolwidget.cpp
FORMS += about.ui \
    resizedialog.ui \
    gradientdialog.ui \
    filterdialog.ui
RESOURCES += images.qrc
LIBS = -L../tools/plugandpaintplugins/basictools \
    -L \
    plugins/ \
    -lpnp_basictools
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) { 
    mac:LIBS = $$member(LIBS, 0) \
        $$member(LIBS, 1)_debug
    win32:LIBS = $$member(LIBS, 0) \
        $$member(LIBS, 1)d
}

# ! [0]
# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaint
sources.files = $$SOURCES \
    $$HEADERS \
    $$RESOURCES \
    $$FORMS \
    plugandpaint.prote
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaint
INSTALLS += target \
    sources
