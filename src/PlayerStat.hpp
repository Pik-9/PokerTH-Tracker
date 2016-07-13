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
  uint32_t observed_hands,
    pf_open, pf_invest;
  
  /* n-bets and other stuff */
  uint32_t pf_3bet,  /* Preflop 3bet */
    f_cb, f_cbe, f_fc,  /* Contibet on flop, confronted with c-bet, folded to c-bet. */
    t_2b, t_2be, t_f2,  /* Turn bet, confronted with Turn bet and folded to Turn bet. */
    postflop_nb, postflop_nbe, postflop_fn; /* Postflop n-bet, confronted with p-n-bet, folded to p-n-bet. */
    
  uint32_t checkraises;
  
  /* How often the player went all-in with more than 10 BB. */
  uint32_t hard_allin;

  /* 
   * 0: Preflop
   * 1: Flop
   * 2: Turn
   * 3: River
   * 4: Showdown
   */
  uint32_t round_seen[5],
    round_bet[4],
    round_check[4], /* Only count passive checks; check-raises are counted separately. */
    round_call[4];

  /* Winnings */
  uint32_t wsd, wwsf;
  
  double VPIP () const;
  double preflop_raise () const;
  double bet3_preflop () const;
  double AF_ave () const;
  double AF (const gameRound) const;
  double F_contibet () const;
  double T_contibet () const;
  double folded_conbet () const;
  double folded_turnbet () const;
  double folded_nbet () const;
  double checkraise_prop () const;
  double hardAllin () const;
  double seen_round (const gameRound) const;
  double wtShowdown () const;
  double wonShowdown () const;
  double wonPostflop () const;
  
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
  
  /* Returns true on success, false otherwise. */
  bool loadStatistics (const QString, uint32_t *count_files = 0);
  QStringList getPlayerNames (tableSize ts, bool onlyEnoughData = false);
  PlayerStat getPlayerStat (const QString, const tableSize);
};

#endif