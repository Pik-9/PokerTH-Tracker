#include "analysis_widget.hpp"
#include "global.hpp"

#include <QVBoxLayout>
#include <QLabel>

#include <math.h>

AnaWidget::AnaWidget (Statistics* pinfo, QWidget* parent)
  : QWidget (parent), stat (pinfo)
{
  l_icon = new QLabel ();
  l_icon->setAlignment (Qt::AlignCenter);
  l_icon->setPixmap (QPixmap (Global::getInstance ()->getDataDir () + "/PokerTH_Tracker.png"));
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

QString AnaWidget::charDescription (const PlayerCharacteristic ps)
{
  QString short_desc[7] = {
    tr (
      "In this player's opinion raising is a sin!\n"
      "He plays only premium hands.\n"
      "You won't get much money out of this player,\n"
      "but he won't hurt you either!\n"
      "You can usually bluff him a lot.\n"
      "If he bets, you better fold unless you got the stone cold nuts."
    ),
    tr (
      "This player is so scared of taking action\n"
      "that he would be off with Bingo!\n"
      "He just picks up some nice hands and hopes for a hit on the Flop.\n"
      "A bluff will succeed often."
    ),
    tr (
      "This player has no idea of what Poker is about!\n"
      "He plays far too many hands and calls down till the river\n"
      "hoping for a miracle!\n"
      "He should be easy to exploit, but don't bluff him!"
    ),
    tr (
      "This player actually knows what he's doing!\n"
      "He plays only hands which have big potential\n"
      "and he knows how to play'em aggressively!\n"
      "You should try to avoid getting yourself into a duel with him.\n"
      "Though a good bluff might succeed sometimes - But BE CAREFULL!"
    ),
    tr (
      "This guy likes to play many hands aggressively. This can either\n"
      "mean that he's a really smart player like Gus Hansen who is specialised\n"
      "in crushing TAGs or he's just a wannabe James Bond who wants to show off!\n"
      "It's up to you to find out. But you should be carefull with him anyway.\n"
      "Don't get on tilt!"
    ),
    tr (
      "Well...\n"
      "I don't know, what this guys is actually playing,\n"
      "but it has not much to do with Texas Hold'em!\n"
      "Just get out of his way unless you know what you're doing."
    ),
    tr (
      "Okay, this guy's just cracked! He is hyper aggressive\n"
      "in a way that can't be profitable. He bets and raises\n"
      "like there's no tomorrow! Of course, he is bluffing a lot,\n"
      "but it's important that you KEEP CALM. You don't want this\n"
      "guy make you push all-in with AK and loose against a pair of threes!\n"
      "So just get out of his way and wait until he eliminates himself."
    )
  };
  
  return short_desc[(int) ps];
}

PlayerCharacteristic AnaWidget::analyseChar (const PlayerStat ps, const tableSize ts)
{
  /*
   * The player characteristics are determined by the VP$IP and the AF.
   * For every table size there are different borders:
   * vl, vh, al, ah
   * 
   *                   VP$IP
   *  AF       <vl  |  vl-vh  |  >vh
   * --------------------------------
   *  <al   | Rock  | W-P     | Fish  
   * --------------------------------
   *  al-ah | TAG   | TAG     | LAG   
   * --------------------------------
   *  >ah   | TAG   | LAG     | Maniac
   * 
   * The borders are the following:
   *   Fullring:  vl = 15%, vh = 25%, al = 0.8,  ah = 2.0
   *   Shorthand: vl = 20%, vh = 35%, al = 1.0,  ah = 2.0
   *   Heads-Up:  vl = 50%, vh = 90%, al = 1.5,  ah = 5.0
   */
  
  const double vl[3] = {15.0, 20.0, 50.0};
  const double vh[3] = {25.0, 35.0, 90.0};
  const double al[3] = {0.8, 1.0, 1.5};
  const double ah[3] = {2.0, 2.0, 5.0};
  
  /* The char. matrix: */
  const PlayerCharacteristic rating[3][3] = {
    {P_Rock, P_Weak_Passive, P_Fish},
    {P_TAG, P_TAG, P_LAG},
    {P_TAG, P_LAG, P_Maniac}
  };
  
  PlayerCharacteristic pc = P_Fool;
  
  if (ts == ANY)  {
  } else  {
    uint32_t ar, vr;
    if (ps.VPIP () < vl[(int) ts])  {
      vr = 0;
    } else if (ps.VPIP () > vh[(int) ts])  {
      vr = 2;
    } else  {
      vr = 1;
    }
      
    if (ps.AF_ave () < al[(int) ts])  {
      ar = 0;
    } else if (ps.AF_ave () > ah[(int) ts])  {
      ar = 2;
    } else  {
      ar = 1;
    }
      
    pc = rating[ar][vr];
    if (!(isnormal (ps.AF_ave ()) || (ps.AF_ave () == 0.0)))  {
      pc = P_Fool;
    }
  }
  
  return pc;
}

void AnaWidget::refresh (const QString pname, tableSize ts)
{
  PlayerStat ps = stat->getPlayerStat (pname, ts);
  
  /* The minimum observed hands: */
  const uint32_t oh[3] = {1, 1, 75};
  
  if (ps.observed_hands < oh[(int) ts])  {
    l_caption->setText (tr ("<font color='#808080'>Too few data!</font>"));
    l_short->setText (tr ("You need at least %1 observed hands\nto make a reliable rating.").arg (oh[(int) ts]));
    l_tips->setText ("");
  } else  {
    QString caption[7] = {
      tr ("<font color='#550055'>Rock</font>"),
      tr ("<font color='#800080'>Weak-Passive</font>"),
      tr ("<font color='#1000EE'>Fish</font>"),
      tr ("<font color='#00FF80'>Shark</font>"),
      tr ("<font color='#FF8000'>Loose-Aggressive</font>"),
      tr ("<font color='#CCCC00'>Fool</font>"),
      tr ("<font color='#FF0000'>Maniac</font>")
    };
    
    PlayerCharacteristic pc = analyseChar (ps, ts);
    
    l_caption->setText (caption[(int) pc]);
    l_short->setText (charDescription (pc));
  }
}