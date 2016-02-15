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
#include "global.hpp"

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

LeftPart::LeftPart (QWidget *parent)
  : QWidget (parent)
{
  layout = new QGridLayout (this);
  t_path = new QLineEdit ();
  t_path->setReadOnly (true);
  b_search = new QPushButton (tr ("Search..."));
  l_players = new QListWidget ();
  l_players->setToolTip (tr ("Double click on player to add to multi player view."));
  lopen = new QFileDialog ();
  lopen->setFileMode (QFileDialog::Directory);
  layout->addWidget (t_path, 0, 0, 1, 1);
  layout->addWidget (b_search, 0, 1, 1, 1);
  layout->addWidget (l_players, 1, 0, 1, 2);

  t_path->setText (Global::getInstance ()->getLogDir ());

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
  
  l_preflop = new QLabel (tr ("<b>Preflop</b>"));
  l_postflop = new QLabel (tr ("<b>Postflop</b>"));
  l_winnings = new QLabel (tr ("<b>Winnings</b>"));
  
  l_vpip = new QLabel (tr ("VP$IP:"));
  l_vpip->setCursor (Qt::WhatsThisCursor);
  l_vpip->setToolTip (tr ("Percantage of hands the player voluntarily invested money in."));
  l_pfr = new QLabel (tr ("Preflop Raise:"));
  l_pfr->setCursor (Qt::WhatsThisCursor);
  l_pfr->setToolTip (tr ("Percentage of hands the player raised with preflop."));
  l_3bet = new QLabel (tr ("3bet:"));
  
  l_conti = new QLabel (tr ("Continuation Bet:"));
  l_fconti = new QLabel (tr ("Folded to Contibet:"));
  l_tbet = new QLabel (tr ("Turn Contibet:"));
  l_ftbet = new QLabel (tr ("Folded to Turn C-Bet:"));
  
  l_fnbet = new QLabel (tr ("Folded to n-Bets:"));
  l_cr = new QLabel (tr ("Check-Raise:"));
  
  l_aAF = new QLabel (tr ("Average AF:"));
  l_fAF = new QLabel (tr ("Flop AF:"));
  l_tseen = new QLabel (tr ("Turn seen:"));
  l_tseen->setCursor (Qt::WhatsThisCursor);
  l_tseen->setToolTip (tr ("With how many hands the player went to the Turn."));
  l_tAF = new QLabel (tr ("Turn AF:"));
  l_rseen = new QLabel (tr ("River seen:"));
  l_rseen->setCursor (Qt::WhatsThisCursor);
  l_rseen->setToolTip (tr ("With how many hands the player went to the River."));
  l_rAF = new QLabel (tr ("River AF:"));
  
  l_wts = new QLabel (tr ("Went to Showdown:"));
  l_wts->setCursor (Qt::WhatsThisCursor);
  l_wts->setToolTip (tr ("With how many hands the player went to Showdown <b>after the flop</b>."));
  l_wwsf = new QLabel (tr ("W$WSF:"));
  l_wsd = new QLabel (tr ("W$SD:"));
  l_wsd->setCursor (Qt::WhatsThisCursor);
  l_wsd->setToolTip (tr ("How often the player wins by going to Showdown."));
  
  t_obh = new QLabel ("0");
  t_vpip = new QLabel ("0");
  t_pfr = new QLabel ("0");
  t_3bet = new QLabel ("0");
  t_conti = new QLabel ("0");
  t_fconti = new QLabel ("0");
  t_tbet = new QLabel ("0");
  t_ftbet = new QLabel ("0");
  t_fnbet = new QLabel ("0");
  t_cr = new QLabel ("0");
  t_aAF = new QLabel ("0");
  t_fAF = new QLabel ("0");
  t_tseen = new QLabel ("0");
  t_tAF = new QLabel ("0");
  t_rseen = new QLabel ("0");
  t_rAF = new QLabel ("0");
  t_wts = new QLabel ("0");
  t_wwsf = new QLabel ("0");
  t_wsd = new QLabel ("0");

  layout->addWidget (l_name, 0, 0, 1, 2);
  layout->addWidget (l_tsize, 1, 0, 1, 1);
  layout->addWidget (c_table, 1, 1, 1, 1);
  layout->addWidget (l_obh, 2, 0, 1, 1);
  layout->addWidget (t_obh, 2, 1, 1, 1);
  
  layout->addWidget (l_preflop, 3, 0, 1, 2);
  
  layout->addWidget (l_vpip, 4, 0, 1, 1);
  layout->addWidget (t_vpip, 4, 1, 1, 1);
  layout->addWidget (l_pfr, 5, 0, 1, 1);
  layout->addWidget (t_pfr, 5, 1, 1, 1);
  layout->addWidget (l_3bet, 6, 0, 1, 1);
  layout->addWidget (t_3bet, 6, 1, 1, 1);
  
  layout->addWidget (l_postflop, 7, 0, 1, 2);
  
  layout->addWidget (l_conti, 8, 0, 1, 1);
  layout->addWidget (t_conti, 8, 1, 1, 1);
  layout->addWidget (l_fconti, 9, 0, 1, 1);
  layout->addWidget (t_fconti, 9, 1, 1, 1);
  layout->addWidget (l_tbet, 10, 0, 1, 1);
  layout->addWidget (t_tbet, 10, 1, 1, 1);
  layout->addWidget (l_ftbet, 11, 0, 1, 1);
  layout->addWidget (t_ftbet, 11, 1, 1, 1);
  layout->addWidget (l_fnbet, 12, 0, 1, 1);
  layout->addWidget (t_fnbet, 12, 1, 1, 1);
  layout->addWidget (l_cr, 13, 0, 1, 1);
  layout->addWidget (t_cr, 13, 1, 1, 1);
  layout->addWidget (l_aAF, 14, 0, 1, 1);
  layout->addWidget (t_aAF, 14, 1, 1, 1);
  layout->addWidget (l_fAF, 15, 0, 1, 1);
  layout->addWidget (t_fAF, 15, 1, 1, 1);
  layout->addWidget (l_tseen, 16, 0, 1, 1);
  layout->addWidget (t_tseen, 16, 1, 1, 1);
  layout->addWidget (l_tAF, 17, 0, 1, 1);
  layout->addWidget (t_tAF, 17, 1, 1, 1);
  layout->addWidget (l_rseen, 18, 0, 1, 1);
  layout->addWidget (t_rseen, 18, 1, 1, 1);
  layout->addWidget (l_rAF, 19, 0, 1, 1);
  layout->addWidget (t_rAF, 19, 1, 1, 1);
  
  layout->addWidget (l_winnings, 20, 0, 1, 2);
  
  layout->addWidget (l_wts, 21, 0, 1, 1);
  layout->addWidget (t_wts, 21, 1, 1, 1);
  layout->addWidget (l_wwsf, 22, 0, 1, 1);
  layout->addWidget (t_wwsf, 22, 1, 1, 1);
  layout->addWidget (l_wsd, 23, 0, 1, 1);
  layout->addWidget (t_wsd, 23, 1, 1, 1);
  
  connect (c_table, SIGNAL (activated (int)), this, SLOT (changedTableSize (int)));
}

