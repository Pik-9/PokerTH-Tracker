#ifndef GLOBAL_HPP
#define GLOBAL_HPP

class QSettings;
class QDir;
class QWidget;
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
  
  bool getGeomSave ();
  void setGeomSave (const bool);
  
  void setMainWinGeom (QWidget*);
  void getMainWinGeom (QWidget*);
  void setMultiViewGeom (QWidget*);
  void getMultiViewGeom (QWidget*);
};

#endif