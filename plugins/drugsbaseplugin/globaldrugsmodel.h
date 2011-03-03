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
#ifndef GLOBALDRUGSMODEL_H
#define GLOBALDRUGSMODEL_H

#include <drugsbaseplugin/drugsbase_exporter.h>

#include <QSqlQueryModel>
#include <QObject>
class QStandardItemModel;

/**
 * \file globaldrugsmodel.h
 * \author Eric MAEKER <eric.maeker@free.fr>
 * \version 0.6.0
 * \date 28 Feb 2011
*/

namespace DrugsDB {
class IDrug;

namespace Internal {
class GlobalDrugsModelPrivate;
}  // end namespace Internal


class DRUGSBASE_EXPORT GlobalDrugsModel : public QSqlQueryModel
{
    Q_OBJECT
    friend class Internal::GlobalDrugsModelPrivate;

public:
    enum DataRepresentation {
        BrandName = 0,
        Strength,
        Routes,
        Forms,
        Marketed,
        DrugId,
        ColumnCount
    };

    enum SearchMode {
        SearchByBrandName = 0,
        SearchByMolecularName,
        SearchByInnName
    };

    GlobalDrugsModel(const SearchMode searchMode = SearchByBrandName, QObject * parent = 0);
    ~GlobalDrugsModel();

    void setSearchMode(const int searchMode);

    static void updateAvailableDosages();

    static bool hasAllergy(const IDrug *drug);
    static bool hasIntolerance(const IDrug *drug);

    static QStandardItemModel *drugsPrecautionsModel();

    int columnCount(const QModelIndex & = QModelIndex()) const { return ColumnCount; }

    bool setData(const QModelIndex &, const QVariant &, int = Qt::EditRole) { return false; }
    QVariant data(const QModelIndex & item, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void setFilter(const QString &searchFor);

public Q_SLOTS:
    void updateCachedAvailableDosage();
    void onDrugsDatabaseChanged();

private Q_SLOTS:
    void refreshDrugsPrecautions(const QModelIndex &topleft, const QModelIndex &bottomright);
//    void physiologyProcessed();
//    void processPhysiology();

private:
    Internal::GlobalDrugsModelPrivate *d;
};


}  // end namespace DrugsDB


#endif // GLOBALDRUGSMODEL_H
