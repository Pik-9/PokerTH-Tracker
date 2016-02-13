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
  return 100.0 * (round_bet[0] + round_call[0]) / observed_hands;
}

double PlayerStat::preflop_raise () const
{
  return 100.0 * round_bet[0] / observed_hands;
}

double PlayerStat::AF_ave () const
{
  return (AF (FLOP) + AF (TURN) + AF (RIVER)) / 3.0;
}

double PlayerStat::AF (const gameRound RND) const
{
  return (double) round_bet[RND] / (round_check[RND] + round_call[RND]);
}

double PlayerStat::F_contibet () const
{
  return 100.0 * contibet / round_bet[0];
}

double PlayerStat::seen_round (const gameRound RND) const
{
  return 100.0 * round_seen[RND] / observed_hands;
}

double PlayerStat::wtShowdown () const
{
  return 100.0 * round_seen[4] / round_seen[1];
}

PlayerStat& PlayerStat::operator+= (PlayerStat& other)
{
  observed_hands += other.observed_hands;
  contibet += other.contibet;
  wsd += other.wsd;
  wwsf += other.wwsf;
  for (int ii = 0; ii < 4; ++ii)  {
    round_bet[ii] += other.round_bet[ii];
    round_call[ii] += other.round_call[ii];
    round_check[ii] += other.round_check[ii];
    round_checkraise[ii] += other.round_checkraise[ii];
    round_fn[ii] += other.round_fn[ii];
    round_nbet[ii] += other.round_nbet[ii];
    round_seen[ii] += other.round_seen[ii];
  }
  round_seen[SHOWDOWN] += other.round_seen[SHOWDOWN];
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
  
  /* Clear the maps before (re-)loading. */
  for (int ii = 0; ii < 4; ++ii)  {
    statMaps[ii].clear ();
  }
  
  for (int ii = 0; ii < files.count (); ++ii)  {
    db.setDatabaseName (dir.absoluteFilePath (files.at (ii)));
    db.open ();
    QSqlQuery qu (db);
    qu.exec (
      "SELECT Action.BeRo, Player.Player, Action.Action, Action.Amount "
      "FROM Action, Player "
      "WHERE Action.UniqueGameID = Player.UniqueGameID AND Action.Player = Player.Seat"
    );
    
    unsigned int round_ = 6, nbet = 0, betsize = 0;
    smap tmp;
    while (qu.next ())  {      
      unsigned int round = qu.value (0).toInt ();
      QString player_name = qu.value (1).toString ();
      QString player_action = qu.value (2).toString ();
      unsigned int amount = qu.value (3).toInt ();
      
      /* New game. */
      if (round < round_)  {
        /* Determine table size and correct map. */
        unsigned int mtu;
        if (tmp.size() < 7)  {
          mtu = 0;
        } else if (tmp.size () > 2)  {
          mtu = 1;
        } else  {
          mtu = 2;
        }
        smap& mapToUse = statMaps[mtu];
        for (smap::iterator it = tmp.begin (); it != tmp.end (); ++it)  {
          mapToUse[it->first] += it->second;
        }
      } else if (round > round_)  {
        /* New round. */
        betsize = nbet = 0;
      }
      
      tmp[player_name].round_seen[round] = 1;
      
      if (player_action == "postst big blind")  {
        nbet = 1;
        betsize = amount;
      }
      
      if (player_action == "checks")  {
        tmp[player_name].round_check[round]++;
      }
      
      if (player_action == "calls")  {
        tmp[player_name].round_call[round]++;
      }
      
      if (player_action == "bets")  {
        if (nbet)  {
          tmp[player_name].round_nbet[round]++;
          if (tmp[player_name].round_check[round])  {
            tmp[player_name].round_checkraise[round]++;
            tmp[player_name].round_check[round] = 0;
          }
        }
        tmp[player_name].round_bet[round]++;
        betsize = amount;
        
        if (tmp[player_name].round_bet[0])  {
          tmp[player_name].contibet++;
        }
        
        nbet++;
      }
      
      if (player_action == "folds")  {
        if (nbet > 1)  {
          tmp[player_name].round_fn[round]++;
        }
      }
      
      if (player_action == "is all in with")  {
        for (int ir = 0; ir < 5; ++ir)  {
          tmp[player_name].round_seen[ir] = 1;
        }
        
        /* Is this all-in a call or a raise? */
        if (amount > betsize)  {
          if (nbet)  {
            tmp[player_name].round_nbet[round]++;
            if (tmp[player_name].round_check[round])  {
              tmp[player_name].round_checkraise[round]++;
              tmp[player_name].round_check[round] = 0;
            }
          }
          tmp[player_name].round_bet[round]++;
          betsize = amount;
          nbet++;
        } else  {
          tmp[player_name].round_call[round]++;
        }
      }
      
      if (player_action == "wins")  {
        if (round)  {
          /* Won money after seeing the flop. */
          tmp[player_name].wwsf++;
        }
        if (round == 5)  {
          /* Won money in showdown. */
          tmp[player_name].wsd++;
        }
      }
    }
    db.close ();
  }
  
  /* Sum up the maps for different table sizes in the last one for any. */
  for (int ii = 0; ii < 3; ++ii)  {
    for (smap::iterator it = statMaps[ii].begin (); it != statMaps[ii].end (); ++it)  {
      statMaps[ANY][it->first] += it->second;
    }
  }
}

QStringList Statistics::getPlayerNames ()
{
  QStringList RET;
  for (smap::iterator it = statMaps[ANY].begin (); it != statMaps[ANY].end (); ++it)  {
    RET.push_back (it->first);
  }
  return RET;
}

PlayerStat Statistics::getPlayerStat(const QString pname, const tableSize size)
{
  return statMaps[size][pname];
}