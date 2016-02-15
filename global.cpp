#include "global.hpp"

#include <QSettings>
#include <QDir>

Global* Global::singleton = 0;

Global::Global ()
{
  app_settings = new QSettings ("Pik-9", "PokerTH-Tracker");
  dataDir = new QDir ("/usr/local/share/PokerTH_tracker");
  if (!dataDir->exists ())  {
    /* The directory above doesn't exist,
     * go to data in the current directory.
     * This is necessary for the Windows client.
     */
    *dataDir = QDir::current ();
    dataDir->cd ("data");
  }
  
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
}

void Global::setLogDir (const QDir logFolder)
{
  setLogDir (logFolder.absolutePath ());
}

QString Global::getDataDir ()
{
  return dataDir->absolutePath ();
}