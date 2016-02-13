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
#include <QStringList>

enum tableSize
{
  FULLRING,
  SHORTHAND,
  HEADSUP,
  ANY
};

enum gameRound
{
  PREFLOP,
  FLOP,
  TURN,
  RIVER,
  SHOWDOWN
};

struct PlayerStat
{
  /* General */
  uint32_t observed_hands, contibet;

  /* 
   * 0: Preflop
   * 1: Flop
   * 2: Turn
   * 3: River
   * 4: Showdown
   */
  uint32_t round_seen[5],
    round_bet[4],
    round_check[4],
    round_call[4],
    round_nbet[4],
    round_fn[4],
    round_checkraise[4];

  /* Winnings */
  uint32_t wsd, wwsf;
  
  double VPIP () const;
  double preflop_raise () const;
  double AF_ave () const;
  double AF (const gameRound) const;
  double F_contibet () const;
  double seen_round (const gameRound) const;
  double wtShowdown () const;
  
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

class Statistics
{
private:
  smap statMaps[4];
  
public:
  Statistics ();
  virtual ~Statistics ();
  
  void loadStatistics (const QString);
  QStringList getPlayerNames ();
  PlayerStat getPlayerStat (const QString, const tableSize);
};

#endif