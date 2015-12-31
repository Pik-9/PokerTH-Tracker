#include "mainwindow.h"

#include <QListWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
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

/* Dirty workaround! */
struct RoundAct
{
  uint8_t data[5];
  uint8_t& operator[] (unsigned int id) { return data[id]; }
};

LeftPart::LeftPart (QWidget *parent)
  : QWidget (parent)
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

  t_path->setText (QDir::homePath () + "/.pokerth/log-files/");

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
  l_obh = new QLabel (tr ("Observed hands:"));
  l_vpip = new QLabel (tr ("VP$IP:"));
  l_pfr = new QLabel (tr ("Preflop Raise:"));
  l_faf = new QLabel (tr ("Flop Aggression Factor:"));
  l_conbet = new QLabel (tr ("Continuation bet:"));
  l_taf = new QLabel (tr ("Turn Aggression Factor:"));
  l_raf = new QLabel (tr ("River Aggression Factor:"));
  l_wts = new QLabel (tr ("Went to Showdown:"));
  l_sdw = new QLabel (tr ("Won Showdown:"));
  t_obh = new QLabel ("0");
  t_vpip = new QLabel ("0");
  t_pfr = new QLabel ("0");
  t_faf = new QLabel ("0");
  t_conbet = new QLabel ("0");
  t_taf = new QLabel ("0");
  t_raf = new QLabel ("0");
  t_wts = new QLabel ("0");
  t_sdw = new QLabel ("0");

  layout->addWidget (l_name, 0, 0, 1, 2);
  layout->addWidget (l_obh, 1, 0, 1, 1);
  layout->addWidget (t_obh, 1, 1, 1, 1);
  layout->addWidget (l_vpip, 2, 0, 1, 1);
  layout->addWidget (t_vpip, 2, 1, 1, 1);
  layout->addWidget (l_pfr, 3, 0, 1, 1);
  layout->addWidget (t_pfr, 3, 1, 1, 1);
  layout->addWidget (l_conbet, 4, 0, 1, 1);
  layout->addWidget (t_conbet, 4, 1, 1, 1);
  layout->addWidget (l_faf, 5, 0, 1, 1);
  layout->addWidget (t_faf, 5, 1, 1, 1);
  layout->addWidget (l_taf, 6, 0, 1, 1);
  layout->addWidget (t_taf, 6, 1, 1, 1);
  layout->addWidget (l_raf, 7, 0, 1, 1);
  layout->addWidget (t_raf, 7, 1, 1, 1);
  layout->addWidget (l_wts, 8, 0, 1, 1);
  layout->addWidget (t_wts, 8, 1, 1, 1);
  layout->addWidget (l_sdw, 9, 0, 1, 1);
  layout->addWidget (t_sdw, 9, 1, 1, 1);
}

RightPart::~RightPart ()
{}

void RightPart::setupProps (const QString pname, const PlayerStat stat)
{
  l_name->setText (pname);
  t_obh->setText (QString::number (stat.observed_hands));

  double vpip = 100.0 * (stat.pf_calls + stat.pf_open) / stat.observed_hands;
  double pfr = 100.0 * stat.pf_open / stat.observed_hands;
  double faf = (double) stat.f_bet / (stat.f_check_call + stat.f_fold);
  double contibet = 100.0 * stat.f_contibet / stat.pf_open;
  double taf = (double) stat.t_bet / (stat.t_check_call + stat.t_fold);
  double raf = (double) stat.r_bet / (stat.r_check_call + stat.r_fold);
  double wts = 100.0 * stat.sd_seen / stat.observed_hands;
  double sdw = 100.0 * stat.sd_won / stat.sd_seen;

  t_vpip->setText (QString ("%1 %").arg (vpip, 1));
  t_pfr->setText (QString ("%1 %").arg (pfr, 1));
  t_faf->setText (QString::number (faf, 'g', 3));
  t_conbet->setText (QString ("%1 %").arg (contibet, 1));
  t_taf->setText (QString::number (taf, 'g', 3));
  t_raf->setText (QString::number (raf, 'g', 3));
  t_wts->setText (QString ("%1 %").arg (wts, 1));
  t_sdw->setText (QString ("%1 %").arg (sdw, 1));
}

