/***************************************************************************
 *  The FreeMedForms project is a set of free, open source medical         *
 *  applications.                                                          *
 *  (C) 2008-2011 by Eric MAEKER, MD (France) <eric.maeker@free.fr>        *
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
 *       Pierre-Marie DESOMBRE <pm.desombre@medsyn.fr>                     *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#ifndef ASSETSRATESPAGE_H
#define ASSETSRATESPAGE_H
#include "ui_assetsratespage.h"
#include <coreplugin/ioptionspage.h>
#include <accountbaseplugin/assetsratesmodel.h>

#include <QPointer>
#include <QString>
#include <QDataWidgetMapper>
#include <QHash>
#include <QSpinBox>
#include <QStandardItemModel>



namespace Core {
class ISettings;
}

namespace AccountDB {
class AssetsRatesModel;
}

namespace Account {
namespace Internal {

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  AssetsRatesWidget  ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class AssetsRatesWidget : public QWidget, private Ui::AssetsRatesWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(AssetsRatesWidget)

public:
    explicit AssetsRatesWidget(QWidget *parent = 0);
    ~AssetsRatesWidget();
    void setDatasToUi();

    static void writeDefaultSettings( Core::ISettings *s );

public Q_SLOTS:
    void saveToSettings(Core::ISettings *s = 0);
    void on_assetsNameComboBox_currentIndexChanged(int index);
    void on_addButton_clicked();
    void on_deleteButton_clicked();
    void changeSpinBoxes(int index);

private:
    void changeEvent(QEvent *e);
    void saveModel();
    bool insertYearsRange();
    QString calcAssetsRatesUid();

private Q_SLOTS:
    void createDefaultAssetsRates();

private:
    QStandardItemModel *assetsRatesModelByLocale();
    bool writeDefaultsWithLocale();

private:
    AccountDB::AssetsRatesModel *m_Model;
    QDataWidgetMapper *m_Mapper;
    QString m_user_uid;
};

}  // End Internal


class AssetsRatesPage : public Core::IOptionsPage
{
public:
    AssetsRatesPage(QObject *parent = 0);
    ~AssetsRatesPage();

    QString id() const;
    QString name() const;
    QString category() const;

    void resetToDefaults();
    void checkSettingsValidity();
    void applyChanges();
    void finish();

    QString helpPage() {return QString();}

    static void writeDefaultSettings(Core::ISettings *s) {Internal::AssetsRatesWidget::writeDefaultSettings(s);}

    QWidget *createPage(QWidget *parent = 0);
private:
    QPointer<Internal::AssetsRatesWidget> m_Widget;
};

}  // End namespace Account

#endif

