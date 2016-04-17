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

#ifdef _OPENMP
#include <omp.h>
#endif

double PlayerStat::VPIP () const
{
  return 100.0 * pf_invest / observed_hands;
}

double PlayerStat::preflop_raise () const
{
  return 100.0 * round_bet[0] / observed_hands;
}

double PlayerStat::bet3_preflop () const
{
  return 100.0 * pf_3bet / observed_hands;
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
  return 100.0 * f_cb / pf_open;
}

double PlayerStat::T_contibet () const
{
  return 100.0 * t_2b / pf_open;
}

double PlayerStat::folded_conbet () const
{
  return 100.0 * f_fc / f_cbe;
}

double PlayerStat::folded_turnbet () const
{
  return 100.0 * t_f2 / t_2be;
}

double PlayerStat::folded_nbet () const
{
  return 100.0 * postflop_fn / postflop_nbe;
}

double PlayerStat::checkraise_prop () const
{
  return 100.0 * checkraises / (checkraises + round_check[1] + round_check[2] + round_check[3]);
}

double PlayerStat::hardAllin () const
{
  return 100.0 * hard_allin / observed_hands;
}

double PlayerStat::seen_round (const gameRound RND) const
{
  return 100.0 * round_seen[RND] / observed_hands;
}

double PlayerStat::wtShowdown () const
{
  return 100.0 * round_seen[4] / round_seen[1];
}

double PlayerStat::wonShowdown () const
{
  return 100.0 * wsd / round_seen[1];
}

double PlayerStat::wonPostflop () const
{
  return 100.0 * wwsf / round_seen[1];
}

