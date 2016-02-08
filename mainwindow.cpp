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

#include "mainwindow.hpp"

#include <QListWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QSplitter>
#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSettings>

double PlayerStat::VPIP () const
{
  return 100.0 * (pf_calls + pf_open) / observed_hands;
}

double PlayerStat::preflop_raise () const
{
  return 100.0 * pf_open / observed_hands;
}

double PlayerStat::AF_ave () const
{
  return (AF_flop () + AF_turn () + AF_river ()) / 3.0;
}

double PlayerStat::AF_flop () const
{
  return (double) f_bet / f_check_call;
}

double PlayerStat::AF_turn () const
{
  return (double) t_bet / t_check_call;
}

double PlayerStat::AF_river () const
{
  return (double) r_bet / r_check_call;
}

double PlayerStat::contibet () const
{
  return 100.0 * f_contibet / pf_open;
}

double PlayerStat::seen_turn () const
{
  return 100.0 * t_seen / observed_hands;
}

double PlayerStat::seen_river () const
{
  return 100.0 * r_seen / observed_hands;
}

double PlayerStat::wtShowdown () const
{
  return 100.0 * sd_seen / f_seen;
}

double PlayerStat::wonShowdown () const
{
  return 100.0 * sd_won / sd_seen;
}

PlayerStat& PlayerStat::operator+= (PlayerStat& other)
{
  observed_hands += other.observed_hands;
  pf_open += other.pf_open;
  pf_calls += other.pf_calls;
  f_seen += other.f_seen;
  f_check_call += other.f_check_call;
  f_bet += other.f_bet;
  f_contibet += other.f_contibet;
  f_fold += other.f_fold;
  t_seen += other.t_seen;
  t_bet += other.t_bet;
  t_check_call += other.t_check_call;
  t_fold += other.t_fold;
  r_seen += other.r_seen;
  r_bet += other.r_bet;
  r_check_call += other.r_check_call;
  r_fold += other.r_fold;
  sd_seen += other.sd_seen;
  sd_won += other.sd_won;
  return *this;
}

/* Dirty workaround! */
struct RoundAct
{
  uint8_t data[5];
  uint8_t& operator[] (unsigned int id) { return data[id]; }
};

LeftPart::LeftPart (QSettings* config, QWidget *parent)
  : QWidget (parent), settings (config)
{
  layout = new QGridLayout (this);
  t_path = new QLineEdit ();
  t_path->setReadOnly (true);
  b_search = new QPushButton (tr ("Search..."));
  l_players = new QListWidget ();
  lopen = new QFileDialog ();
  lopen->setFileMode (QFileDialog::Directory);
  layout->addWidget (t_path, 0, 0, 1, 1);
  layout->addWidget (b_search, 0, 1, 1, 1);
  layout->addWidget (l_players, 1, 0, 1, 2);

  if (!settings->contains ("defaultPath"))  {
    QDir logData (QDir::home ());
    /* Try to navigate to the linux client's log file directory.
     * Otherwise try to navigate to the windows client's log file directory. */
    if (!logData.cd (".pokerth/log-files"))  {
      /* If this fails as well, stay at home! */
      logData.cd ("AppData/Roaming/pokerth/log-files");
    }
    settings->setValue ("defaultPath", logData.absolutePath ());
  }
  t_path->setText (settings->value ("defaultPath").toString ());

  connect (b_search, SIGNAL (clicked ()), lopen, SLOT (show ()));
  connect (lopen, SIGNAL (fileSelected (const QString)), this, SLOT (chooseURL (const QString)));
}

LeftPart::~LeftPart ()
{}

QString LeftPart::getFilePath () const
{
  return t_path->text ();
}

QListWidget* LeftPart::getListWidget ()
{
  return l_players;
}

void LeftPart::chooseURL (const QString path)
{
  t_path->setText (path);
  emit changedDirectory ();
}

