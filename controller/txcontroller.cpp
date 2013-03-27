/**
  @file
  @author tipok
*/

#include "txcontroller.h"

#include "tableitem.h"

#include "requestmapper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"


TxController::TxController(QObject *parent) {
    rm_ptr = (RequestMapper *) parent;
}

void TxController::service(HttpRequest& request, HttpResponse& response) {
    MainWindow *mw_ptr = (MainWindow *) rm_ptr->mw_ptr;
    QMap<QString, TableItem *> map;

    //map.insert("", new TableItem(rm_ptr, "", "", mw_ptr->ui->));
    map.insert("preemphasis_id", new TableItem(rm_ptr, "preemphasis_id", "Preemphasis", mw_ptr->ui->comboBox_transmitter_preemph));
    map.insert("audio_deviation", new TableItem(rm_ptr, "audio_deviation", "Audio Deviation", mw_ptr->ui->spinBox_transmitter_audioDev));
    map.insert("pilot", new TableItem(rm_ptr, "pilot", "Pilot-Tone enabled", mw_ptr->ui->checkBox_transmitter_pilot));
    map.insert("pilot_deviation", new TableItem(rm_ptr, "pilot_deviation", "Pilot Deviation", mw_ptr->ui->spinBox_transmitter_pilot_devi));
    map.insert("pilot_frequency", new TableItem(rm_ptr, "pilot_frequency", "Pilot Frequency", mw_ptr->ui->spinBox_transmitter_pilot_freq));
    map.insert("lmr", new TableItem(rm_ptr, "lmr", "Enable Left-Right (stereo)", mw_ptr->ui->checkBox_transmitter_lmr));
    map.insert("rds", new TableItem(rm_ptr, "rds", "Enable RDS", mw_ptr->ui->checkBox_transmitter_rds));
    map.insert("rds_deviation", new TableItem(rm_ptr, "rds_deviation", "RDS Deviation", mw_ptr->ui->spinBox_transmitter_rds_devi));
    map.insert("tx_enabled", new TableItem(rm_ptr, "tx_enabled", "Transmission Enabled", mw_ptr->ui->checkBox_transmitter_enabled));
    map.insert("tx_frequency", new TableItem(rm_ptr, "tx_frequency", "Transmission Frequency", mw_ptr->ui->doubleSpinBox_transmitter_freq));
    map.insert("tx_power", new TableItem(rm_ptr, "tx_power", "Transmission Power", mw_ptr->ui->spinBox_transmitter_power));
    map.insert("tx_antcap", new TableItem(rm_ptr, "tx_antcap", "Manual tune antenna capacitor", mw_ptr->ui->checkBox_transmitter_antcap));
    map.insert("tx_antcap_value", new TableItem(rm_ptr, "tx_antcap_value", "Antenna Capacitor value", mw_ptr->ui->doubleSpinBox_transmitter_antcap_val));

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    QFile file(":/fmstick/html/index.html");
    if (file.open(QIODevice::ReadOnly)) {
        response.write(file.readAll());
        file.close();
    }

    response.write("<div class=\"logo_style\">"
                   "<img src=\"/images/1318446568_transmit_blue.png\">"
                   "<h2>Tx Configuration Controller</h2>");
    response.write("</div><div class=\"container\">");

    QString action(request.getParameter("action"));

    bool have_any_action=FALSE;
    if (map.contains(action)) {
        TableItem *table_data = map.value(action);
        response.write(table_data->doHTMLParse(&request));
        have_any_action=TRUE;
    }

    if(!have_any_action && action == "Tx_custom"){
        qDebug() << "TODO: custom_Tx action";
        have_any_action=TRUE;
    }
    if(have_any_action){
        response.write("<br/><a href=\"/tx\">[Back To Tx Section]</a> | <a href=\"/\">[Back To Main Section]</a>");
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
        QString Tx_custom = QString(
                    "<form method=\"post\">"
                    "  <input type=\"hidden\" name=\"action\" value=\"Tx_custom\">"
                    "<div class=\"form_info\">Custom Tx message:</div>"
                    "<div class=\"form_editor\">"
                    "<input type=\"text\" name=\"Tx_custom_b\" value=\"4000\" size=\"4\">"
                    "<input type=\"text\" name=\"Tx_custom_c\" value=\"0000\" size=\"4\">"
                    "<input type=\"text\" name=\"Tx_custom_d\" value=\"0000\" size=\"4\">"
                    "Circular:<input type=\"checkbox\" name=\"Tx_custom_circular\" value=\"0\">"
                    "</div>"
                    "<div class=\"form_submitter\"><input type=\"submit\" value=\"&gt;&gt;\"></div>"
                    "<div class=\"form_tooltip\"></div>"
                    "</form>");
         response.write(Tx_custom.toUtf8());
#endif
    }

    response.write("</div></div></div></body></html>", true);

}
