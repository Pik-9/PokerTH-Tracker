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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "PlayerStat.hpp"
#include "worker.hpp"
#include "multiview.hpp"
#include "analysis_widget.hpp"
#include "notes_stars.hpp"

class QSplitter;
class QLineEdit;
class QLabel;
class QPushButton;
class QComboBox;
class QListWidget;
class QListWidgetItem;
class QAction;
class QActionGroup;
class QGridLayout;
class QTabWidget;
class QFileDialog;

class LeftPart : public QWidget
{
  Q_OBJECT
private:
  QGridLayout *layout;
  QPushButton *b_search;
  QLineEdit *t_path;
  QListWidget *l_players;
  QFileDialog *lopen;

public:
  LeftPart (QWidget *parent = 0);
  ~LeftPart ();

  QString getFilePath () const;
  QListWidget* getListWidget ();

public slots:
  void chooseURL (const QString);

signals:
  void changedDirectory ();
};

class RightPart : public QWidget
{
  Q_OBJECT
private:
  QGridLayout *layout;
  QComboBox *c_table;
  QLabel *l_tsize, *l_name, *l_obh, *t_obh;
  QLabel *l_preflop, *l_postflop, *l_winnings;
  QLabel *l_vpip, *t_vpip,
    *l_pfr, *t_pfr,
    *l_3bet, *t_3bet;
  QLabel *l_conti, *t_conti,
    *l_fconti, *t_fconti,
    *l_tbet, *t_tbet,
    *l_ftbet, *t_ftbet;
  QLabel *l_fnbet, *t_fnbet;
  QLabel *l_cr, *t_cr;
  QLabel *l_aAF, *t_aAF,
    *l_fAF, *t_fAF,
    *l_tseen, *t_tseen,
    *l_tAF, *t_tAF,
    *l_rseen, *t_rseen,
    *l_rAF, *t_rAF;
  QLabel *l_wts, *t_wts,
    *l_wwsf, *t_wwsf,
    *l_wsd, *t_wsd;

public:
  RightPart (QWidget *parent = 0);
  ~RightPart ();

  void setupProps (const QString, Statistics*);
  tableSize desiredTableSize ();
  
public slots:
  void changedTableSize (int);
  
signals:
  void requestStat ();
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

private:
  QSplitter *splitter;
  QTabWidget *tabs;
  QAction *saveGeomSetting;
  QActionGroup *m_lang;
  LeftPart *lp;
  RightPart *rp;
  AnaWidget *ap;
  NotesStarsWidget *np;
  MultiView *mv;

  Statistics *stat;
  Worker *work_thread;
  
  std::map<QAction*, QString> langCodes;

public:
  MainWindow (QWidget *parent = 0);
  ~MainWindow ();

public slots:
  void refresh ();
  void buildList ();
  void showPlayerStats ();
  void showPlayerStats (const QString);
  void addToMultiview (QListWidgetItem*);
  void clickedAboutQT ();
  void clickedAbout ();
  void changedLang ();
  void quitSavingSettings ();
};

#endif