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

#ifndef ANALYSIS_WIDGET_HPP
#define ANALYSIS_WIDGET_HPP

#include <QWidget>

#include "PlayerStat.hpp"

class QVBoxLayout;
class QLabel;

enum PlayerCharacteristic
{
  P_NEData,
  P_Rock,
  P_Weak_Passive,
  P_Fish,
  P_Donkey,
  P_TAG,
  P_LAG,
  P_dumb_LAG,
  P_Fool,
  P_Maniac,
  P_Allin_Troll
};

class AnaWidget : public QWidget
{
  Q_OBJECT
private:
  Statistics* stat;
  
  QVBoxLayout *layout;
  QLabel *l_icon,
    *l_caption,
    *l_short,
    *l_tips;
    
public:
  /* The minimum observed hands. */
  static const uint32_t min_observed_hands[4];
  
public:
  AnaWidget (Statistics*, QWidget* parent = 0);
  ~AnaWidget ();
  
  /* 
   * It is necessary to make a function for this, because
   * tr() doesn't work with static QString arrays.
   */
  static QString charDescription (const PlayerCharacteristic);
  
  static PlayerCharacteristic analyseChar (const PlayerStat, const tableSize);
  
public slots:
  void refresh (const QString, tableSize ts = ANY);
};

#endif