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

#include "PlayerStat.hpp"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

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

Statistics::Statistics ()
{

}

Statistics::~Statistics ()
{}

void Statistics::loadStatistics (const QString path)
{
  QDir dir (path);
  QStringList files = dir.entryList (QStringList ("*.pdb"));
  QSqlDatabase db = QSqlDatabase::addDatabase ("QSQLITE", "Logfile");
  QSqlQuery qu (db);
  for (int ii = 0; ii < files.count (); ++ii)  {
    db.setDatabaseName (dir.absoluteFilePath (files.at (ii)));
    db.open ();
    QSqlQuery qu (db);
    qu.exec (
      "SELECT Action.BeRo, Player.Player, Action.Action, Action.Amount "
      "FROM Action, Player "
      "WHERE Action.UniqueGameID = Player.UniqueGameID AND Action.Player = Player.Seat"
    );
    
    unsigned int round_ = 6, nbet = 0;
    smap tmp;
    while (qu.next ())  {
      unsigned int round = qu.value (0).toInt ();
      QString player_name = qu.value (1).toString ();
      QString player_action = qu.value (2).toString ();
      unsigned int amount = qu.value (3).toInt ();
      
      /* New game. */
      if (round < round_)  {
        
      } else if (round > round_)  {
        /* New round. */
        nbet = 0;
      }
      
      switch (round)  {
        case 1: { /* Flop */
          if (!tmp[player_name].f_seen)  {
            tmp[player_name].f_seen++;
          }
          break;
        }
      }
      
    }
  }
}