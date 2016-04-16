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

#include "PlayerStat.hpp"
#include "worker.hpp"

Worker::Worker (Statistics *pstats)
  : QThread (), stat (pstats), filesLoaded (0), filePath ("")
{}

Worker::~Worker ()
{}

void Worker::setFilePath (const QString path)
{
  filePath = path;
}

QString Worker::getFilePath () const
{
  return filePath;
}

uint32_t Worker::countLoadedFiles () const
{
  return filesLoaded;
}

bool Worker::loadedSuccessfully () const
{
  return success;
}

void Worker::run ()
{
  success = stat->loadStatistics (filePath, &filesLoaded);
}