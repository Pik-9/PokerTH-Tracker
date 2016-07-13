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

#include "analysis_widget.hpp"
#include "global.hpp"

#include <QVBoxLayout>
#include <QLabel>

#include <math.h>

/* In C++11 the isfinite macro became a function under std. */
#if __cplusplus >= 201103L
#define isfinite std::isfinite
#endif

const uint32_t AnaWidget::min_observed_hands[4] = {100, 100, 75, 120};

AnaWidget::AnaWidget (Statistics* pinfo, QWidget* parent)
  : QWidget (parent), stat (pinfo)
{
  l_icon = new QLabel ();
  l_icon->setAlignment (Qt::AlignCenter);
  l_caption = new QLabel (tr ("Category"));
  l_caption->setAlignment (Qt::AlignCenter);
  l_short = new QLabel (tr ("Short description"));
  l_tips = new QLabel (tr ("Weaknesses"));
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
  QString short_desc[11] = {
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
      "This player plays far too many hands preflop,\n"
      "but he is a fit-or-fold player! He might be chasing\n"
      "draws sometimes, but he will usually throw his\n"
      "hand away if he didn't hit anything. Try to figure\n"
      "out whether he has something and if not: Bluff him\n"
      "out!"
    ),
    tr (
      "This player has no idea of what Poker is about!\n"
      "He plays far too many hands and calls down till\n"
      "the river hoping for a miracle! No matter how\n"
      "bad his hand is, he will play it till the end!\n"
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
    ),
    tr (
      "This pathetic little worm's only purpose in\n"
      "life is trolling other people! He thinks he\n"
      "is incredibly funny, because he annoys other\n"
      "players by pushing all-in with junk all the\n"
      "time! He loves to see other players complaining\n"
      "about him in the chat window, so remember:\n"
      "DON'T FEED THE TROLL!\n"
      "Just wait for premium hands to call him.\n"
      "Attention: The tips below only apply when\n"
      "he is playing seriously."
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
   *   Fullring:  vl =   15%, vh = 25%, al = 1.0, ah = 2.0
   *   Shorthand: vl =   20%, vh = 35%, al = 1.1, ah = 2.0
   *   Heads-Up:  vl =   50%, vh = 90%, al = 1.5, ah = 5.0
   *   All:       vl = 22.5%, vh = 35%, al = 1.0, ah = 2.2
   */
  
  const double vl[4] = {15.0, 20.0, 50.0, 22.5};
  const double vh[4] = {25.0, 35.0, 90.0, 35.0};
  const double al[4] = {1.0, 1.1, 1.5, 1.0};
  const double ah[4] = {2.0, 2.0, 5.0, 2.2};
  
  /* The char. matrix: */
  const PlayerCharacteristic rating[3][3] = {
    {P_Rock, P_Weak_Passive, P_Fish},
    {P_Rock, P_TAG, P_dumb_LAG},
    {P_TAG, P_dumb_LAG, P_Maniac}
  };
  
  PlayerCharacteristic pc;
  
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
  if (!isfinite (ps.AF_ave ()))  {
    pc = P_Fool;
  }
  
  /* Determine whether it's a dumb or a successfull LAG. */
  if (pc == P_dumb_LAG)  {
    if (
      (ps.wonPostflop () >= 37.5) &&
      (((ps.wonPostflop () - ps.wonShowdown ()) >= 20.0) ||
      (ps.wonPostflop () / ps.wonShowdown () >= 2.0))
    )  {
      pc = P_LAG;
    }
  }
  
  /* Determine whether it's a fish or a donkey. */
  const double wts_l[4] = {40.0, 45.0, 75.0, 40.0};
  if (pc == P_Fish)  {
    if (ps.wtShowdown () > wts_l[(int) ts])  {
      pc = P_Donkey;
    }
  }
  
  /* Recognise All-In Trolls */
  if ((ts != HEADSUP) && (ps.hardAllin () >= 20.0))  {
    pc = P_Allin_Troll;
  }
  
  /* The minimum observed hands: */
  if (ps.observed_hands < min_observed_hands[(int) ts])  {
    pc = P_NEData;
  }
  
  return pc;
}

void AnaWidget::refresh (const QString pname, tableSize ts)
{
  PlayerStat ps = stat->getPlayerStat (pname, ts);
  
  QString caption[11] = {
    tr ("<font color='#808080'>Too few data!</font>"),
    tr ("<font color='#550055'>Rock</font>"),
    tr ("<font color='#800080'>Weak-Passive</font>"),
    tr ("<font color='#1000EE'>Fish</font>"),
    tr ("<font color='#FF0080'>Donkey</font>"),
    tr ("<font color='#00FF80'>Shark</font>"),
    tr ("<font color='#FF8000'>Loose-Aggressive</font>"),
    tr ("<font color='#559900'>Dumb Loose-Aggressive</font>"),
    tr ("<font color='#CCCC00'>Fool</font>"),
    tr ("<font color='#FF0000'>Maniac</font>"),
    tr ("<font color='#66AA22'>All-In Troll</font>")
  };
    
  
  PlayerCharacteristic pc = (pname.isEmpty () ? P_NEData : analyseChar (ps, ts));
    
  l_caption->setText (caption[(int) pc]);
  l_short->setText (charDescription (pc));
  
  QString icons[11] = {
    ":/s_NoData.png",
    ":/s_Rock.png",
    ":/s_Weak-Passive.png",
    ":/s_Fish.png",
    ":/s_Donkey.png",
    ":/s_Shark.png",
    ":/s_LAG.png",
    ":/s_Dumb_LAG.png",
    ":/s_Fool.png",
    ":/s_Maniac.png",
    ":/s_All-in_Troll.png"
  };
  
  QPixmap cat_icon (icons[(int) pc]);
  l_icon->setPixmap (cat_icon.scaled (l_icon->size (), Qt::KeepAspectRatio));
  
  QString advise ("");
  
  if (ps.seen_round (TURN) / ps.seen_round (FLOP) <= 0.65)  {
    advise += tr (
      "- He often gives up after missing the flop.\n  Try to bluff him on the flop.\n"
    );
  }
  
  if (ps.seen_round (RIVER) / ps.seen_round (TURN) <= 0.65)  {
    advise += tr (
      "- He often gives up after missing the turn.\n  Try to bluff him on the turn.\n"
    );
  }
  
  if (ps.seen_round (SHOWDOWN) / ps.seen_round (RIVER) <= 0.65)  {
    advise += tr (
      "- He often gives up after missing the river.\n  Try to bluff him on the river.\n"
    );
  }
  
  if ((ps.preflop_raise () >= 4.0) && (ps.F_contibet () > 70) && (ps.F_contibet () - ps.T_contibet () >= 20.0))  {
    advise += tr (
      "- He may be vulnerable to floating.\n"
    );
  }
  
  if ((ps.AF (FLOP) < 1.0) && (ps.AF (TURN) < 1.0) && (ps.AF (RIVER) > 1.0) && (ps.wtShowdown () > 40.0))  {
    advise += tr (
      "- This player is probably a calling station.\n  Be carefull with bluffing.\n"
    );
  }
  
  if (ps.folded_nbet () >= 30.0)  {
    advise += tr (
      "- Try to reraise him to see, whether he's bluffing.\n"
      "  He will often give up his hand then.\n"
    );
  }
  
  if (ps.folded_conbet () >= 40.0)  {
    advise += tr (
      "- You should usually fire continuation bets against him!\n"
    );
  }
  
  if (ps.folded_turnbet () - ps.folded_conbet () >= 20.0)  {
    advise += tr (
      "- Don't worry if he called your continuation bet.\n"
      "  Just 2nd barrel him!\n"
    );
  }
  
  if (ps.checkraise_prop () > 5.0)  {
    advise += tr (
      "- This player likes to check-raise.\n"
      "  Be carefull when he checks - it\n"
      "  might be a trap!\n"
    );
  }
  
  if ((ps.wonPostflop () - ps.wonShowdown () >= 30.0) && (ps.AF_ave () >= 0.8))  {
    advise += tr (
      "- Be carefull: He might be a competent LAG!\n"
    );
  }
  
  if (ts != HEADSUP)  {
    double won_in_showdown = ps.wonShowdown () / ps.wtShowdown ();
    if (won_in_showdown < 0.45)  {
      advise += tr (
        "- This player often goes to\n"
        "  showdown with weak hands.\n"
      );
    } else if ((won_in_showdown > 0.55) && (ps.wtShowdown () < 45.0))  {
      advise += tr (
        "- This player only goes to\n"
        "  showdown with pretty strong hands.\n"
      );
    }
  }
  
  if (pc == P_NEData) {
    l_tips->setText (tr ("Too few data to detect weaknesses!"));
  } else  {
    l_tips->setText (advise);
  }
}