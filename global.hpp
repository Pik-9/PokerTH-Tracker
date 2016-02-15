#ifndef GLOBAL_HPP
#define GLOBAL_HPP

class QSettings;
class QDir;
class QString;

class Global
{
private:
  static Global* singleton;
  
  QSettings* app_settings;
  QDir* dataDir;
  
  Global ();
  virtual ~Global ();
  
public:
  static Global* getInstance ();
  QString getLogDir ();
  void setLogDir (const QString);
  void setLogDir (const QDir);
  QString getDataDir ();
};

#endif