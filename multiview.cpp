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

#include "multiview.hpp"
#include "global.hpp"
#include "analysis_widget.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QTableWidget>
#include <QShowEvent>

MultiView::MultiView (Statistics* PInformation, QWidget* parent)
  : QWidget (parent), stat (PInformation)
{
  setWindowTitle (tr ("Multiplayer view"));
  
  Global* glset = Global::getInstance ();
  glset->getMultiViewGeom (this);
  setWindowIcon (QIcon (":/PokerTH_Tracker.png"));
  
  layout = new QGridLayout (this);
  l_tsize = new QLabel (tr ("Table size:"));
  c_tsize = new QComboBox ();
  c_tsize->addItem (tr ("Fullring (7 - 10 players)"));
  c_tsize->addItem (tr ("Shorthand (3 - 6 players)"));
  c_tsize->addItem (tr ("Heads-Up (2 players)"));
  c_tsize->addItem (tr ("Any"));
  c_tsize->setCurrentIndex (3);
  table = new QTableWidget ();
  b_ok = new QPushButton (tr ("OK"));
  b_clear = new QPushButton (tr ("Clear"));
  b_rem = new QPushButton (tr ("Remove player"));
  b_rem->setEnabled (false);
  
  layout->addWidget (l_tsize, 0, 0, 1, 5);
  layout->addWidget (c_tsize, 0, 5, 1, 5);
  layout->addWidget (table, 1, 0, 1, 10);
  layout->addWidget (b_ok, 2, 2, 1, 2);
  layout->addWidget (b_rem, 2, 4, 1, 2);
  layout->addWidget (b_clear, 2, 6, 1, 2);
  
  connect (b_ok, SIGNAL (clicked ()), this, SLOT (hide ()));
  connect (b_clear, SIGNAL (clicked ()), this, SLOT (clickedClear ()));
  connect (b_rem, SIGNAL (clicked ()), this, SLOT (clickedRemove ()));
  connect (c_tsize, SIGNAL (currentIndexChanged (int)), this, SLOT (updateTable (int)));
  connect (table, SIGNAL (itemSelectionChanged ()), this, SLOT (selectPlayers ()));
}

MultiView::~MultiView ()
{}

void MultiView::showEvent (QShowEvent* event)
{
  QWidget::showEvent (event);
  writeTable ();
}

