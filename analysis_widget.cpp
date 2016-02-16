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
  QString short_desc[9] = {
    tr (
      "You didn't observe enough hands\n"
      "to make a reliable rating."
    ),
    tr (
      "In this player's opinion raising is a sin!\n"
      "He plays only premium hands.\n"
      "You won't get much money out of this player,\n"
      "but he won't hurt you either!\n"
      "You can usually bluff him a lot.\n"
      "If he bets, you better fold unless you got the\n"
      "nuts."
    ),
    tr (
      "This player is so scared of taking action\n"
      "that he would be better off with Bingo!\n"
      "He just picks up some nice hands and hopes\n"
      "for a hit on the Flop.\n"
      "A bluff will succeed often."
    ),
    tr (
      "This player has no idea of what Poker is about!\n"
      "He plays far too many hands and calls down till\n"
      "the river hoping for a miracle!\n"
      "He should be easy to exploit, but don't bluff him!"
    ),
    tr (
      "This player actually knows what he's doing!\n"
      "He plays only hands which have big potential\n"
      "and he knows how to play'em aggressively!\n"
      "You should try to avoid getting yourself into\n"
      "a duel with him. Though a good bluff might\n"
      "succeed sometimes - But BE CAREFULL!"
    ),
    tr (
      "This guy likes to play many hands aggressively.\n"
      "But he seems to be pretty successfull with it,\n"
      "so don't consider him a stupid bluffer! Even\n"
      "though he's bluffing a lot he actually knows\n"
      "what he's doing and he can be a really strong\n"
      "opponent. Be carefull and don't get on tilt."
    ),
    tr (
      "This guy is a nut, who thinks Poker would be\n"
      "about showing off! A wannabe cowboy who is\n"
      "regularily overplaying his cards just to not\n"
      "look weak! I wonder if he really doesn't notice\n"
      "that he's loosing in the long run or whether he\n"
      "just doesn't care. However the best you can do\n"
      "is just letting him have his fun and get out of\n"
      "his way..."
    ),
    tr (
      "Well...\n"
      "I don't know, what this guys is actually playing,\n"
      "but it has not much to do with Texas Hold'em!\n"
      "Just get out of his way unless you know what\n"
      "you're doing."
    ),
    tr (
      "Okay, this guy's just cracked! He is hyper\n"
      "aggressive in a way that can't be profitable.\n"
      "He bets and raises like there's no tomorrow!\n"
      "Of course, he is bluffing a lot, but it's\n"
      "important that you KEEP CALM. You don't want\n"
      "this guy to make you push all-in with AK and\n"
      "loose against a pair of threes! So just get\n"
      "out of his way and wait until he eliminates\n"
      "himself."
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
   *   Fullring:  vl =   15%, vh = 25%, al = 0.8, ah = 2.0
   *   Shorthand: vl =   20%, vh = 35%, al = 1.0, ah = 2.0
   *   Heads-Up:  vl =   50%, vh = 90%, al = 1.5, ah = 5.0
   *   Any:       vl = 22.5%, vh = 35%, al = 1.0, ah = 2.2
   */
  
  const double vl[4] = {15.0, 20.0, 50.0, 22.5};
  const double vh[4] = {25.0, 35.0, 90.0, 35.0};
  const double al[4] = {0.8, 1.0, 1.5, 1.0};
  const double ah[4] = {2.0, 2.0, 5.0, 2.2};
  
  /* The char. matrix: */
  const PlayerCharacteristic rating[3][3] = {
    {P_Rock, P_Weak_Passive, P_Fish},
    {P_Rock, P_TAG, P_LAG},
    {P_TAG, P_LAG, P_Maniac}
  };
  
  PlayerCharacteristic pc = P_Fool;
  
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
  
  /* Determine whether it's a dump or a successfull LAG. */
  if (pc == P_LAG)  {
    if ((ps.wonPostflop () - ps.wonShowdown ()) < 25.0)  {
      pc = P_dump_LAG;
    }
  }
  
  /* The minimum observed hands: */
  const uint32_t oh[4] = {100, 100, 75, 120};
  if (ps.observed_hands < oh[(int) ts])  {
    pc = P_NEData;
  }
  
  return pc;
}

void AnaWidget::refresh (const QString pname, tableSize ts)
{
  PlayerStat ps = stat->getPlayerStat (pname, ts);
  
  QString caption[9] = {
    tr ("<font color='#808080'>Too few data!</font>"),
    tr ("<font color='#550055'>Rock</font>"),
    tr ("<font color='#800080'>Weak-Passive</font>"),
    tr ("<font color='#1000EE'>Fish</font>"),
    tr ("<font color='#00FF80'>Shark</font>"),
    tr ("<font color='#FF8000'>Loose-Aggressive</font>"),
    tr ("<font color='#559900'>Dump Loose-Aggressive</font>"),
    tr ("<font color='#CCCC00'>Fool</font>"),
    tr ("<font color='#FF0000'>Maniac</font>")
  };
    
  PlayerCharacteristic pc = analyseChar (ps, ts);
    
  l_caption->setText (caption[(int) pc]);
  l_short->setText (charDescription (pc));
}