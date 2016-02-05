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

#include <stdint.h>
#include <map>

class QSplitter;
class QLineEdit;
class QLabel;
class QPushButton;
class QComboBox;
class QListWidget;
class QGridLayout;
class QFileDialog;

struct PlayerStat
{
  /* Preflop */
  uint32_t observed_hands, pf_calls, pf_open;

  /* Flop */
  uint32_t f_seen, f_check_call, f_bet, f_contibet, f_fold;

  /* Turn */
  uint32_t t_seen, t_check_call, t_bet, t_fold;

  /* River */
  uint32_t r_seen, r_check_call, r_bet, r_fold;

  /* Showdown */
  uint32_t sd_seen, sd_won;
  
  double VPIP () const;
  double preflop_raise () const;
  double AF_ave () const;
  double AF_flop () const;
  double AF_turn () const;
  double AF_river () const;
  double contibet () const;
  double seen_turn () const;
  double seen_river () const;
  double wtShowdown () const;
  double wonShowdown () const;
  
  PlayerStat& operator+= (PlayerStat&);
};

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
  QLabel *l_tsize, *l_name, *l_obh, *l_vpip, *l_pfr, *l_faf, *l_conbet, *l_tseen, *l_taf, *l_rseen, *l_raf, *l_af, *l_wts, *l_sdw;
  QLabel *t_obh, *t_vpip, *t_pfr, *t_faf, *t_conbet, *t_tseen, *t_taf, *t_rseen, *t_raf, *t_af, *t_wts, *t_sdw;
  QLabel *l_preflop_cap, *l_flop_cap, *l_turn_cap, *l_river_cap, *l_showdown_cap;

public:
  RightPart (QWidget *parent = 0);
  ~RightPart ();

  void setupProps (const QString, const PlayerStat);
  int desiredTableSize ();
  
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
  LeftPart *lp;
  RightPart *rp;

  /* 
   * Different maps for different table sizes:
   *   player[0]: Fullring (7 - 10 players)
   *   player[1]: Shorthand (3 - 6 players)
   *   player[2]: Heads-Up (2 players)
   */
  std::map<QString, PlayerStat> player[3];
  
  /* A map for the players in any situation. */
  std::map<QString, PlayerStat> allPlayers;

public:
  MainWindow (QWidget *parent = 0);
  ~MainWindow ();

public slots:
  void refresh ();
  void showPlayerStats ();
  void showPlayerStats (const QString);
  void clickedAboutQT ();
  void clickedAbout ();
};

#endif