/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   (C) 2008-2010 by Eric MAEKER, MD                                      *
 *   eric.maeker@free.fr                                                   *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   This program is a free and open source software.                      *
 *   It is released under the terms of the new BSD License.                *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *   - Redistributions of source code must retain the above copyright      *
 *   notice, this list of conditions and the following disclaimer.         *
 *   - Redistributions in binary form must reproduce the above copyright   *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *   - Neither the name of the FreeMedForms' organization nor the names of *
 *   its contributors may be used to endorse or promote products derived   *
 *   from this software without specific prior written permission.         *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS     *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,  *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      *
 *   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER      *
 *   CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT    *
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     *
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
 *   POSSIBILITY OF SUCH DAMAGE.                                           *
 ***************************************************************************/
/***************************************************************************
 *   Main Developper : Eric MAEKER, <eric.maeker@free.fr>                  *
 *   Contributors :                                                        *
 *       NAME <MAIL@ADRESS>                                                *
 *       NAME <MAIL@ADRESS>                                                *
 ***************************************************************************/
#include "patientbasepreferencespage.h"
#include "constants_settings.h"
#include "constants_db.h"
#include "patientselector.h"
#include "patientbase.h"

#include <utils/log.h>
#include <utils/randomizer.h>
#include <translationutils/constanttranslations.h>

#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/theme.h>

#include <usermanagerplugin/usermodel.h>

#include <QUuid>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QProgressDialog>


using namespace Patients;
using namespace Internal;
using namespace Trans::ConstantTranslations;


static inline Core::ISettings *settings() { return Core::ICore::instance()->settings(); }
static inline Core::ITheme *theme()  { return Core::ICore::instance()->theme(); }
static inline Patients::Internal::PatientBase *patientBase()  { return Patients::Internal::PatientBase::instance(); }
static inline UserPlugin::UserModel *userModel() {return UserPlugin::UserModel::instance();}


PatientBasePreferencesPage::PatientBasePreferencesPage(QObject *parent) :
        IOptionsPage(parent), m_Widget(0)
{ setObjectName("PatientBasePreferencesPage"); }

PatientBasePreferencesPage::~PatientBasePreferencesPage()
{
    if (m_Widget) delete m_Widget;
    m_Widget = 0;
}

QString PatientBasePreferencesPage::id() const { return objectName(); }
QString PatientBasePreferencesPage::name() const { return tkTr(Trans::Constants::PATIENTS); }
QString PatientBasePreferencesPage::category() const { return tkTr(Trans::Constants::PATIENT_DATABASE); }

void PatientBasePreferencesPage::resetToDefaults()
{
    m_Widget->writeDefaultSettings(settings());
}

void PatientBasePreferencesPage::applyChanges()
{
    if (!m_Widget) {
        return;
    }
    m_Widget->saveToSettings(settings());
}

void PatientBasePreferencesPage::finish() { delete m_Widget; }

void PatientBasePreferencesPage::checkSettingsValidity()
{
    QHash<QString, QVariant> defaultvalues;
    defaultvalues.insert(Constants::S_SELECTOR_FIELDSTOSHOW, PatientSelector::Default);
    defaultvalues.insert(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, true);
    defaultvalues.insert(Constants::S_SELECTOR_USEGENDERCOLORS, true);

    foreach(const QString &k, defaultvalues.keys()) {
        if (settings()->value(k) == QVariant())
            settings()->setValue(k, defaultvalues.value(k));
    }
    settings()->sync();
}

QWidget *PatientBasePreferencesPage::createPage(QWidget *parent)
{
    if (m_Widget)
        delete m_Widget;
    m_Widget = new PatientBasePreferencesWidget(parent);
    return m_Widget;
}



PatientBasePreferencesWidget::PatientBasePreferencesWidget(QWidget *parent) :
        QWidget(parent)
{
    setupUi(this);
    setDatasToUi();
}

void PatientBasePreferencesWidget::setDatasToUi()
{
    showVirtualPatients->setChecked(settings()->value(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS).toBool());
    genderColor->setChecked(settings()->value(Constants::S_SELECTOR_USEGENDERCOLORS).toBool());
}

void PatientBasePreferencesWidget::saveToSettings(Core::ISettings *sets)
{
    Core::ISettings *s;
    if (!sets)
        s = settings();
    else
        s = sets;

    s->setValue(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, showVirtualPatients->isChecked());
    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, genderColor->isChecked());
}

