#include "global.h"
#include "aboutdialog.h"
#include <QDialog>
#include <QFile>

#include <QTextCodec>

CAboutForm::CAboutForm(QWidget *parent)
    :QDialog(parent)
{

    QTextCodec *codec = QTextCodec::codecForName("UTF8");  // назначаем кодировку для всех надписей
    QTextCodec::setCodecForTr(codec);

    setupUi(this); // setup user interface (see Trolltech Qt designer, uic, moc user manual pages
    setAttribute( Qt::WA_DeleteOnClose );
    setFixedSize( size() ); // not resizeable

// html файлы не юзатся, оставлено "просто так"
  //  lInformations->setText( QString( "%1 %2 %3\n" ).arg( APP_NAME, APP_VERSION,BUILD_DATE ) ); // set APP info
     lInformations->setText(tr("%1 %2").arg(APP_NAME_AND_VER,BUILD_DATE));
	QFile file;
        file.setFileName( ":/resources/about/authors.html" ); // get authors page
	file.open( QFile::ReadOnly | QFile::Text );
	teAuthors->setHtml( file.readAll() ); 						// show it
        file.close();
        file.setFileName( ":/resources/about/todo.html" ); // get todo page
	file.open( QFile::ReadOnly | QFile::Text );
        teTodo->setHtml(file.readAll());
        //teTodo->setHtml( file.readAll() );                       // show it
	file.close();
        file.setFileName( ":/resources/about/greetings.html" ); // get greetings page
	file.open( QFile::ReadOnly | QFile::Text );
	teGreetings->setHtml( file.readAll() );                       // show it
	file.close();
        file.setFileName( ":/resources/about/license.html" ); // get license page
	file.open( QFile::ReadOnly | QFile::Text );
        teBugs->setHtml( file.readAll() );                       // show it
	file.close();



}





