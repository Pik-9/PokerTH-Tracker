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

#ifndef MULTIVIEW_HPP
#define MULTIVIEW_HPP

#include <QWidget>

#include <vector>

#include "PlayerStat.hpp"

class QTableWidget;
class QPushButton;
class QGridLayout;
class QLabel;
class QComboBox;
class QShowEvent;

class MultiView : public QWidget
{
  Q_OBJECT
private:
  QGridLayout *layout;
  QLabel *l_tsize;
  QComboBox *c_tsize;
  QPushButton *b_ok, *b_clear, *b_rem;
  QTableWidget *table;
  
  Statistics* stat;
  std::vector<QString> observedPlayers;
  
public:
  MultiView (Statistics*, QWidget* parent = 0);
  ~MultiView ();
  
protected:
  void showEvent (QShowEvent*);
  
public slots:
  void writeTable ();
  void updateTable (int);
  void selectPlayers ();
  void clickedRemove ();
  void clickedClear ();
  void addPlayer (const QString);
  
  /* Not sure whether this function will ever be necessary... */
  void removePlayer (const QString);
};

#endif