RightPart::RightPart (QWidget *parent)
  : QWidget (parent)
{
  layout = new QGridLayout (this);
  l_name = new QLabel ("Name");
  l_name->setStyleSheet ("QLabel { font-weight: bold }");
  l_name->setAlignment(Qt::AlignCenter);
  
  l_tsize = new QLabel (tr ("Table size:"));
  c_table = new QComboBox ();
  c_table->addItem (tr ("Fullring (7 - 10 players)"));
  c_table->addItem (tr ("Shorthand (3 - 6 players)"));
  c_table->addItem (tr ("Heads-Up (2 players)"));
  c_table->addItem (tr ("Any"));
  c_table->setCurrentIndex (3);
  
  l_obh = new QLabel (tr ("Observed hands:"));
  l_af = new QLabel (tr ("Average Aggression Factor:"));
  l_af->setCursor (Qt::WhatsThisCursor);
  l_af->setToolTip (tr ("Agression Factor = (times of <i>bet</i> or <i>raise</i>) / (times of <i>check</i> or <i>call</i>)"));
  
  l_preflop_cap = new QLabel (tr ("<b>Preflop</b>"));
  l_vpip = new QLabel (tr ("VP$IP:"));
  l_vpip->setCursor (Qt::WhatsThisCursor);
  l_vpip->setToolTip (tr ("Percantage of hands the player voluntarily invested money in."));
  l_pfr = new QLabel (tr ("Preflop Raise:"));
  l_pfr->setCursor (Qt::WhatsThisCursor);
  l_pfr->setToolTip (tr ("Percentage of hands the player raised with preflop."));
  
  l_flop_cap = new QLabel (tr ("<b>Flop</b>"));
  l_faf = new QLabel (tr ("Flop Aggression Factor:"));
  l_conbet = new QLabel (tr ("Continuation bet:"));
  l_conbet->setCursor (Qt::WhatsThisCursor);
  l_conbet->setToolTip (tr ("How often the player bet on the Flop after he raised preflop."));
  
  l_turn_cap = new QLabel (tr ("<b>Turn</b>"));
  l_tseen = new QLabel (tr ("Turn seen:"));
  l_tseen->setCursor (Qt::WhatsThisCursor);
  l_tseen->setToolTip (tr ("With how many hands the player went to the Turn."));
  l_taf = new QLabel (tr ("Turn Aggression Factor:"));
  
  l_river_cap = new QLabel (tr ("<b>River</b>"));
  l_rseen = new QLabel (tr ("River seen:"));
  l_rseen->setCursor (Qt::WhatsThisCursor);
  l_rseen->setToolTip (tr ("With how many hands the player went to the River."));
  l_raf = new QLabel (tr ("River Aggression Factor:"));
  
  
  l_showdown_cap = new QLabel (tr ("<b>Showdown</b>"));
  l_wts = new QLabel (tr ("Went to Showdown:"));
  l_wts->setCursor (Qt::WhatsThisCursor);
  l_wts->setToolTip (tr ("With how many hands the player went to Showdown <b>after the flop</b>."));
  l_sdw = new QLabel (tr ("Won Showdown:"));
  l_sdw->setCursor (Qt::WhatsThisCursor);
  l_sdw->setToolTip (tr ("How many Showdowns the player won."));
  
  t_obh = new QLabel ("0");
  t_vpip = new QLabel ("0");
  t_pfr = new QLabel ("0");
  t_faf = new QLabel ("0");
  t_conbet = new QLabel ("0");
  t_tseen = new QLabel ("0");
  t_taf = new QLabel ("0");
  t_rseen = new QLabel ("0");
  t_raf = new QLabel ("0");
  t_af = new QLabel ("0");
  t_wts = new QLabel ("0");
  t_sdw = new QLabel ("0");

  layout->addWidget (l_name, 0, 0, 1, 2);
  layout->addWidget (l_tsize, 1, 0, 1, 1);
  layout->addWidget (c_table, 1, 1, 1, 1);
  
  layout->addWidget (l_obh, 2, 0, 1, 1);
  layout->addWidget (t_obh, 2, 1, 1, 1);
  layout->addWidget (l_af, 3, 0, 1, 1);
  layout->addWidget (t_af, 3, 1, 1, 1);
  
  layout->addWidget (l_preflop_cap, 4, 0, 1, 1);
  layout->addWidget (l_vpip, 5, 0, 1, 1);
  layout->addWidget (t_vpip, 5, 1, 1, 1);
  layout->addWidget (l_pfr, 6, 0, 1, 1);
  layout->addWidget (t_pfr, 6, 1, 1, 1);
  
  layout->addWidget (l_flop_cap, 7, 0, 1, 1);
  layout->addWidget (l_conbet, 8, 0, 1, 1);
  layout->addWidget (t_conbet, 8, 1, 1, 1);
  layout->addWidget (l_faf, 9, 0, 1, 1);
  layout->addWidget (t_faf, 9, 1, 1, 1);
  
  layout->addWidget (l_turn_cap, 10, 0, 1, 1);
  layout->addWidget (l_tseen, 11, 0, 1, 1);
  layout->addWidget (t_tseen, 11, 1, 1, 1);
  layout->addWidget (l_taf, 12, 0, 1, 1);
  layout->addWidget (t_taf, 12, 1, 1, 1);
  
  layout->addWidget (l_river_cap, 13, 0, 1, 1);
  layout->addWidget (l_rseen, 14, 0, 1, 1);
  layout->addWidget (t_rseen, 14, 1, 1, 1);
  layout->addWidget (l_raf, 15, 0, 1, 1);
  layout->addWidget (t_raf, 15, 1, 1, 1);
  
  layout->addWidget (l_showdown_cap, 16, 0, 1, 1);
  layout->addWidget (l_wts, 17, 0, 1, 1);
  layout->addWidget (t_wts, 17, 1, 1, 1);
  layout->addWidget (l_sdw, 18, 0, 1, 1);
  layout->addWidget (t_sdw, 18, 1, 1, 1);
  
  connect (c_table, SIGNAL (activated (int)), this, SLOT (changedTableSize (int)));
}

