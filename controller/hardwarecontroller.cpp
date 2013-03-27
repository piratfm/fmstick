/**
  @file
  @author tipok
*/

#include "hardwarecontroller.h"

#include "tableitem.h"

#include "requestmapper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"


HardwareController::HardwareController(QObject *parent) {
    rm_ptr = (RequestMapper *) parent;
}

void HardwareController::service(HttpRequest& request, HttpResponse& response) {
    MainWindow *mw_ptr = (MainWindow *) rm_ptr->mw_ptr;
    QMap<QString, TableItem *> map;

    //map.insert("", new TableItem(rm_ptr, "", "", mw_ptr->ui->));
    map.insert("tx_at_powerup", new TableItem(rm_ptr, "tx_at_powerup", "Transmitter Auto Start", mw_ptr->ui->checkBox_hw_run_txAtPowerUp));
    map.insert("tx_fe_start", new TableItem(rm_ptr, "tx_fe_start", "Start Frontend", mw_ptr->ui->pushButton_fe_powerup));
    map.insert("tx_fe_stop", new TableItem(rm_ptr, "tx_fe_stop", "Stop Frontend", mw_ptr->ui->pushButton_fe_powerdown));
    map.insert("tx_fe_reset", new TableItem(rm_ptr, "tx_fe_reset", "Reset Frontend", mw_ptr->ui->pushButton_fe_reset));

    map.insert("hwid5", new TableItem(rm_ptr, "hwid5", "CPU ID", mw_ptr->ui->lineEdit_hw_id_cpuid));
    map.insert("hwid6", new TableItem(rm_ptr, "hwid6", "Firmware rev.", mw_ptr->ui->lineEdit_hw_id_cpufwid));
    map.insert("hwid0", new TableItem(rm_ptr, "hwid0", "TX Part", mw_ptr->ui->lineEdit_hw_id_txpart));
    map.insert("hwid1", new TableItem(rm_ptr, "hwid1", "TX Firmware rev.", mw_ptr->ui->lineEdit_hw_id_txfwrev));
    map.insert("hwid2", new TableItem(rm_ptr, "hwid2", "TX Patch ID", mw_ptr->ui->lineEdit_hw_id_txpatchid));
    map.insert("hwid3", new TableItem(rm_ptr, "hwid3", "TX Component rev.", mw_ptr->ui->lineEdit_hw_id_txcomprev));
    map.insert("hwid4", new TableItem(rm_ptr, "hwid4", "TX Chip rev.", mw_ptr->ui->lineEdit_hw_id_txchiprev));



    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    QFile file(":/fmstick/html/index.html");
    if (file.open(QIODevice::ReadOnly)) {
        response.write(file.readAll());
        file.close();
    }

    response.write("<div class=\"logo_style\">"
                   "<img src=\"/images/fw.png\">"
                   "<h2>Hardware Configuration Controller</h2>");
    response.write("</div><div class=\"container\">");

    QString action(request.getParameter("action"));

    bool have_any_action=FALSE;
    if (map.contains(action)) {
        TableItem *table_data = map.value(action);
        response.write(table_data->doHTMLParse(&request));
        have_any_action=TRUE;
    }

    if(!have_any_action && action == "Hardware_custom"){
        qDebug() << "TODO: custom_Hardware action";
        have_any_action=TRUE;
    }
    if(have_any_action){
        response.write("<br/><a href=\"/hw\">[Back To Hardware Section]</a> | <a href=\"/\">[Back To Main Section]</a>");
    } else {
        response.write("<form><input type=\"hidden\" name=\"action\" value=\"dummy\">"
                       "<div class=\"row\">"
                       "<div class=\"prop\">"
                       "<p><b>Property</b></p>"
                       "</div>"
                       "<div class=\"val\">"
                       "<p><b>Value</b></p>"
                       "</div>"
                       "<div class=\"submit\">"
                       "<p><b>Action</b></p>"
                       "</div>"
                       "<div class=\"tooltip\">"
                       "<p><b>Tooltip</b></p>"
                       "</div>"
                       "</div></form>");
        QMapIterator<QString, TableItem *> i(map);
        while (i.hasNext()) {
            i.next();
            response.write(i.value()->GenHTMLForm());
        }
#if 0
        /* forcer */
        QString Hardware_custom = QString(
                    "<form method=\"post\">"
                    "  <input type=\"hidden\" name=\"action\" value=\"Hardware_custom\">"
                    "<div class=\"form_info\">Custom Hardware message:</div>"
                    "<div class=\"form_editor\">"
                    "<input type=\"text\" name=\"Hardware_custom_b\" value=\"4000\" size=\"4\">"
                    "<input type=\"text\" name=\"Hardware_custom_c\" value=\"0000\" size=\"4\">"
                    "<input type=\"text\" name=\"Hardware_custom_d\" value=\"0000\" size=\"4\">"
                    "Circular:<input type=\"checkbox\" name=\"Hardware_custom_circular\" value=\"0\">"
                    "</div>"
                    "<div class=\"form_submitter\"><input type=\"submit\" value=\"&gt;&gt;\"></div>"
                    "<div class=\"form_tooltip\"></div>"
                    "</form>");
         response.write(Hardware_custom.toUtf8());
#endif
    }

    response.write("</div></div></div></body></html>", true);

}
