#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main (int argc, char *argv[])
{
  QApplication app (argc, argv);
  QTranslator *trans = new QTranslator ();
  QString filename = QString ("PokerTH_tracker_%1.qm").arg (QLocale::system ().name ());
  trans->load (filename, "/usr/local/share/PokerTH_tracker");
  app.installTranslator (trans);
  MainWindow *mw = new MainWindow ();
  mw->show ();
  return app.exec();
}