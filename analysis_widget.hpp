#ifndef ANALYSIS_WIDGET_HPP
#define ANALYSIS_WIDGET_HPP

#include <QWidget>

#include "PlayerStat.hpp"

class QVBoxLayout;
class QLabel;

class AnaWidget : public QWidget
{
  Q_OBJECT
private:
  QVBoxLayout *layout;
  QLabel *l_icon,
    *l_caption,
    *l_short,
    *l_tips;
  
public:
  AnaWidget (QWidget* parent = 0);
  ~AnaWidget ();
  
public slots:
  void refresh (tableSize ts = ANY);
};

#endif