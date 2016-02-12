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

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <stdint.h>
#include <map>
#include <QString>

struct PlayerStat
{
  /* Preflop */
  uint32_t observed_hands, pf_calls, pf_open;

  /* Flop */
  uint32_t f_seen, f_check_call, f_bet, f_contibet, f_fold;

  /* Turn */
  uint32_t t_seen, t_check_call, t_bet, t_fold;

  /* River */
  uint32_t r_seen, r_check_call, r_bet, r_fold;

  /* Showdown */
  uint32_t sd_seen, sd_won;
  
  double VPIP () const;
  double preflop_raise () const;
  double AF_ave () const;
  double AF_flop () const;
  double AF_turn () const;
  double AF_river () const;
  double contibet () const;
  double seen_turn () const;
  double seen_river () const;
  double wtShowdown () const;
  double wonShowdown () const;
  
  PlayerStat& operator+= (PlayerStat&);
};

/*
 * This is just a comparison object.
 * It defines an < operator where upper and lower case letters are
 * treated the same.
 */
struct orderQStringIgnCase
{
  bool operator ()(const QString& a, const QString& b) const
  {
    return (a.toUpper () < b.toUpper ());
  }
};

typedef std::map<QString, PlayerStat, orderQStringIgnCase> smap;

#endif