RightPart::~RightPart ()
{}

void RightPart::setupProps (const QString pname, Statistics* ps)
{
  PlayerStat stat = ps->getPlayerStat (pname, (tableSize) c_table->currentIndex ());
  l_name->setText (pname);
  t_obh->setText (QString::number (stat.observed_hands));
  t_vpip->setText (QString ("%1 %").arg (stat.VPIP (), 0, 'g', 3));
  t_pfr->setText (QString ("%1 %").arg (stat.preflop_raise (), 0, 'g', 3));
  t_3bet->setText (QString ("%1 %").arg (stat.bet3_preflop (), 0, 'g', 3));
  t_conti->setText (QString ("%1 %").arg (stat.F_contibet (), 0, 'g', 3));
  t_fconti->setText (QString ("%1 %").arg (stat.folded_conbet (), 0, 'g', 3));
  t_tbet->setText (QString ("%1 %").arg (stat.T_contibet (), 0, 'g', 3));
  t_ftbet->setText (QString ("%1 %").arg (stat.folded_turnbet (), 0, 'g', 3));
  t_fnbet->setText (QString ("%1 %").arg (stat.folded_nbet (), 0, 'g', 3));
  t_cr->setText (QString ("%1 %").arg (stat.checkraise_prop (), 0, 'g', 3));
  t_aAF->setText (QString::number (stat.AF_ave (), 'g', 3));
  t_fAF->setText (QString::number (stat.AF (FLOP), 'g', 3));
  t_tseen->setText (QString ("%1 %").arg (stat.seen_round (TURN), 0, 'g', 3));
  t_tAF->setText (QString::number (stat.AF (TURN), 'g', 3));
  t_rseen->setText (QString ("%1 %").arg (stat.seen_round (RIVER), 0, 'g', 3));
  t_rAF->setText (QString::number (stat.AF (RIVER), 'g', 3));
  t_wts->setText (QString ("%1 %").arg (stat.wtShowdown (), 0, 'g', 3));
  t_wwsf->setText (QString ("%1 %").arg (stat.wonPostflop (), 0, 'g', 3));
  t_wsd->setText (QString ("%1 %").arg (stat.wonShowdown (), 0, 'g', 3));
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
  : QMainWindow (parent)
{
  resize (1024, 786);
  setWindowTitle (tr ("PokerTH Tracker"));
  
 setWindowIcon (QIcon (Global::getInstance ()->getDataDir () + "/PokerTH_Tracker.png"));
  
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

  stat = new Statistics ();
  
  lp = new LeftPart ();
  rp = new RightPart ();
  ap = new AnaWidget ();
  mv = new MultiView (stat);
  splitter->addWidget (lp);
  splitter->addWidget (rp);
  splitter->addWidget (ap);
  setCentralWidget (splitter);

  connect (lp, SIGNAL (changedDirectory ()), this, SLOT (refresh ()));
  connect (lp->getListWidget (), SIGNAL (currentTextChanged (const QString)), this, SLOT (showPlayerStats (const QString)));
  connect (lp->getListWidget (), SIGNAL (itemDoubleClicked (QListWidgetItem*)), this, SLOT (addToMultiview (QListWidgetItem*)));
  connect (rp, SIGNAL (requestStat ()), this, SLOT (showPlayerStats ()));

  refresh ();
}

MainWindow::~MainWindow ()
{}

void MainWindow::refresh ()
{
  uint32_t cf = 0;
  if (stat->loadStatistics (lp->getFilePath (), &cf))  {
    QListWidget *overview = lp->getListWidget ();
    overview->clear ();
    overview->addItems (stat->getPlayerNames ());
    Global::getInstance ()->setLogDir (lp->getFilePath ());
    mv->writeTable ();
    statusBar ()->showMessage (tr ("Opened %1 files from %2").arg (cf).arg (lp->getFilePath ()));
  } else  {
    statusBar ()->showMessage (tr ("There were errors while loading files from %1!").arg (lp->getFilePath ()));
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
    rp->setupProps (pname, stat);
  }
}

void MainWindow::addToMultiview (QListWidgetItem* item)
{
  mv->addPlayer (item->text ());
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