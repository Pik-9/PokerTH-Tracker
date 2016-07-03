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

#include "notes_stars.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTextEdit>

NotesStarsWidget::NotesStarsWidget (QWidget* parent)
  : QWidget (parent)
{
  layout = new QGridLayout (this);
  l_stars = new QLabel (tr ("Stars:"));
  s_stars = new QSlider (Qt::Horizontal);
  s_stars->setMinimum (0);
  s_stars->setMaximum (5);
  s_stars->setTickPosition (QSlider::TicksBelow);
  s_stars->setTickInterval (1);
  t_notes = new QTextEdit ();
  b_appl = new QPushButton (tr ("Apply"));
  b_disc = new QPushButton (tr ("Discard"));
  
  b_appl->setEnabled (false);
  b_disc->setEnabled (false);
  
  layout->addWidget (l_stars, 0, 0, 1, 2);
  layout->addWidget (s_stars, 1, 0, 1, 2);
  layout->addWidget (t_notes, 2, 0, 1, 2);
  layout->addWidget (b_disc, 3, 0, 1, 1);
  layout->addWidget (b_appl, 3, 1, 1, 1);
  
  connect (b_appl, SIGNAL (clicked ()), this, SLOT (clickedApply ()));
  connect (b_disc, SIGNAL (clicked ()), this, SLOT (clickedDiscard ()));
}

NotesStarsWidget::~NotesStarsWidget ()
{

}

void NotesStarsWidget::clickedApply ()
{

}

void NotesStarsWidget::clickedDiscard ()
{

}