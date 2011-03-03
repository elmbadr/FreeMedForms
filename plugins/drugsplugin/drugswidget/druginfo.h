/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2010 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef DRUGINFO_H
#define DRUGINFO_H

#include <QDialog>
#include <QObject>

/**
 * \file druginfo.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.5.0
 * \date 12 Oct 2010
*/
namespace DrugsWidget {
namespace Internal {
class DrugInfoPrivate;

class DrugInfo : public QDialog
{
    Q_OBJECT
public:
    DrugInfo(const QVariant &drugUid, QWidget * parent = 0 );
    ~DrugInfo() {}

    void setDrug(const QVariant &drugUid);

protected Q_SLOTS:
    void accept();
    void reject();
    void done() { QDialog::done( QDialog::result() ); }

private:
    Internal::DrugInfoPrivate *d;
};

}  // End Internal
}  // End DrugsWidget

#endif  //  DRUGINFO
