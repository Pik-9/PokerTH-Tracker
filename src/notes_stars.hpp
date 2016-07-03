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

#ifndef NOTES_STARS_HPP
#define NOTES_STARS_HPP

#include <QWidget>

#include <map>

class QGridLayout;
class QSlider;
class QPushButton;
class QLabel;
class QTextEdit;
class QDomDocument;

struct NotesStars
{
  QString notes;
  int stars;
};

class NotesStarsWidget : public QWidget
{
  Q_OBJECT
private:
  QGridLayout *layout;
  QLabel *l_stars;
  QSlider *s_stars;
  QTextEdit *t_notes;
  QPushButton *b_disc, *b_appl;
  
  QString currentPlayer;
  QDomDocument *pth_config;
  std::map<QString, NotesStars> entries;
  
public:
  NotesStarsWidget (QWidget *parent = 0);
  ~NotesStarsWidget ();
  
  static const QString separator;
  
public slots:
  void clickedApply ();
  void clickedDiscard ();
  void contentEdited (int dump = 0);
  void loadNotes ();
  void showPlayerNotes (QString player_name = "");
};

#endif