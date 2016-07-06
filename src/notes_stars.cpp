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
#include "global.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTextEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QtXml>

const QString NotesStarsWidget::separator = "(!#$%)";

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
  
  layout->addWidget (l_stars, 0, 0, 1, 2);
  layout->addWidget (s_stars, 1, 0, 1, 2);
  layout->addWidget (t_notes, 2, 0, 1, 2);
  layout->addWidget (b_disc, 3, 0, 1, 1);
  layout->addWidget (b_appl, 3, 1, 1, 1);
  
  pth_config = new QDomDocument ();
  
  connect (b_appl, SIGNAL (clicked ()), this, SLOT (clickedApply ()));
  connect (b_disc, SIGNAL (clicked ()), this, SLOT (clickedDiscard ()));
  connect (t_notes, SIGNAL (textChanged ()), this, SLOT (contentEdited ()));
  connect (s_stars, SIGNAL (valueChanged (int)), this, SLOT (contentEdited (int)));
}

NotesStarsWidget::~NotesStarsWidget ()
{}

void NotesStarsWidget::clickedApply ()
{
  entries[currentPlayer].stars = s_stars->value ();
  entries[currentPlayer].notes = t_notes->toPlainText ();
  b_appl->setEnabled (false);
  b_disc->setEnabled (false);
  
  if (pth_config->isNull ())  {
    qDebug () << "The config.xml hasn't been opened properly or has invalid content!"
      << " Won't apply anything!";
    return;
  }
  
  QDomNode PTTn = pth_config->documentElement ().firstChild ().namedItem ("PlayerTooltips");
  QDomElement PTT;
  if (PTTn.isNull ())  {
    /* Create the nonexistent PlayerTooltips node. */
    PTT = pth_config->createElement ("PlayerTooltips");
    PTT.setAttribute ("value", "PlayerTooltips");
    PTT.setAttribute ("type", "list");
    pth_config->documentElement ().firstChild ().appendChild (PTT);
  } else  {
    /* Clear the existing tooltips. */
    QDomNodeList PTTs = PTTn.childNodes ();
    PTT = PTTn.toElement ();
    while (PTTs.size ())  {
      PTTn.removeChild (PTTs.at (0));
    }
  }
  
  /* Insert the new ones. */
  for (std::map<QString, NotesStars>::iterator it = entries.begin (); it != entries.end (); ++it)  {
    QString line = it->first + separator + it->second.notes + separator + QString::number (it->second.stars) + separator;
    QDomElement ptooltip = pth_config->createElement ("PlayerTooltips");
    ptooltip.setAttribute ("value", line);
    PTT.appendChild (ptooltip);
  }
  
  /* Save to config file. */
  QFile conf_file (Global::getInstance ()->getConfigFile ());
  if (!conf_file.open (QIODevice::WriteOnly | QIODevice::Text))  {
    qDebug () << "Error while writing file " << conf_file.fileName ()
      << ": " << conf_file.errorString ();
    return;
  }
  QTextStream tstr (&conf_file);
  tstr << pth_config->toString ();
  conf_file.close ();
}

void NotesStarsWidget::clickedDiscard ()
{
  showPlayerNotes ();
}

/* dump will be dumped! */
void NotesStarsWidget::contentEdited (int dump)
{
  if (!(currentPlayer.isEmpty () || pth_config->isNull ()))  {
    b_appl->setEnabled (true);
    b_disc->setEnabled (true);
  }
}

void NotesStarsWidget::loadNotes ()
{
  b_appl->setEnabled (false);
  b_disc->setEnabled (false);
  s_stars->setEnabled (false);
  t_notes->setEnabled (false);
  currentPlayer = "";
  
  entries.clear ();
  
  QString cfilePath = Global::getInstance ()->getConfigFile ();
  if (!QFile::exists (cfilePath))  {
    if (QMessageBox::question (
      this,
      tr ("Config file not found!"),
      tr ("The PokerTH config file 'config.xml' could not be found. Do you want to set it manually?"),
      QMessageBox::Yes,
      QMessageBox::No
    ) == QMessageBox::No)  {
      return;
    }
    
    cfilePath = QFileDialog::getOpenFileName (
      this,
      tr ("Open config file."),
      QDir::homePath (),
      tr ("XML files (*.xml)")
    );
    if (cfilePath.isEmpty ())  {
      return;
    }
    Global::getInstance ()->setConfigFile (cfilePath);
  }
  
  QFile conf_file (cfilePath);
  if (!conf_file.open (QIODevice::ReadOnly | QIODevice::Text))  {
    qDebug () << "Error while loading file " << conf_file.fileName ()
      << ": " << conf_file.errorString ();
    return;
  }
  if (!conf_file.isReadable ())  {
    qDebug () << "Error: Could not read from file " << conf_file.fileName () << ".";
    conf_file.close ();
    return;
  }
  
  QString parsingError;
  int parsingErrorLine = -1;
  if (!pth_config->setContent (&conf_file, &parsingError, &parsingErrorLine))   {
    qDebug () << "Error while parsing XML file" << cfilePath << "in line "
      << parsingErrorLine << ": " << parsingError;
    conf_file.close ();
    pth_config->clear ();
    return;
  }
  conf_file.close ();
  
  QDomNodeList PTTs = pth_config->documentElement ().firstChild ().namedItem ("PlayerTooltips").childNodes ();
  
  for (int ii = 0; ii < PTTs.count (); ii++)  {
    QString line = PTTs.at (ii).toElement ().attribute ("value");
    QStringList parts = line.split (separator);
    if (parts.size () == 4)  {
      QString pname = parts.at (0);
      
      /* 
       * Okay, this is just crazy, but PokerTH seems to put a & into the player
       * names for the nr of stars and use a clear player name for the tooltips!
       * I don't know why! However, I need to work around this, so don't wonder
       * about this insane code...
       */
      if (pname.contains (QChar ('&')))  {
        pname.remove (QChar ('&'));
        entries[pname].notes = parts.at (1);
      } else  {
        entries[pname].stars = parts.at (2).toInt ();
        if (!parts.at (1).isEmpty ())  {
          entries[pname].notes = parts.at (1);
        }
      }
    }
  }
}

void NotesStarsWidget::showPlayerNotes (QString player_name)
{
  if (!player_name.isEmpty ())  {
    currentPlayer = player_name;
  }
  
  s_stars->blockSignals (true);
  t_notes->blockSignals (true);
  s_stars->setEnabled (!(currentPlayer.isEmpty () || pth_config->isNull ()));
  t_notes->setEnabled (!(currentPlayer.isEmpty () || pth_config->isNull ()));
  s_stars->setValue (entries[currentPlayer].stars);
  t_notes->clear ();
  t_notes->setText (entries[currentPlayer].notes);
  b_appl->setEnabled (false);
  b_disc->setEnabled (false);
  s_stars->blockSignals (false);
  t_notes->blockSignals (false);
}