void PatientBasePreferencesWidget::writeDefaultSettings(Core::ISettings *s)
{
    //    qWarning() << "---------> writedefaults";
    Utils::Log::addMessage("PatientBasePreferencesWidget", tkTr(Trans::Constants::CREATING_DEFAULT_SETTINGS_FOR_1).arg("FreeDiamsMainWindow"));
    s->setValue(Constants::S_SELECTOR_SHOWVIRTUALPATIENTS, true);
    s->setValue(Constants::S_SELECTOR_USEGENDERCOLORS, true);
    s->sync();
}

void PatientBasePreferencesWidget::on_populateDb_clicked()
{
    QSqlQuery query(patientBase()->database());
    QHash<int, QString> where;
    where.insert(Constants::IDENTITY_NAME, "LIKE 'DOE'");
    int c = patientBase()->count(Constants::Table_IDENT, Constants::IDENTITY_NAME, patientBase()->getWhereClause(Constants::Table_IDENT, where));
    if (!c) {
        query.finish();
        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_IDENT));
        query.bindValue(Constants::IDENTITY_ISACTIVE, 1);
        query.bindValue(Constants::IDENTITY_NAME, "DOE");
        query.bindValue(Constants::IDENTITY_SURNAME, "John");
        query.bindValue(Constants::IDENTITY_SECONDNAME, "Junior");
        query.bindValue(Constants::IDENTITY_GENDER, "M");
        query.bindValue(Constants::IDENTITY_TITLE, 1);
        query.bindValue(Constants::IDENTITY_DOB, QDate(1990, 10, 20));
        query.bindValue(Constants::IDENTITY_ADDRESS_COUNTRY, "France");
        query.bindValue(Constants::IDENTITY_ADDRESS_NOTE, "Patient is lost...");
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET, "21, street Doe");
        query.bindValue(Constants::IDENTITY_ADDRESS_ZIPCODE, "21213");
        query.bindValue(Constants::IDENTITY_ADRESS_CITY, "NoWhereLand");
        query.bindValue(Constants::IDENTITY_UID, QUuid::createUuid().toString());
        query.bindValue(Constants::IDENTITY_FAMILY_UID, "Not yet implemented");
        query.bindValue(Constants::IDENTITY_ISVIRTUAL, 1);

        query.bindValue(Constants::IDENTITY_MARITAL_STATUS, QVariant());
        query.bindValue(Constants::IDENTITY_FAXES, QVariant());
        query.bindValue(Constants::IDENTITY_TELS, QVariant());
        query.bindValue(Constants::IDENTITY_DATEOFDEATH, QVariant());
        query.bindValue(Constants::IDENTITY_ID, QVariant());
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::IDENT_VERSION, QVariant());
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET_NUMBER, QVariant());
        query.bindValue(Constants::IDENTITY_MAILS, QVariant());

        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        query.finish();

        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_IDENT));
        query.bindValue(Constants::IDENTITY_ISACTIVE, 1);
        query.bindValue(Constants::IDENTITY_NAME, "DOE");
        query.bindValue(Constants::IDENTITY_SURNAME, "Jane");
        query.bindValue(Constants::IDENTITY_SECONDNAME, "ComicsGirl");
        query.bindValue(Constants::IDENTITY_GENDER, "F");
        query.bindValue(Constants::IDENTITY_TITLE, 2);
        query.bindValue(Constants::IDENTITY_DOB, QDate(1985, 04, 20));
        query.bindValue(Constants::IDENTITY_ADDRESS_COUNTRY, "France");
        query.bindValue(Constants::IDENTITY_ADDRESS_NOTE, "SuperJane owns to an unknown user");
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET, "21, street Jane");
        query.bindValue(Constants::IDENTITY_ADDRESS_ZIPCODE, "54545");
        query.bindValue(Constants::IDENTITY_ADRESS_CITY, "JaneLand");
        query.bindValue(Constants::IDENTITY_UID, QUuid::createUuid().toString());
        query.bindValue(Constants::IDENTITY_FAMILY_UID, "Not yet implemented");
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 2);
        query.bindValue(Constants::IDENTITY_ISVIRTUAL, 1);

        query.bindValue(Constants::IDENTITY_MARITAL_STATUS, QVariant());
        query.bindValue(Constants::IDENTITY_FAXES, QVariant());
        query.bindValue(Constants::IDENTITY_TELS, QVariant());
        query.bindValue(Constants::IDENTITY_DATEOFDEATH, QVariant());
        query.bindValue(Constants::IDENTITY_ID, QVariant());
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::IDENT_VERSION, QVariant());
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET_NUMBER, QVariant());
        query.bindValue(Constants::IDENTITY_MAILS, QVariant());

        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        query.finish();

        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_IDENT));
        query.bindValue(Constants::IDENTITY_ISACTIVE, 1);
        query.bindValue(Constants::IDENTITY_NAME, "KIRK");
        query.bindValue(Constants::IDENTITY_SURNAME, "James Tiberius");
        query.bindValue(Constants::IDENTITY_SECONDNAME, "");
        query.bindValue(Constants::IDENTITY_TITLE, 6);
        query.bindValue(Constants::IDENTITY_GENDER, "M");
        query.bindValue(Constants::IDENTITY_DOB, QDate(1968, 04, 20));
        query.bindValue(Constants::IDENTITY_ADDRESS_COUNTRY, "USA");
        query.bindValue(Constants::IDENTITY_ADDRESS_NOTE, "USS Enterprise");
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET, "21, StarFleet Command");
        query.bindValue(Constants::IDENTITY_ADDRESS_ZIPCODE, "1968");
        query.bindValue(Constants::IDENTITY_ADRESS_CITY, "EarthTown");
        query.bindValue(Constants::IDENTITY_FAMILY_UID, "Not yet implemented");
        query.bindValue(Constants::IDENTITY_UID, QUuid::createUuid().toString());
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::IDENTITY_ISVIRTUAL, 1);

        query.bindValue(Constants::IDENTITY_MARITAL_STATUS, QVariant());
        query.bindValue(Constants::IDENTITY_FAXES, QVariant());
        query.bindValue(Constants::IDENTITY_TELS, QVariant());
        query.bindValue(Constants::IDENTITY_DATEOFDEATH, QVariant());
        query.bindValue(Constants::IDENTITY_ID, QVariant());
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::IDENT_VERSION, QVariant());
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET_NUMBER, QVariant());
        query.bindValue(Constants::IDENTITY_MAILS, QVariant());

        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        query.finish();

        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_IDENT));
        query.bindValue(Constants::IDENTITY_ISACTIVE, 1);
        query.bindValue(Constants::IDENTITY_NAME, "PICARD");
        query.bindValue(Constants::IDENTITY_SURNAME, "Jean-Luc");
        query.bindValue(Constants::IDENTITY_SECONDNAME, "");
        query.bindValue(Constants::IDENTITY_TITLE, 6);
        query.bindValue(Constants::IDENTITY_GENDER, "M");
        query.bindValue(Constants::IDENTITY_DOB, QDate(1968, 04, 20));
        query.bindValue(Constants::IDENTITY_ADDRESS_COUNTRY, "USA");
        query.bindValue(Constants::IDENTITY_ADDRESS_NOTE, "USS Enterprise");
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET, "21, StarFleet Command");
        query.bindValue(Constants::IDENTITY_ADDRESS_ZIPCODE, "2333");
        query.bindValue(Constants::IDENTITY_ADRESS_CITY, "EarthTown");
        query.bindValue(Constants::IDENTITY_FAMILY_UID, "Not yet implemented");
        query.bindValue(Constants::IDENTITY_UID, QUuid::createUuid().toString());
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::IDENTITY_ISVIRTUAL, 1);

        query.bindValue(Constants::IDENTITY_MARITAL_STATUS, QVariant());
        query.bindValue(Constants::IDENTITY_FAXES, QVariant());
        query.bindValue(Constants::IDENTITY_TELS, QVariant());
        query.bindValue(Constants::IDENTITY_DATEOFDEATH, QVariant());
        query.bindValue(Constants::IDENTITY_ID, QVariant());
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::IDENT_VERSION, QVariant());
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET_NUMBER, QVariant());
        query.bindValue(Constants::IDENTITY_MAILS, QVariant());

        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        query.finish();

        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_IDENT));
        query.bindValue(Constants::IDENTITY_ISACTIVE, 1);
        query.bindValue(Constants::IDENTITY_NAME, "HIPPOCRATE");
        query.bindValue(Constants::IDENTITY_SURNAME, "Le Grand");
        query.bindValue(Constants::IDENTITY_SECONDNAME, "");
        query.bindValue(Constants::IDENTITY_TITLE, 4);
        query.bindValue(Constants::IDENTITY_GENDER, "M");
        query.bindValue(Constants::IDENTITY_DOB, QDate(-460, 01, 01));
        query.bindValue(Constants::IDENTITY_DATEOFDEATH, QDate(-370, 01, 01));
        query.bindValue(Constants::IDENTITY_ADDRESS_COUNTRY, "Greece");
        query.bindValue(Constants::IDENTITY_ADDRESS_NOTE, "");
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET, "Heaven");
        query.bindValue(Constants::IDENTITY_ADDRESS_ZIPCODE, "------");
        query.bindValue(Constants::IDENTITY_ADRESS_CITY, "-------");
        query.bindValue(Constants::IDENTITY_FAMILY_UID, "Not yet implemented");
        query.bindValue(Constants::IDENTITY_UID, QUuid::createUuid().toString());
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::IDENTITY_ISVIRTUAL, 1);

        query.bindValue(Constants::IDENTITY_MARITAL_STATUS, QVariant());
        query.bindValue(Constants::IDENTITY_FAXES, QVariant());
        query.bindValue(Constants::IDENTITY_TELS, QVariant());
        query.bindValue(Constants::IDENTITY_ID, QVariant());
        query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::IDENT_VERSION, QVariant());
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET_NUMBER, QVariant());
        query.bindValue(Constants::IDENTITY_MAILS, QVariant());

        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        query.finish();

        // create links
        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_LK_TOPRACT));
        query.bindValue(Constants::LK_TOPRACT_ID, QVariant());
        query.bindValue(Constants::LK_TOPRACT_LKID, 1);
        query.bindValue(Constants::LK_TOPRACT_GROUP_UUID, QVariant());
        query.bindValue(Constants::LK_TOPRACT_PRACT_UUID, userModel()->currentUserData(UserPlugin::User::Uuid).toString());

        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        query.finish();

        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_LK_TOPRACT));
        query.bindValue(Constants::LK_TOPRACT_ID, QVariant());
        query.bindValue(Constants::LK_TOPRACT_LKID, 2);
        query.bindValue(Constants::LK_TOPRACT_GROUP_UUID, QVariant());
        query.bindValue(Constants::LK_TOPRACT_PRACT_UUID, QUuid::createUuid().toString());

        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        query.finish();
    }

    // Prepare virtual patients
    int nb = nbVirtualPatients->value();
    QProgressDialog dlg(tr("Creating %1 virtual patients").arg(nb), tr("Cancel"), 0, nb, qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    Utils::Randomizer r;
    r.setPathToFiles(settings()->path(Core::ISettings::BundleResourcesPath) + "/textfiles/");

    for(int i = 0; i < nb; ++i) {
        if (i % 100 == 0) {
            dlg.setValue(i);
            patientBase()->database().transaction();
        }

        query.prepare(patientBase()->prepareInsertQuery(Constants::Table_IDENT));
        query.bindValue(Constants::IDENTITY_NAME, r.getRandomName());
        query.bindValue(Constants::IDENTITY_ISVIRTUAL, 1);
        query.bindValue(Constants::IDENTITY_DOB, r.randomDate(1910));

        if (r.randomInt(2) == 1) {
            query.bindValue(Constants::IDENTITY_GENDER, "F");
            query.bindValue(Constants::IDENTITY_SURNAME, r.getRandomSurname(false));
            query.bindValue(Constants::IDENTITY_SECONDNAME, r.getRandomName());
            query.bindValue(Constants::IDENTITY_TITLE, 2);
        } else {
            query.bindValue(Constants::IDENTITY_GENDER, "M");
            query.bindValue(Constants::IDENTITY_SURNAME, r.getRandomSurname(true));
            query.bindValue(Constants::IDENTITY_SECONDNAME, QVariant());
            query.bindValue(Constants::IDENTITY_TITLE, 1);
        }

        if (r.randomInt(2)) {
            query.bindValue(Constants::IDENTITY_DATEOFDEATH, r.randomDate(1980));
            query.bindValue(Constants::IDENTITY_ISACTIVE, 0);
        } else {
            query.bindValue(Constants::IDENTITY_DATEOFDEATH, QVariant());
            query.bindValue(Constants::IDENTITY_ISACTIVE, 1);
        }

        query.bindValue(Constants::IDENTITY_ADDRESS_COUNTRY, "France");
        query.bindValue(Constants::IDENTITY_ADDRESS_NOTE, "");
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET, r.getRandomString(65));
        QPair<int, QString> p = r.getRandomFrenchCity();
        query.bindValue(Constants::IDENTITY_ADDRESS_ZIPCODE, p.first);
        query.bindValue(Constants::IDENTITY_ADRESS_CITY, p.second);

        query.bindValue(Constants::IDENTITY_FAMILY_UID, "Not yet implemented");
        query.bindValue(Constants::IDENTITY_UID, QUuid::createUuid().toString());
        if (r.randomInt(3) == 1)
            query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 2);
        else
            query.bindValue(Constants::IDENTITY_LK_TOPRACT_LKID, 1);

        /** \todo add marital status, faxes, tels and mails to dummy patients */
        query.bindValue(Constants::IDENTITY_MARITAL_STATUS, QVariant());
        query.bindValue(Constants::IDENTITY_FAXES, QVariant());
        query.bindValue(Constants::IDENTITY_TELS, QVariant());
        query.bindValue(Constants::IDENTITY_ID, QVariant());
        query.bindValue(Constants::IDENT_VERSION, QVariant());
        query.bindValue(Constants::IDENTITY_ADDRESS_STREET_NUMBER, QVariant());
        query.bindValue(Constants::IDENTITY_MAILS, QVariant());

        if (!query.exec()) {
            Utils::Log::addQueryError(this, query);
        }
        query.finish();

        if (i % 100 == 99)
            patientBase()->database().commit();

        qApp->processEvents();
        if (dlg.wasCanceled())
            break;
    }
    patientBase()->database().commit();

}

