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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#include "versionnumber.h"

#include <utils/log.h>

/**
  \class Utils::VersionNumber
  \brief Ease the processing of version strings.
  Extract the major, minor and debug versions. Extract the alpha version.\n
  The version string must be formatted as follow: \n
      - major.minor.debug
      - major.minor.debug~alpha
      - major.minor.debug~alphaalpha_nb
      - major.minor.debug~beta
      - major.minor.debug~betabeta_nb
      - major.minor.debug~RC
      - major.minor.debug~RCrc_nb
*/

using namespace Utils;

VersionNumber::VersionNumber(const QString &version) :
        m_Version(version),
        m_Major(0),
        m_Minor(0),
        m_Debug(0),
        m_Alpha(0),
        m_Beta(0),
        m_RC(0),
        m_IsAlpha(false),
        m_IsBeta(false),
        m_IsRC(false)
{
//    int tilde = version.indexOf("~");
    int dotCount = version.count(".");
    int dot = 0;
    int nextDot = 0;
    bool ok;
    if (dotCount<1) {
        LOG_ERROR_FOR("VersionNumber", "Unknown/invalid version number detected: " + version);
    } else {
        dot = version.indexOf(".");
        m_Major = version.left(dot).toInt(&ok);
        if (!ok)
            LOG_ERROR_FOR("VersionNumber", "Invalid major version detected: " + version.left(dot));
        ++dot;
        nextDot = version.indexOf(".", dot);
        m_Minor = version.mid(dot,nextDot-dot).toInt(&ok);
        if (!ok)
            LOG_ERROR_FOR("VersionNumber", "Invalid minor version detected: "+ version.mid(dot,nextDot-dot));
        dot = nextDot + 1;
        // get next non-digit character
        nextDot = version.indexOf(QRegExp("\\D"), dot);
        if (nextDot!=-1) {
            // there is a non-digit character in the string
            m_Debug = version.mid(dot,nextDot-dot).toInt(&ok);
            if (!ok)
                LOG_ERROR_FOR("VersionNumber", "Invalid debug version detected: " + version.mid(dot,nextDot-dot));
        } else {
            // only digits
            m_Debug = version.mid(dot).toInt();
            nextDot = dot;
        }
    }
    if (version.contains(QRegExp("alpha\\d*", Qt::CaseInsensitive))) {
        m_IsAlpha = true;
        m_Alpha = version.mid(version.indexOf(QRegExp("alpha\\d*", Qt::CaseInsensitive)) + 5).toInt(&ok);
    } else if (version.contains(QRegExp("beta\\d*", Qt::CaseInsensitive))) {
        m_IsBeta = true;
        m_Beta = version.mid(version.indexOf(QRegExp("beta\\d*", Qt::CaseInsensitive)) + 4).toInt(&ok);
    } else if (version.contains(QRegExp("rc\\d*", Qt::CaseInsensitive))) {
        m_IsRC = true;
        m_RC = version.mid(version.indexOf(QRegExp("rc\\d*", Qt::CaseInsensitive)) + 2).toInt(&ok);
    }
}

bool VersionNumber::operator>(const VersionNumber &b) const
{
    if (m_Major > b.majorNumber()) {
        return true;
    } else if (m_Major == b.majorNumber()) {
        if (m_Minor > b.minorNumber()) {
            return true;
        } else if (m_Minor == b.minorNumber()) {
            if (m_Debug > b.debugNumber()) {
                return true;
            } else if (m_Debug==b.debugNumber()) {
                // Check sub versions
                if (m_IsAlpha) {
                    if (b.isAlpha()) {
                        if (m_Alpha > b.alphaNumber()) {
                            return true;
                        }
                    }
                }

                if (m_IsBeta) {
                    if (b.isAlpha())
                        return true;
                    if (b.isBeta() && m_Beta > b.betaNumber())
                        return true;
                }

                if (m_IsRC) {
                    if (b.isAlpha() || b.isBeta())
                        return true;
                    if (m_RC > b.rcNumber())
                        return true;
                }
            }
        }
    }
    return false;
}

bool VersionNumber::operator<(const VersionNumber &b) const
{
    return (b > *this);
}

bool VersionNumber::operator>=(const VersionNumber &b) const
{
    return (*this > b || *this == b);
}

bool VersionNumber::operator<=(const VersionNumber &b) const
{
    return (*this < b || *this == b);
}

bool VersionNumber::operator==(const VersionNumber &b) const
{
    if (m_Major==b.majorNumber() && m_Minor==b.minorNumber() && m_Debug==b.debugNumber()) {
        if ((m_IsAlpha && b.isAlpha()) && (m_Alpha==b.alphaNumber())) {
            return true;
        } else if ((m_IsBeta && b.isBeta()) && (m_Beta==b.betaNumber())) {
            return true;
        } else if ((m_IsRC && b.isRC()) && (m_RC==b.rcNumber())) {
            return true;
        } else {
            return (m_IsAlpha==b.isAlpha() && m_IsBeta==b.isBeta() && m_IsRC==b.isRC());
        }
    }
    return false;
}

bool VersionNumber::operator!=(const VersionNumber &b) const
{
    return (!(*this == b));
}

QDebug operator<<(QDebug dbg, const Utils::VersionNumber &c)
{
    QString t = QString("VersionNumber(maj:%1; min%2; deb:%3").arg(c.majorNumber()).arg(c.minorNumber()).arg(c.debugNumber());
    if (c.isAlpha())
        t += "; alpha:" + QString::number(c.alphaNumber());
    if (c.isBeta())
        t += "; beta:" + QString::number(c.betaNumber());
    t += ")";
    dbg.nospace() << t;
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const Utils::VersionNumber *c)
{
    if (!c) {
        dbg.nospace() << "Utils::VersionNumber(0x0)";
        return dbg.space();
    }
    return operator<<(dbg, *c);
}
