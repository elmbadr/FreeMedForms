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
 *       NAME <MAIL@ADDRESS.COM>                                           *
 ***************************************************************************/
#ifndef IUSERVIEWERPAGE_H
#define IUSERVIEWERPAGE_H

#include <usermanagerplugin/usermanager_exporter.h>
#include <coreplugin/igenericpage.h>

/**
 * \file iuserviewerpage.h
 * \author Eric MAEKER <eric.maeker@gmail.com>
 * \version 0.6.0
 * \date 08 Jul 2011
*/

namespace UserPlugin {
class UserModel;

class USER_EXPORT IUserViewerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IUserViewerWidget(QWidget *parent = 0) : QWidget(parent) {}
    virtual ~IUserViewerWidget() {}

    virtual void setUserModel(UserModel *model) = 0;
    virtual void setUserIndex(const int row) = 0;
    virtual void clear() = 0;
    virtual bool submit() = 0;
};

class USER_EXPORT IUserViewerPage : public Core::IGenericPage
{
    Q_OBJECT
public:
    explicit IUserViewerPage(QObject *parent = 0) : Core::IGenericPage(parent) {}
    virtual ~IUserViewerPage() {}
};

}  // End namespace UserPlugin

#endif // IUSERVIEWERPAGE_H
