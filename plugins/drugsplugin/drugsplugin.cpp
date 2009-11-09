/***************************************************************************
 *   FreeMedicalForms                                                      *
 *   Copyright (C) 2008-2009 by Eric MAEKER                                *
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
#include "drugsplugin.h"
#include "drugswidgetfactory.h"
#include "constants.h"
#include "drugspreferences/mfDrugsPreferences.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/log.h>

#include <coreplugin/dialogs/pluginaboutpage.h>
#include <coreplugin/icore.h>
#include <coreplugin/isettings.h>
#include <coreplugin/translators.h>

#include <drugsbaseplugin/drugsbase.h>

#include <QtCore/QtPlugin>
#include <QDebug>

using namespace DrugsWidget::Internal;
//using namespace DrugsWidget;

static inline DrugsDB::Internal::DrugsBase *drugsBase() {return DrugsDB::Internal::DrugsBase::instance();}

DrugsPlugin::DrugsPlugin() :
        viewPage(0),
        printPage(0),
        userPage(0),
        extraPage(0)
{
    setObjectName("DrugsPlugin");
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "creating DrugsPlugin";
}

DrugsPlugin::~DrugsPlugin()
{
    if (viewPage) {
        removeObject(viewPage);
        delete viewPage; viewPage=0;
    }
    if (userPage) {
        removeObject(userPage);
        delete userPage; userPage=0;
    }
    if (extraPage) {
        removeObject(extraPage);
        delete extraPage; extraPage=0;
    }
    if (printPage) {
        removeObject(printPage);
        delete printPage; printPage=0;
    }
}

bool DrugsPlugin::initialize(const QStringList &arguments, QString *errorMessage)
{
    Q_UNUSED(arguments);
    Q_UNUSED(errorMessage);
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsPlugin::initialize";
#ifdef FREEDIAMS
    Utils::Log::addMessage(this,"Running as FreeDiams");
#endif
    addAutoReleasedObject(new Core::PluginAboutPage(this->pluginSpec(), this));
    addAutoReleasedObject(new DrugsWidgetsFactory(this));

    // Initialize drugs database
    drugsBase();

    return true;
}

void DrugsPlugin::extensionsInitialized()
{
    if (Utils::Log::warnPluginsCreation())
        qWarning() << "DrugsPlugin::extensionsInitialized";

    // Add Translator to the Application
    Core::ICore::instance()->translators()->addNewTranslator("mfDrugsWidget");

    viewPage = new DrugsViewOptionsPage(this);
    printPage = new DrugsPrintOptionsPage(this);
    userPage = new DrugsUserOptionsPage(this);
    extraPage = new DrugsExtraOptionsPage(this);
    // check settings
    if (!Core::ICore::instance()->settings()->value(Constants::S_CONFIGURED, false).toBool()) {
        viewPage->writeDefaultSettings(Core::ICore::instance()->settings());
        printPage->writeDefaultSettings(Core::ICore::instance()->settings());
        userPage->writeDefaultSettings(Core::ICore::instance()->settings());
        extraPage->writeDefaultSettings(Core::ICore::instance()->settings());
    } else {
        viewPage->checkSettingsValidity();
        printPage->checkSettingsValidity();
        userPage->checkSettingsValidity();
        extraPage->checkSettingsValidity();
    }

    addObject(viewPage);
    addObject(printPage);
    addObject(userPage);
    addObject(extraPage);
}

void DrugsPlugin::remoteArgument(const QString& arg)
{
    qWarning() << "drugplugin" << arg;
    // An empty argument is sent to trigger activation
    // of the window via QtSingleApplication. It should be
    // the last of a sequence.
//    if (arg.isEmpty()) {
//        m_mainWindow->activateWindow();
//    } else {
//        m_mainWindow->openFiles(QStringList(arg));
//    }
}

Q_EXPORT_PLUGIN(DrugsPlugin)
