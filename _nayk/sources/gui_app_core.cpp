/****************************************************************************
** Copyright (c) 2019 Evgeny Teterin (nayk) <sutcedortal@gmail.com>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#include <QGuiApplication>
#include <QIcon>
#include <QFont>
#include <QFile>

#include "AppCore"
#include "FileSys"
#include "ImagesConst"
#include "gui_app_core.h"

namespace gui_app_core { //=====================================================

using namespace nayk;
using namespace file_sys;

//==============================================================================
bool initializeApplication(const QString &applicationName,
                                 const QString &applicationDisplayName,
                                 const QString &organizationName,
                                 const QString &organizationDomain)
{
    QGuiApplication *application = qobject_cast<QGuiApplication*>( QGuiApplication::instance() );
    if(!application) return false;

    if(!applicationDisplayName.isEmpty()) {
        application->setApplicationDisplayName(applicationDisplayName);
    }

    if(!app_core::initializeApplication(applicationName, organizationName, organizationDomain)) {
        return false;
    }

    if(fileExists(imageMainIcon)) {
        application->setWindowIcon(QIcon(imageMainIcon));
    }

    QFont f = application->font();
    f.setStyleStrategy(QFont::PreferAntialias);
    application->setFont(f);
    application->setQuitOnLastWindowClosed(true);

    return true;
}

} // namespace gui_app_core //==================================================