void PatientBasePreferencesWidget::on_populateEpisodes_clicked()
{
    // Prepare virtual episodes
    QSqlQuery query(patientBase()->database());
    int nb = nbVirtualEpisodes->value();
    Utils::Randomizer r;

    // Retreive all patients and create n episodes for them
    QSet<QString> patients;
    query.exec(patientBase()->selectDistinct(Constants::Table_IDENT, Constants::IDENTITY_UID));
    if (query.isActive()) {
        while (query.next())
            patients << query.value(0).toString();
    } else {
        Utils::Log::addQueryError(this, query);
    }

    QProgressDialog dlg(tr("Creating %1 virtual episodes").arg(nb * patients.count()), tr("Cancel"), 0, nb * patients.count(), qApp->activeWindow());
    dlg.setWindowModality(Qt::WindowModal);

    foreach(const QString &uid, patients) {
        QDateTime date = QDateTime(r.randomDate(2008), QTime(r.randomInt(23), r.randomInt(59), r.randomInt(59), 0));

//        qWarning()<<"xxxxxxxxxxxxxxxxx" << date << uid;

        for(int i = 0; i < nb; ++i) {
            if (i % 100 == 0) {
                dlg.setValue(i);
                patientBase()->database().transaction();
            }
            date = r.randomDateTime(date);
//            qWarning() << date;
            if (date > QDateTime::currentDateTime())
                break;

            query.prepare(patientBase()->prepareInsertQuery(Constants::Table_EPISODES));
            query.bindValue(Constants::EPISODES_ID, QVariant());
            query.bindValue(Constants::EPISODES_PATIENT_UID, uid);
            if (r.randomInt(3) == 1)
                query.bindValue(Constants::EPISODES_LK_TOPRACT_LKID, 2);
            else
                query.bindValue(Constants::EPISODES_LK_TOPRACT_LKID, 1);
            query.bindValue(Constants::EPISODES_DATE, date);
            query.bindValue(Constants::EPISODES_DATEOFCREATION, date);
            query.bindValue(Constants::EPISODES_DATEOFMODIFICATION, QVariant());
            query.bindValue(Constants::EPISODES_DATEOFVALIDATION, QVariant());
            query.bindValue(Constants::EPISODES_VALIDATED, QVariant());

            if (!query.exec()) {
                Utils::Log::addQueryError(this, query);
            }
            query.finish();

            if (i % 100 == 99)
                patientBase()->database().commit();

            qApp->processEvents();
            if (dlg.wasCanceled())
                break;
        }
        patientBase()->database().commit();
    }
}

void PatientBasePreferencesWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}
