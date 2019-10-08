/* Copyright (C) 2019 J.F.Dockes
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "autoconfig.h"
#include "winschedtool.h"

#include <stdio.h>
#include <string>

#include <QPushButton>
#include <QMessageBox>

#include "recoll.h"
#include "smallut.h"
#include "rclutil.h"
#include "log.h"
#include "execmd.h"

using namespace std;

void WinSchedToolW::init()
{
    if (!theconfig) {
	QMessageBox::warning(0, tr("Error"), 
			     tr("Configuration not initialized"));
	return;
    }

    connect(startPB, SIGNAL(clicked()), this, SLOT(startWinScheduler()));

    // thisexecpath returns the directory
    string recollindex = path_cat(path_thisexecpath(), "recollindex.exe");
    LOGDEB("WinSchedTool: recollindex: " << recollindex << endl);

    string batchfile = path_cat(theconfig->getConfDir(), "winsched.bat");
    LOGDEB("WinSchedTool: batch file " << batchfile << endl);

    if (!path_exists(batchfile)) {
        FILE *fp = fopen(batchfile.c_str(), "w");
        fprintf(fp, "%s -c %s\n", recollindex.c_str(),
                theconfig->getConfDir().c_str());
        fclose(fp);
    }
    QString blurb = tr(
        "<h3>Recoll indexing batch scheduling</h3>"
        "<p>We use the standard Windows task scheduler for this. The program "
        "will be started when you click the button below.</p>"
        "<p>You can use either the full interface "
        "(<i>Create task</i> in the menu on the right), or the simplified "
        "<i>Create Basic task</i> wizard. In both cases Copy/Paste the "
        "batch file path listed below as the <i>Action</i> to be performed."
        "Once created, you will find your task in the <i>WPD</i> section "
        "of the task library.</p>" 
        );

    blurb.append("</p><p><tt>").append(u8s2qs(batchfile)).append("</tt></p>");
    explainLBL->setText(blurb);
    explainLBL->setTextInteractionFlags(Qt::TextSelectableByMouse);
}

void WinSchedToolW::startWinScheduler()
{
    if (m_cmd) {
        int status;
        if (m_cmd->maybereap(&status)) {
            delete m_cmd;
        } else {
            QMessageBox::warning(0, "Recoll", 
                                 tr("Command already started"));
            return;
        }
    }
    m_cmd = new ExecCmd();
    vector<string> lcmd{"c:/windows/system32/taskschd.msc"};
    m_cmd->startExec("rclstartw", lcmd, false, false);
}
