/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2012 by Eric MAEKER, MD (France) <eric.maeker@gmail.com>      *
 *  All rights reserved.                                                   *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program (COPYING.FREEMEDFORMS file).                   *
 *  If not, see <http://www.gnu.org/licenses/>.                            *
 ***************************************************************************/
/***************************************************************************
 *   Main developers : Eric MAEKER, <eric.maeker@gmail.com>                *
 *   Contributors :                                                        *
 *       Christian A. Reiter                                               *
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef AGENDA_DAYAVAILABILITY_H
#define AGENDA_DAYAVAILABILITY_H

#include "agenda_exporter.h"

#include <QTime>
#include <QVector>

namespace Agenda {

struct AGENDA_EXPORT TimeRange {
    TimeRange() : id(-1) {}

    // the id is used for database accesses and should be modified
    int id;
    QTime from, to;
};

class AGENDA_EXPORT DayAvailability
{
public:
    DayAvailability();

    void setId(const int id) {m_id = id;}
    int id() const {return m_id;}

    void clearTimeRange() {timeRanges.clear();}

    void setWeekDay(const int weekDay)
    {
        Q_ASSERT((weekDay >= Qt::Monday && weekDay <= Qt::Sunday) || weekDay == -1);
        m_WeekDay = weekDay;
    }
    int weekDay() const {return m_WeekDay;}

    void addTimeRange(const QTime &from, const QTime &to);
    void addTimeRange(const TimeRange &tr);
    void setTimeRanges(const QVector<TimeRange> &ranges) {timeRanges = ranges;}

    int timeRangeCount() const {return timeRanges.count();}
    TimeRange timeRange(const int index) const;
    void removeTimeRangeAt(const int index);

private:
    int m_id;
    int m_WeekDay;
    bool m_isAvailable;
    QVector<TimeRange> timeRanges;
};

} // end namespace Agenda
#endif // AGENDA_DAYAVAILABILITY_H