RightPart::~RightPart ()
{}

void RightPart::setupProps (const QString pname, const PlayerStat stat)
{
  l_name->setText (pname);
  t_obh->setText (QString::number (stat.observed_hands));
  t_vpip->setText (QString ("%1 %").arg (stat.VPIP ()));
  t_pfr->setText (QString ("%1 %").arg (stat.preflop_raise ()));
  t_faf->setText (QString::number (stat.AF_flop (), 'g', 3));
  t_conbet->setText (QString ("%1 %").arg (stat.contibet ()));
  t_tseen->setText (QString ("%1 %").arg (stat.seen_turn ()));
  t_taf->setText (QString::number (stat.AF_turn (), 'g', 3));
  t_rseen->setText (QString ("%1 %").arg (stat.seen_river ()));
  t_raf->setText (QString::number (stat.AF_river (), 'g', 3));
  t_af->setText (QString::number (stat.AF_ave (), 'g', 3));
  t_wts->setText (QString ("%1 %").arg (stat.wtShowdown ()));
  t_sdw->setText (QString ("%1 %").arg (stat.wonShowdown ()));
}

int RightPart::desiredTableSize ()
{
  return c_table->currentIndex ();
}

void RightPart::changedTableSize (int index)
{
  emit requestStat ();
}

MainWindow::MainWindow (QWidget *parent)
  : QMainWindow (parent), player ()
{
  settings = new QSettings ("Pik-9", "PokerTH-Tracker");  
  resize (1024, 786);
  setWindowTitle (tr ("PokerTH Tracker"));
  
  /* If the linux directory for shared files exists, load icon from it. */
  QDir sharedData ("/usr/local/share/PokerTH_tracker");
  if (sharedData.exists ("PokerTH_Tracker.png"))  {
    setWindowIcon (QIcon ("/usr/local/share/PokerTH_tracker/PokerTH_Tracker.png"));
  } else  {
    /* Otherwise try to load from current directory.
     * This is necessary for the windows cross build. */
    setWindowIcon (QIcon ("PokerTH_Tracker.png"));
  }
  
  splitter = new QSplitter (Qt::Horizontal);

  /* Move window to screen center. */
  QRect geom = QApplication::desktop ()->screenGeometry ();
  move ((geom.width () - width ()) / 2, (geom.height () - height ()) / 2);

  QMenu *fmenu = new QMenu (tr ("&File"));
  fmenu->addAction (tr ("&Refresh"), this, SLOT (refresh ()), QKeySequence::Refresh);
  fmenu->addAction (tr ("&Quit"), qApp, SLOT (quit ()));
  menuBar ()->addMenu (fmenu);

  QMenu *fhelp = new QMenu (tr ("&Help"));
  fhelp->addAction (tr ("About Qt"), this, SLOT (clickedAboutQT ()));
  fhelp->addAction (tr ("About"), this, SLOT (clickedAbout ()));
  menuBar ()->addMenu (fhelp);

  lp = new LeftPart (settings);
  rp = new RightPart ();
  splitter->addWidget (lp);
  splitter->addWidget (rp);
  splitter->setStretchFactor (0, 1);
  splitter->setStretchFactor (1, 1);
  setCentralWidget (splitter);

  connect (lp, SIGNAL (changedDirectory ()), this, SLOT (refresh ()));
  connect (lp->getListWidget (), SIGNAL (currentTextChanged (const QString)), this, SLOT (showPlayerStats (const QString)));
  connect (rp, SIGNAL (requestStat ()), this, SLOT (showPlayerStats ()));

  refresh ();
}

