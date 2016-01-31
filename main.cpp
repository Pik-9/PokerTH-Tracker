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

#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main (int argc, char *argv[])
{
  QApplication app (argc, argv);
  QTranslator *trans = new QTranslator ();
  QString filename = QString ("PokerTH_tracker_%1.qm").arg (QLocale::system ().name ());
  if (!trans->load (filename, "/usr/local/share/PokerTH_tracker"))  {
    /* If the translation couldn't be found in the directory above, try current directory.
     * This is necessary for the Windows cross build.
     */
    trans->load (filename);
  }
  app.installTranslator (trans);
  MainWindow *mw = new MainWindow ();
  mw->show ();
  return app.exec();
}