#include "global.hpp"

#include <QSettings>
#include <QDir>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>

Global* Global::singleton = 0;

Global::Global ()
{
  app_settings = new QSettings ("Pik-9", "PokerTH-Tracker");
  
  if (!app_settings->contains ("defaultPath"))  {
    QDir logData (QDir::home ());
    /* Try to navigate to the linux client's log file directory.
     * Otherwise try to navigate to the windows client's log file directory. */
    if (!logData.cd (".pokerth/log-files"))  {
      /* If this fails as well, stay at home! */
      logData.cd ("AppData/Roaming/pokerth/log-files");
    }
    app_settings->setValue ("defaultPath", logData.absolutePath ());
  }
}

Global::~Global ()
{
  delete app_settings;
}

Global* Global::getInstance ()
{
  if (!singleton)  {
    singleton = new Global ();
  }
  return singleton;
}

QString Global::getLogDir ()
{
  return app_settings->value ("defaultPath").toString ();
}

void Global::setLogDir (const QString logPath)
{
  app_settings->setValue ("defaultPath", logPath);
  app_settings->sync ();
}

void Global::setLogDir (const QDir logFolder)
{
  setLogDir (logFolder.absolutePath ());
}

bool Global::getGeomSave ()
{
  return app_settings->value ("geomSave", false).toBool ();
}

void Global::setGeomSave (const bool save_geometry)
{
  app_settings->setValue ("geomSave", save_geometry);
  app_settings->sync ();
}

void Global::setMainWinGeom (QWidget* mainWindow)
{
  app_settings->beginGroup ("MainWinGeom");
  app_settings->setValue ("size", mainWindow->size ());
  app_settings->setValue ("position", mainWindow->pos ());
  app_settings->endGroup ();
  app_settings->sync ();
}

void Global::getMainWinGeom (QWidget* mainWindow)
{
  app_settings->beginGroup ("MainWinGeom");
  QPoint posi = app_settings->value ("position").toPoint ();
  QSize mwsize = app_settings->value ("size").toSize ();
  app_settings->endGroup ();
  
  if (getGeomSave ())  {
    mainWindow->move (posi);
    mainWindow->resize (mwsize);
  } else  {
    mainWindow->resize (1024, 786);
    
    /* Move window to screen center. */
    QRect geom = QApplication::desktop ()->screenGeometry ();
    mainWindow->move ((geom.width () - mainWindow->width ()) / 2, (geom.height () - mainWindow->height ()) / 2);
  }
}

void Global::setMultiViewGeom (QWidget* multiView)
{
  app_settings->beginGroup ("MultiViewGeom");
  app_settings->setValue ("size", multiView->size ());
  app_settings->setValue ("position", multiView->pos ());
  app_settings->endGroup ();
  app_settings->sync ();
}

void Global::getMultiViewGeom(QWidget* multiView)
{
  app_settings->beginGroup ("MultiViewGeom");
  QPoint posi = app_settings->value ("position").toPoint ();
  QSize mwsize = app_settings->value ("size").toSize ();
  app_settings->endGroup ();
  
  if (getGeomSave ())  {
    multiView->move (posi);
    multiView->resize (mwsize);
  } else  {
    multiView->resize (1000, 786);
    
    /* Move window to screen center. */
    QRect geom = QApplication::desktop ()->screenGeometry ();
    multiView->move ((geom.width () - multiView->width ()) / 2, (geom.height () - multiView->height ()) / 2);
  }
}