MainWindow::MainWindow (QWidget *parent)
  : QMainWindow (parent), player ()
{
  resize (1024, 786);
  setWindowTitle (tr ("PokerTH Tracker"));
  setWindowIcon (QIcon ("/usr/local/share/PokerTH_tracker/PokerTH_Tracker.png"));
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

  lp = new LeftPart ();
  rp = new RightPart ();
  splitter->addWidget (lp);
  splitter->addWidget (rp);
  splitter->setStretchFactor (0, 1);
  splitter->setStretchFactor (1, 1);
  setCentralWidget (splitter);

  connect (lp, SIGNAL (changedDirectory ()), this, SLOT (refresh ()));
  connect (lp->getListWidget (), SIGNAL (currentTextChanged (const QString)), this, SLOT (showPlayerStats (const QString)));

  refresh ();
}

MainWindow::~MainWindow ()
{}

void MainWindow::refresh ()
{
  player.clear ();
  QDir dir (lp->getFilePath ());
  QStringList files = dir.entryList (QStringList ("*.pdb"));
  for (int ii = 0; ii < files.count (); ++ii)  {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName (dir.absoluteFilePath (files.at (ii)));
    db.open ();
    QSqlQuery qu (db);
    qu.exec ("SELECT Action.BeRo, Player.Player, Action.Action, Action.Amount FROM Action, Player WHERE Action.UniqueGameID = Player.UniqueGameID AND Action.Player = Player.Seat");

    /* The interesting stuff happens here: */
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
        for (std::map<QString, RoundAct>::iterator it = hand_agg.begin (); it != hand_agg.end (); ++it)  {
          /* Preflop */
          player[it->first].observed_hands++;
          if ((it->second[0] & 6) == 2)  {
            player[it->first].pf_calls++;
          }
          if (it->second[0] & 4)  {
            player[it->first].pf_open++;
          }

          /* Flop */
          if (it->second[1] & 1)  {
            player[it->first].f_fold++;
          }
          if (it->second[1] & 2)  {
            player[it->first].f_check_call++;
          }
          if (it->second[1] & 4)  {
            player[it->first].f_bet++;
            if (it->second[0] & it->second[1] & 4)  {
              player[it->first].f_contibet++;
            }
          }
          if (it->second[1] > 0)  {
            player[it->first].f_seen++;
          }

          /* Turn */
          if (it->second[2] & 1)  {
            player[it->first].t_fold++;
          }
          if (it->second[2] & 2)  {
            player[it->first].t_check_call++;
          }
          if (it->second[2] & 4)  {
            player[it->first].t_bet++;
          }
          if (it->second[2] > 0)  {
            player[it->first].t_seen++;
          }

          /* River */
          if (it->second[3] & 1)  {
            player[it->first].r_fold++;
          }
          if (it->second[3] & 2)  {
            player[it->first].r_check_call++;
          }
          if (it->second[3] & 4)  {
            player[it->first].r_bet++;
          }
          if (it->second[3] > 0)  {
            player[it->first].r_seen++;
          }

          /* Showdown */
          if (it->second[4] > 0)  {
            player[it->first].sd_seen++;
            if (it->second[4] & 2)  {
              player[it->first].sd_won++;
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

  QListWidget *overview = lp->getListWidget ();
  overview->clear ();
  for (std::map<QString, PlayerStat>::iterator it = player.begin (); it != player.end (); ++it)  {
    overview->addItem (it->first);
  }
  statusBar ()->showMessage (tr ("Opened %1 files from %2").arg (files.count ()).arg (dir.absolutePath ()));
}

void MainWindow::showPlayerStats (const QString pname)
{
  rp->setupProps (pname, player[pname]);
}

void MainWindow::clickedAboutQT ()
{
  QMessageBox::aboutQt (this);
}

void MainWindow::clickedAbout ()
{
  QMessageBox::about (this, tr ("PokerTH Tracker"), tr ("PokerTH Tracker - A simple poker statistics tracker for PokerTH\n(C) 2015-2016 Daniel Steinhauer (9 of Spades)"));
}