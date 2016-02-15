#include "analysis_widget.hpp"

#include <QVBoxLayout>
#include <QLabel>

AnaWidget::AnaWidget (QWidget* parent)
  : QWidget (parent)
{
  l_icon = new QLabel ();
  l_icon->setAlignment (Qt::AlignCenter);
  l_icon->setPixmap (QPixmap ("/usr/local/share/PokerTH_tracker/PokerTH_Tracker.png"));
  l_caption = new QLabel (tr ("Category"));
  l_caption->setAlignment (Qt::AlignCenter);
  l_short = new QLabel (tr ("Short description"));
  l_tips = new QLabel (tr ("Try to..."));
  layout = new QVBoxLayout (this);
  
  layout->addWidget (l_icon);
  layout->addWidget (l_caption);
  layout->addWidget (l_short);
  layout->addWidget (l_tips);
}

AnaWidget::~AnaWidget ()
{}

void AnaWidget::refresh (tableSize ts)
{
  
}