MainWindow::~MainWindow ()
{}

void MainWindow::refresh ()
{
  bool success = false;
  allPlayers.clear ();
  for (uint32_t ii = 0; ii < 3; ++ii)  {
    player[ii].clear ();
  }
  QDir dir (lp->getFilePath ());
  QStringList files = dir.entryList (QStringList ("*.pdb"));
  QSqlDatabase db = QSqlDatabase::addDatabase ("QSQLITE", "Logfile");
  QSqlQuery qu (db);
  for (int ii = 0; ii < files.count (); ++ii)  {
    db.setDatabaseName (dir.absoluteFilePath (files.at (ii)));
    success = db.open ();
    if (!success)  {
      qDebug () << "Error while loading database: " << db.lastError ().text ();
      qDebug () << "Available SQL drivers: " << QSqlDatabase::drivers ();
      break;
    }
    
    QSqlQuery qu (db);
    if (!qu.exec (
      "SELECT Action.BeRo, Player.Player, Action.Action, Action.Amount "
      "FROM Action, Player "
      "WHERE Action.UniqueGameID = Player.UniqueGameID AND Action.Player = Player.Seat"))
    {
      success = false;
      qDebug () << "Error while executing query: " << qu.lastError ().text ();
      break;
    }

    /* The interesting stuff happens here: */
    
    /* Which actions were performed during the bet rounds?
     *   Bit 1: fold
     *   Bit 2: check, call
     *   Bit 3: bet, raise
     *   Bit 4: is all-in
     * 
     * Showdown:
     *   Bit 1: Went to showdown
     *   Bit 2: Won showdown
     */
    std::map<QString, RoundAct> hand_agg;
    unsigned int current_bet = 0, current_round = 0;
    while (qu.next ())  {
      unsigned int round = qu.value (0).toInt ();
      QString player_name = qu.value (1).toString ();
      QString player_action = qu.value (2).toString ();
      unsigned int amount = qu.value (3).toInt ();

      if (round > current_round)  {
        /* A new round. */
        current_bet = 0;
        current_round = round;
      }

      if (player_action == "starts as dealer")  {
        /* New game begins. */
        current_bet = current_round = 0;
        /* Evaluate the stats from the last game. */
        for (std::map<QString, RoundAct>::iterator it = hand_agg.begin (); it != hand_agg.end (); ++it)  {
          /* Determine which map should be used. */
          uint32_t mapToUse;
          if (hand_agg.size () > 6)  {
            /* Fullring */
            mapToUse = 0;
          } else if (hand_agg.size () > 2)  {
            /* Shorthand */
            mapToUse = 1;
          } else  {
            /* Heads-Up */
            mapToUse = 2;
          }
  
          /* Preflop */
          player[mapToUse][it->first].observed_hands++;
          if ((it->second[0] & 6) == 2)  {
            player[mapToUse][it->first].pf_calls++;
          }
          if (it->second[0] & 4)  {
            player[mapToUse][it->first].pf_open++;
          }

          /* Flop */
          if (it->second[1] & 1)  {
            player[mapToUse][it->first].f_fold++;
          }
          if (it->second[1] & 2)  {
            player[mapToUse][it->first].f_check_call++;
          }
          if (it->second[1] & 4)  {
            player[mapToUse][it->first].f_bet++;
            if (it->second[0] & it->second[1] & 4)  {
              player[mapToUse][it->first].f_contibet++;
            }
          }
          if (it->second[1] > 0)  {
            player[mapToUse][it->first].f_seen++;
          }
          
          /* Turn */
          if (it->second[2] & 1)  {
            player[mapToUse][it->first].t_fold++;
          }
          if (it->second[2] & 2)  {
            player[mapToUse][it->first].t_check_call++;
          }
          if (it->second[2] & 4)  {
            player[mapToUse][it->first].t_bet++;
          }
          if (it->second[2] > 0)  {
            player[mapToUse][it->first].t_seen++;
          }

          /* River */
          if (it->second[3] & 1)  {
            player[mapToUse][it->first].r_fold++;
          }
          if (it->second[3] & 2)  {
            player[mapToUse][it->first].r_check_call++;
          }
          if (it->second[3] & 4)  {
            player[mapToUse][it->first].r_bet++;
          }
          if (it->second[3] > 0)  {
            player[mapToUse][it->first].r_seen++;
          }

          /* Showdown */
          if (it->second[4] > 0)  {
            player[mapToUse][it->first].sd_seen++;
            if (it->second[4] & 2)  {
              player[mapToUse][it->first].sd_won++;
            }
          }
        }
        hand_agg.clear ();
      }

      if (player_action == "posts big blind")  {
        current_bet = amount;
      }

      if (player_action == "bets")  {
        hand_agg[player_name][round] |= 4;
        current_bet = amount;
      }

      if ((player_action == "calls") || (player_action == "checks"))  {
        hand_agg[player_name][round] |= 2;
      }

      if (player_action == "folds")  {
        hand_agg[player_name][round] |= 1;
      }

      if (player_action == "is all in with")  {
        if (amount > current_bet)  {
          hand_agg[player_name][round] |= 4;
        } else {
          hand_agg[player_name][round] |= 2;
        }
        for (int ii = round + 1; ii < 5; ++ii)  {
          hand_agg[player_name][ii] |= 8;
        }
      }

      /* Showdown */
      if (round == 4)  {
        hand_agg[player_name][4] |= 1;
        if (player_action == "wins")  {
          hand_agg[player_name][4] |= 2;
        }
      }
      /***************************************************/
    }
    db.close ();
  }
  
  if (success)  {
    QListWidget *overview = lp->getListWidget ();
    overview->clear ();
    
    /* Create the map of players in any situation. */
    for (uint32_t ii = 0; ii < 3; ++ii)  {
      for (std::map<QString, PlayerStat>::iterator it = player[ii].begin (); it != player[ii].end (); ++it)  {
        allPlayers[it->first] += it->second;
      }
    }
    
    for (std::map<QString, PlayerStat>::iterator it = allPlayers.begin (); it != allPlayers.end (); ++it)  {
      overview->addItem (it->first);
    }
    settings->setValue ("defaultPath", lp->getFilePath ());
    statusBar ()->showMessage (tr ("Opened %1 files from %2").arg (files.count ()).arg (dir.absolutePath ()));
  } else  {
    statusBar ()->showMessage (tr ("There were errors while loading files from %1!").arg (dir.absolutePath ()));
  }
}

void MainWindow::showPlayerStats ()
{
  QListWidgetItem *li = lp->getListWidget ()->currentItem ();
  if (li)  {
    showPlayerStats (li->text ());
  }
}

void MainWindow::showPlayerStats (const QString pname)
{
  if (!pname.isEmpty ())  {
    if (rp->desiredTableSize () > 2)  {
      rp->setupProps (pname, allPlayers[pname]);
    } else  {
      rp->setupProps (pname, player[rp->desiredTableSize ()][pname]);
    }
  }
}

void MainWindow::clickedAboutQT ()
{
  QMessageBox::aboutQt (this);
}

void MainWindow::clickedAbout ()
{
  QString msg = tr ("PokerTH Tracker - A simple poker statistics tracker for PokerTH\n");
  msg += "(C) 2015-2016 Daniel Steinhauer (9 of Spades)\n";
  msg += tr ("This software is published under the termns of the GNU General Public License Version 3.");
  QMessageBox::about (this, tr ("PokerTH Tracker"), msg);
}