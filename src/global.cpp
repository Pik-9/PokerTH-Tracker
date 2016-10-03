/* 
 * PokerTH Tracker - A simple poker statistics tracker for PokerTH
 * 
 * (C) 2015-2016 Daniel Steinhauer
 * E-Mail: pik-9@users.sourceforge.net
 * 
 * This file is part of PokerTH Tracker
 * 
 * PokerTH Tracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PokerTH Tracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.hpp"

#include <QSettings>
#include <QDir>
#include <QWidget>
#include <QApplication>
#include <QDesktopWidget>
#include <QRect>
#include <QLocale>

Global* Global::singleton = 0;

Global::Global ()
{
  app_settings = new QSettings ("Pik-9", "PokerTH-Tracker");
  QDir logData (QDir::home ());
  
  /* Try to find the log file path automatically. */
  if (!app_settings->contains ("defaultPath"))  {
    /* Try to navigate to the linux client's log file directory.
     * Otherwise try to navigate to the windows client's log file directory. */
    if (!logData.cd (".pokerth/log-files"))  {
      /* If this fails as well, stay at home! */
      logData.cd ("AppData/Roaming/pokerth/log-files");
    }
    app_settings->setValue ("defaultPath", logData.absolutePath ());
  }
  
  /* Try to find the config.xml automatically. */
  if (!app_settings->contains ("configPath"))  {
    QDir configP (QDir::home ());
    /* Try to navigate to the linux client's config file.
     * Otherwise try to navigate to the windows client's config file. */
    if (!configP.cd (".pokerth"))  {
      /* If this fails as well, try the log file directory! */
      if (!configP.cd ("AppData/Roaming/pokerth/log-files"))  {
        configP.cd (logData.absolutePath ());
        configP.cdUp ();
      }
    }
    /* Check whether we find the config.xml here. */
    if (configP.exists ("config.xml"))  {
      app_settings->setValue ("configPath", configP.absolutePath () + "config.xml");
    }
  }
  
  app_settings->sync ();
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

QString Global::getConfigFile ()
{
  QString RET = app_settings->value ("configPath", "").toString ();
  if (RET.isEmpty ())  {
    QDir suggest_dir = QDir::home ();
    if (!suggest_dir.cd (".pokerth"))  {
      /* If this is no Linux system, try the default Windows folder. */
      if (!suggest_dir.cd ("AppData/Roaming/pokerth"))  {
        /* Giving up, if this fails as well! */
        return "";
      }
    }
    QString suggest = suggest_dir.absoluteFilePath ("config.xml");
    if (QFile::exists (suggest))  {
      app_settings->setValue ("configPath", suggest);
      RET = suggest;
    }
  }
  return RET;
}

void Global::setConfigFile (const QString configPath)
{
  app_settings->setValue ("configPath", configPath);
  app_settings->sync ();
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

QString Global::getLang ()
{
  QString lang_xx;
  if (app_settings->contains ("language"))  {
    lang_xx = app_settings->value ("language").toString ();
  } else  {
    lang_xx = QLocale::system ().name ();
    setLang (lang_xx);
  }
  return lang_xx;
}

void Global::setLang (const QString xx)
{
  app_settings->setValue ("language", xx);
  app_settings->sync ();
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