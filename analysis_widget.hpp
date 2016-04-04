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
  P_dump_LAG,
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