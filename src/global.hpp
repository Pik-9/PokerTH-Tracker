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
  QString getConfigFile ();
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