PlayerStat& PlayerStat::operator+= (PlayerStat& other)
{
  observed_hands += other.observed_hands;
  f_cb += other.f_cb;
  wsd += other.wsd;
  wwsf += other.wwsf;
  pf_open += other.pf_open;
  pf_invest += other.pf_invest;
  pf_3bet += other.pf_3bet;
  t_2b += other.t_2b;
  t_2be += other.t_2be;
  t_f2 += other.t_f2;
  f_cbe += other.f_cbe;
  f_fc += other.f_fc;
  postflop_fn += other.postflop_fn;
  postflop_nb += other.postflop_nb;
  postflop_nbe += other.postflop_nbe;
  checkraises += other.checkraises;
  hard_allin += other.hard_allin;
  for (int ii = 0; ii < 4; ++ii)  {
    round_bet[ii] += other.round_bet[ii];
    round_call[ii] += other.round_call[ii];
    round_check[ii] += other.round_check[ii];
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

bool Statistics::loadStatistics (const QString path, uint32_t* count_files)
{
  bool success = true;
  QDir dir (path);
  QStringList files = dir.entryList (QStringList ("*.pdb"));
  
  /* Clear the maps before (re-)loading. */
  for (uint32_t ii = 0; ii < 4; ++ii)  {
    statMaps[ii].clear ();
  }
  
  #pragma omp parallel for reduction(&:success) if (files.count () > 100)
  for (uint32_t ii = 0; ii < files.count (); ++ii)  {
    QString conName = "Logfile_";
    #ifdef _OPENMP
      conName += QString::number (omp_get_thread_num ());
    #endif
    
    QSqlDatabase db = (
      QSqlDatabase::contains (conName) ?
      QSqlDatabase::database (conName) :
      QSqlDatabase::addDatabase ("QSQLITE", conName)
    );
    db.setDatabaseName (dir.absoluteFilePath (files.at (ii)));
    bool t_suc = db.open ();
    QSqlQuery qu (db);
    if (!t_suc)  {
      qDebug () << "Error while loading database: " << db.lastError ().text ();
      qDebug () << "Available SQL drivers: " << QSqlDatabase::drivers ();
    } else  {
      t_suc = qu.exec (
        "SELECT Action.BeRo, Player.Player, Action.Action, Action.Amount "
        "FROM Action, Player "
        "WHERE Action.UniqueGameID = Player.UniqueGameID AND Action.Player = Player.Seat"
      );
    
      if (!t_suc)  {
        qDebug () << "Error while executing query: " << qu.lastError ().text ();
      }
    }
    
    uint32_t round_ = 6, nbet = 0, betsize = 0, bblind = 0;
    
    /* If a player in this round fired a contibet or turnbet. */
    uint32_t conbet = 0, turnbet = 0;
    smap tmp;
    while (qu.next ())  {      
      uint32_t round = qu.value (0).toInt ();
      QString player_name = qu.value (1).toString ();
      QString player_action = qu.value (2).toString ();
      uint32_t amount = qu.value (3).toInt ();
      
      /* New game. */
      if (player_action == "starts as dealer")  {
        /* Determine table size and correct map. */
        tableSize mtu;
        if (tmp.size() > 6)  {
          mtu = FULLRING;
        } else if (tmp.size () > 2)  {
          mtu = SHORTHAND;
        } else  {
          mtu = HEADSUP;
        }
        smap& mapToUse = statMaps[mtu];
        for (smap::iterator it = tmp.begin (); it != tmp.end (); ++it)  {
          mapToUse[it->first] += it->second;
        }
        betsize = nbet = conbet = turnbet = 0;
        tmp.clear ();
      }
      
      if (round > round_)  {
        /* New round. */
        betsize = nbet = conbet = turnbet = 0;
      }
      round_ = round;
      
      //tmp[player_name].round_seen[round] = 1;
      for (uint32_t ir = 0; ir <= round; ++ir)  {
        tmp[player_name].round_seen[ir] = 1;
      }
      tmp[player_name].observed_hands = 1;
      
      /* When this player is confronted with a contibet. */
      if ((conbet) && (!tmp[player_name].f_cb))  {
        tmp[player_name].f_cbe = 1;
      }
      
      /* When this player is confronted with a turnbet. */
      if ((turnbet) && (!tmp[player_name].t_2b))  {
        tmp[player_name].t_2be = 1;
      }
      
      /* When this player is confronted with a n-bet. */
      if ((nbet > 1) && (round))  {
        tmp[player_name].postflop_nbe++;
      }
      
      if (player_action == "posts big blind")  {
        nbet = 1;
        bblind = betsize = amount;
      }
      
      if (player_action == "checks")  {
        tmp[player_name].round_check[round]++;
      }
      
      if (player_action == "calls")  {
        tmp[player_name].round_call[round]++;
      }
      
      if (player_action == "bets")  {
        /* If somebody betted before in this round. */
        if (nbet)  {
          /* If it is post flop action. */
          if (round)  {
            tmp[player_name].postflop_nb++;
          } else if (nbet == 2)  {
            /* If it is a 3bet preflop. */
            tmp[player_name].pf_3bet++;
          }
          
          /* Recognise check-raises. */
          if (tmp[player_name].round_check[round])  {
            tmp[player_name].checkraises++;
            /* Reset the check counter, because this is not a passive check. */
            tmp[player_name].round_check[round] = 0;
          }
        }
        
        /* Recognise Turn bets. */
        if ((round == TURN) && (tmp[player_name].f_cb))  {
          tmp[player_name].t_2b++;
          turnbet = 1;
        }
        
        /* If it happens preflop, this player is opening the hand. */
        if (!round)  {
          tmp[player_name].pf_open = 1;
        }
        
        tmp[player_name].round_bet[round]++;
        betsize = amount;
        
        /* Recognise a continuation bet. */
        if ((tmp[player_name].pf_open) && (round == FLOP))  {
          tmp[player_name].f_cb = 1;
          conbet = 1;
        }
        
        nbet++;
      }
      
      if (player_action == "folds")  {
        /* If the player folds to a n-bet after betting first post flop. (n > 1) */
        if ((nbet > 1) && (tmp[player_name].round_bet[round]) && (round))  {
          tmp[player_name].postflop_fn++;
        }
        
        /* If the player folds to a continuation bet. */
        if ((conbet) && (tmp[player_name].f_cbe))  {
          tmp[player_name].f_fc++;
        }
        
        /* If the player folds to a turn bet. */
        if ((turnbet) && (tmp[player_name].t_2be))  {
          tmp[player_name].t_f2++;
        }
      }
      
      if (player_action == "is all in with")  {
        for (uint32_t ir = 0; ir < 5; ++ir)  {
          tmp[player_name].round_seen[ir] = 1;
        }
        
        /* Was it more than 10 BB? */
        if ((amount / bblind) > 10)  {
          tmp[player_name].hard_allin++;
        }
        
        /* Is this all-in a call or a raise? */
        if (amount > betsize)  {
          if (nbet)  {
            if (round)  {
              tmp[player_name].postflop_nb++;
            }
            if (tmp[player_name].round_check[round])  {
              tmp[player_name].checkraises++;
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
      
      /* Check whether the player invested anything preflop,
       * either by betting or by calling. */
      if ((tmp[player_name].round_call[PREFLOP]) || (tmp[player_name].round_bet[PREFLOP]))  {
        tmp[player_name].pf_invest = 1;
      }
      
      if (player_action == "wins")  {
        if (round)  {
          /* Won money after seeing the flop. */
          tmp[player_name].wwsf++;
        }
        if (round == SHOWDOWN)  {
          /* Won money in showdown. */
          tmp[player_name].wsd++;
        }
      }
    }
    /* Determine table size and correct map. */
    tableSize mtu;
    if (tmp.size() > 6)  {
      mtu = FULLRING;
    } else if (tmp.size () > 2)  {
      mtu = SHORTHAND;
    } else  {
      mtu = HEADSUP;
    }
    smap& mapToUse = statMaps[mtu];
    
    #pragma omp critical
    for (smap::iterator it = tmp.begin (); it != tmp.end (); ++it)  {
      mapToUse[it->first] += it->second;
    }
    
    db.close ();
    success &= t_suc;
  }
  
  /* Sum up the maps for different table sizes in the last one for any. */
  for (uint32_t ii = 0; ii < 3; ++ii)  {
    for (smap::iterator it = statMaps[ii].begin (); it != statMaps[ii].end (); ++it)  {
      statMaps[ANY][it->first] += it->second;
    }
  }
  
  if ((success) && (count_files))  {
    *count_files = files.size ();
  }
  
  return success;
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