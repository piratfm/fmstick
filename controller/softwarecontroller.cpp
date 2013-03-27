/**
  @file
  @author tipok
*/

#include "softwarecontroller.h"

#include "tableitem.h"

#include "requestmapper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"


SoftwareController::SoftwareController(QObject *parent) {
    rm_ptr = (RequestMapper *) parent;
}

void SoftwareController::service(HttpRequest& request, HttpResponse& response) {
    MainWindow *mw_ptr = (MainWindow *) rm_ptr->mw_ptr;
    QMap<QString, TableItem *> map;

    //map.insert("", new TableItem(rm_ptr, "", "", mw_ptr->ui->));
    map.insert("mode", new TableItem(rm_ptr, "mode", "run in Mini Mide", mw_ptr->ui->checkBox_mini_mode));
    //map.insert("toolbar", new TableItem(rm_ptr, "toolbar", "Show ToolBar", mw_ptr->toolBarIsVisible);
    map.insert("ontop", new TableItem(rm_ptr, "ontop", "be on top in mini mode", mw_ptr->ui->checkBox_be_on_the_top));
    map.insert("intray", new TableItem(rm_ptr, "intray", "hide in tray after start-up", mw_ptr->ui->checkBox_sw_hideInTray));
    //map.insert("language", new TableItem(rm_ptr, "language", "", mw_ptr->UsedLanguage);

    map.insert("flag", new TableItem(rm_ptr, "flag", "Send RDS time stamps", mw_ptr->ui->checkBox_rds_time));
    map.insert("interval", new TableItem(rm_ptr, "interval", "RDS time stamps interval", mw_ptr->ui->spinBox_rds_time_int));

    map.insert("softcfg", new TableItem(rm_ptr, "softcfg", "Use software config instead of EEPROM", mw_ptr->ui->checkBox_rewriteCfg));
    map.insert("autoconnect", new TableItem(rm_ptr, "autoconnect", "Auto-connect device to FmStick software", mw_ptr->ui->checkBox_sw_autoconnect));
    map.insert("liveedit", new TableItem(rm_ptr, "liveedit", "Enable live-editing device's parameters", mw_ptr->ui->checkBox_sw_live_edit));
    map.insert("http_serve", new TableItem(rm_ptr, "http_serve", "Enable remote HTTP access (WARNING!)", mw_ptr->ui->checkBox_sw_remote));
    map.insert("http_port", new TableItem(rm_ptr, "http_port", "HTTP port (WARNING!)", mw_ptr->ui->spinBox_sw_remote_port));
    map.insert("http_login", new TableItem(rm_ptr, "http_login", "HTTP login (WARNING!)", mw_ptr->ui->lineEdit_sw_remote_login));
    map.insert("http_password", new TableItem(rm_ptr, "http_password", "HTTP password (WARNING!)", mw_ptr->ui->lineEdit_sw_remote_pwd));

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    QFile file(":/fmstick/html/index.html");
    if (file.open(QIODevice::ReadOnly)) {
        response.write(file.readAll());
        file.close();
    }

    response.write("<div class=\"logo_style\">"
                   "<img src=\"/images/Administrative Tools-64 (1).png\">"
                   "<h2>Software Configuration Controller</h2>");
    response.write("</div><div class=\"container\">");

    QString action(request.getParameter("action"));

    bool have_any_action=FALSE;
    if (map.contains(action)) {
        TableItem *table_data = map.value(action);
        response.write(table_data->doHTMLParse(&request));
        have_any_action=TRUE;
    }

    if(!have_any_action && action == "Software_custom"){
        qDebug() << "TODO: custom_Software action";
        have_any_action=TRUE;
    }
    if(have_any_action){
        response.write("<br/><a href=\"/sw\">[Back To Software Section]</a> | <a href=\"/\">[Back To Main Section]</a>");
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
        QString Software_custom = QString(
                    "<form method=\"post\">"
                    "  <input type=\"hidden\" name=\"action\" value=\"Software_custom\">"
                    "<div class=\"form_info\">Custom Software message:</div>"
                    "<div class=\"form_editor\">"
                    "<input type=\"text\" name=\"Software_custom_b\" value=\"4000\" size=\"4\">"
                    "<input type=\"text\" name=\"Software_custom_c\" value=\"0000\" size=\"4\">"
                    "<input type=\"text\" name=\"Software_custom_d\" value=\"0000\" size=\"4\">"
                    "Circular:<input type=\"checkbox\" name=\"Software_custom_circular\" value=\"0\">"
                    "</div>"
                    "<div class=\"form_submitter\"><input type=\"submit\" value=\"&gt;&gt;\"></div>"
                    "<div class=\"form_tooltip\"></div>"
                    "</form>");
         response.write(Software_custom.toUtf8());
#endif
    }

    response.write("</div></div></div></body></html>", true);

}
