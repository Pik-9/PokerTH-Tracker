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
  
  Global ();
  virtual ~Global ();
  
public:
  static Global* getInstance ();
  
  QString getLogDir ();
  void setLogDir (const QString);
  void setLogDir (const QDir);
  
  QString getLang ();
  void setLang (const QString);
  
  bool getGeomSave ();
  void setGeomSave (const bool);
  void setMainWinGeom (QWidget*);
  void getMainWinGeom (QWidget*);
  void setMultiViewGeom (QWidget*);
  void getMultiViewGeom (QWidget*);
};

#endif