void MultiView::writeTable ()
{
  table->clear ();
  table->setColumnCount (observedPlayers.size () + 1);
  table->setRowCount (21);
  
  tableSize ts = (tableSize) c_tsize->currentIndex ();
  
  QString pchar[10] = {
    tr ("No data..."),
    tr ("Rock"),
    tr ("Weak-Passive"),
    tr ("Fish"),
    tr ("Donkey"),
    tr ("Shark"),
    tr ("Loose-Aggressive"),
    tr ("Dump Loose-Aggressive"),
    tr ("Fool"),
    tr ("Maniac")
  };
  
  QString desc[21] = {
    tr ("Name:"),
    tr ("Observed hands:"),
    tr ("VP$IP:"),
    tr ("Preflop Raise:"),
    tr ("3bet:"),
    tr ("Continuation bet:"),
    tr ("Folded to Contibet:"),
    tr ("2nd barrel:"),
    tr ("Folded to 2nd barrel:"),
    tr ("Folded to n-Bet:"),
    tr ("Check-Raise:"),
    tr ("Average AF:"),
    tr ("Flop AF:"),
    tr ("Turn seen:"),
    tr ("Turn AF:"),
    tr ("River seen:"),
    tr ("River AF:"),
    tr ("Went to Showdown:"),
    tr ("W$WSF:"),
    tr ("W$SD:"),
    tr ("Characteristic:")
  };
  
  for (uint32_t ic = 0; ic < table->columnCount (); ++ic)  {
    for (uint32_t ir = 0; ir < 21; ++ir)  {
      QTableWidgetItem *ti = new QTableWidgetItem ();
      QFont boldText;
      boldText.setBold (true);
      if (ic)  {
        QString pname = observedPlayers[ic - 1];
        switch (ir)  {
          case 0:  {
            ti->setText (QString ("%1").arg (pname));
            ti->setFont (boldText);
            break;
          }
          
          case 1:  {
            ti->setText (QString::number (stat->getPlayerStat(pname, ts).observed_hands));
            break;
          }
          
          case 2:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat(pname, ts).VPIP (), 0, 'g', 3));
            break;
          }
          
          case 3:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat(pname, ts).preflop_raise (), 0, 'g', 3));
            break;
          }
          
          case 4:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).bet3_preflop (), 0, 'g', 3));
            break;
          }
          
          case 5:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).F_contibet (), 0, 'g', 3));
            break;
          }
          
          case 6:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).folded_conbet (), 0, 'g', 3));
            break;
          }
          
          case 7:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).T_contibet (), 0, 'g', 3));
            break;
          }
          
          case 8:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).folded_turnbet (), 0, 'g', 3));
            break;
          }
          
          case 9:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).folded_nbet (), 0, 'g', 3));
            break;
          }
          
          case 10:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).checkraise_prop (), 0, 'g', 3));
            break;
          }
          
          case 11:  {
            ti->setText (QString ("%1").arg (stat->getPlayerStat (pname, ts).AF_ave (), 0, 'g', 3));
            break;
          }
          
          case 12:  {
            ti->setText (QString ("%1").arg (stat->getPlayerStat (pname, ts).AF (FLOP), 0, 'g', 3));
            break;
          }
          
          case 13:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).seen_round (TURN), 0, 'g', 3));
            break;
          }
          
          case 14:  {
            ti->setText (QString ("%1").arg (stat->getPlayerStat (pname, ts).AF (TURN), 0, 'g', 3));
            break;
          }
          
          case 15:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).seen_round (RIVER), 0, 'g', 3));
            break;
          }
          
          case 16:  {
            ti->setText (QString ("%1").arg (stat->getPlayerStat (pname, ts).AF (RIVER), 0, 'g', 3));
            break;
          }
          
          case 17:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).wtShowdown (), 0, 'g', 3));
            break;
          }
          
          case 18:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).wonPostflop (), 0, 'g', 3));
            break;
          }
          
          case 19:  {
            ti->setText (QString ("%1 %").arg (stat->getPlayerStat (pname, ts).wonShowdown (), 0, 'g', 3));
            break;
          }
          
          case 20:  {
            ti->setText (pchar[(int) AnaWidget::analyseChar (stat->getPlayerStat (pname, ts), ts)]);
            break;
          }
        }
      } else  {
        ti->setText (QString ("%1").arg (desc[ir]));
        ti->setFont (boldText);
      }
      table->setItem (ir, ic, ti);
    }
  }
  table->resizeColumnsToContents ();
}

void MultiView::updateTable (int index)
{
  writeTable ();
}

void MultiView::selectPlayers ()
{
  b_rem->setEnabled (!table->selectedItems ().isEmpty ());
}

void MultiView::clickedRemove ()
{
  QList<QTableWidgetItem*> si = table->selectedItems ();
  uint32_t min_col = -1, max_col = 0;
  for (uint32_t ii = 0; ii < si.size (); ++ii)  {
    /* column #0 is the description column - ignore it. */
    if (si[ii]->column ())  {
      if ((si[ii]->column () - 1) < min_col)  {
        min_col = si[ii]->column () - 1;
      }
      if ((si[ii]->column () - 1) > max_col)  {
        max_col = si[ii]->column () - 1;
      }
    }
  }
  
  if (min_col <= max_col)  {
    observedPlayers.erase (observedPlayers.begin () + min_col, observedPlayers.begin () + max_col + 1);
  }
  
  writeTable ();
}

void MultiView::clickedClear ()
{
  observedPlayers.clear ();
  writeTable ();
}

void MultiView::addPlayer (const QString pname)
{
  int32_t pos = -1;
  for (uint32_t ii = 0; ii < observedPlayers.size (); ++ii)  {
    if (pname == observedPlayers[ii])  {
      pos = (int32_t) ii;
      break;
    }
  }
  
  /* Only add new player if it doesn't exist yet. */
  if (pos == -1)  {
    observedPlayers.push_back (pname);
  }
  
  show ();
}

void MultiView::removePlayer (const QString pname)
{
  std::vector<QString>::iterator it;
  for (it = observedPlayers.begin (); it != observedPlayers.end (); ++it)  {
    if (pname == *it)  {
      observedPlayers.erase (it);
      break;
    }
  }
  
  writeTable ();
}