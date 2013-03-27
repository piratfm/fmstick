/**
  @file
  @author Stefan Frings
*/

#include "requestmapper.h"
//#include "static.h"
#include "staticfilecontroller.h"
//#include "controller/templatecontroller.h"

#include "controller/audiocontroller.h"
#include "controller/txcontroller.h"
#include "controller/rdscontroller.h"
#include "controller/hardwarecontroller.h"
#include "controller/softwarecontroller.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"

RequestMapper::RequestMapper(QObject* parent)
    :HttpRequestHandler(parent) {
    web_cfg.setValue("path", ":/fmstick");
}

bool RequestMapper::emitUpdateItem(QObject *obj, QString *val) {
  return emit updateItem(obj, val);
}

QString RequestMapper::fileBase64(QString path)
{
    QByteArray ba;
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        ba = file.readAll();
        file.close();
    }
    return QString(ba.toBase64());
}


void RequestMapper::mainPageStaff(HttpRequest& request, HttpResponse& response)
{
    MainWindow *mw = (MainWindow *) mw_ptr;
    QString val;

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    QFile file(":/fmstick/html/index.html");
    if (file.open(QIODevice::ReadOnly)) {
        response.write(file.readAll());
        file.close();
    } else {
        response.write("<html><body><div>error.<div style=\"display: none\">");
    }


    QString tx_icon = "fmstick.png";
    QString bullet_code="";
    if(mw->deviceStatus) {
        if(mw->ui->checkBox_transmitter_enabled->isChecked()) {
            if(mw->ui->actionRead_from_Device->isEnabled()) {
                bullet_code="<font color=\"green\">&bull;</font>&nbsp;";
                tx_icon = "fmstick.png";
            } else {
                bullet_code="<font color=\"grey\">&bull;</font>&nbsp;";
                tx_icon = "Gnome-Network-Wireless-NoTxWarning-64.png";
            }
        } else {
            bullet_code="<font color=\"grey\">&bull;</font>&nbsp;";
            tx_icon = "Gnome-Network-Wireless-NoTx-64.png";
        }
    } else {
        if(!mw->ui->checkBox_transmitter_enabled->isChecked()){
            tx_icon = "Gnome-Network-Wireless-NoTx-64.png";
        } else {
            tx_icon = "Gnome-Network-Wireless-NoDevice-64.png";
        }
    }


    response.write(QString("<div class=\"logo_style\">"
                   "<img src=\"/images/%1\">"
                   "<h2>Fast Actions</h2>").arg(tx_icon).toUtf8());
    response.write("</div><div class=\"container\">");

    QString action(request.getParameter("action"));
    if(action == "") {
        response.write("<div style=\"display:block;border-top: 1px dotted #CCCCCC;height: 48px;\">");
        response.write(QString("<form style=\"display:inline-block;padding: 10px;vertical-align: top;\" method=\"post\" title=\"%1\">"
                               "<input type=\"image\" name=\"action\" value=\"connect\" src=\"/images/%2connect_32.png\" width=24 height=24 /></form>")
                       .arg(mw->ui->actionConnect_to_device->toolTip())
                       .arg(mw->deviceStatus ? "" : "dis")
                       .toUtf8());

        response.write(QString("<form style=\"display:inline-block;padding: 10px;vertical-align: top;\" method=\"post\" title=\"%1\">"
                               "<input type=\"image\" name=\"action\" value=\"transmit\" src=\"/images/%2\" width=24 height=24 %3/></form>")
                       .arg(mw->ui->actionToggle_transmission->toolTip())
                       .arg(tx_icon)
                       .arg(mw->deviceStatus ? "" : "disabled class=\"filter_grayscale\"")
                       .toUtf8());
        response.write(QString("<form style=\"display:inline-block;padding: 10px;vertical-align: top;\" method=\"post\" title=\"%1\">"
                               "<input type=\"image\" name=\"action\" value=\"readcfg\" src=\"/images/1318446499_transmit_go.png\" width=24 height=24 %2/></form>")
                       .arg(mw->ui->actionRead_from_Device->toolTip())
                       .arg(mw->ui->actionRead_from_Device->isEnabled() ? "" : "disabled class=\"filter_grayscale\" ")
                       .toUtf8());
        response.write(QString("<form style=\"display:inline-block;padding: 10px;vertical-align: top;\" method=\"post\" title=\"%1\">"
                               "<input type=\"image\" name=\"action\" value=\"sendcfg\" src=\"/images/write_to_transmitter.png\" width=24 height=24 %2/></form>")
                       .arg(mw->ui->actionWrite_to_Device->toolTip())
                       .arg(mw->ui->actionWrite_to_Device->isEnabled() ? "" : "disabled class=\"filter_grayscale\" ")
                       .toUtf8());
        response.write(QString("<form style=\"display:inline-block;padding: 10px;vertical-align: top;\" method=\"post\" title=\"%1\">"
                               "<input type=\"image\" name=\"action\" value=\"readeeprom\" src=\"/images/read_from_eeprom.png\" width=24 height=24 %2/></form>")
                       .arg(mw->ui->actionRead_from_EEPROM->toolTip())
                       .arg(mw->ui->actionRead_from_EEPROM->isEnabled() ? "" : "disabled class=\"filter_grayscale\" ")
                       .toUtf8());
        response.write(QString("<form style=\"display:inline-block;padding: 10px;vertical-align: top;\" method=\"post\" title=\"%1\">"
                       "<input type=\"image\" name=\"action\" value=\"writeeeprom\" src=\"/images/write_to_eeprom.png\" width=24 height=24 %2/></form>")
                       .arg(mw->ui->actionWrite_to_EEPROM->toolTip())
                       .arg(mw->ui->actionWrite_to_EEPROM->isEnabled() ? "" : "disabled class=\"filter_grayscale\" ")
                       .toUtf8());
        response.write(QString("<form style=\"display:inline-block;padding: 10px;vertical-align: top;\" method=\"post\" title=\"%1\">"
                       "<input type=\"image\" name=\"action\" value=\"reset_to_default\" "
                       "src=\"/images/browser_reset.png\" width=24 height=24/></form>")
                       .arg(mw->ui->actionReset_to_Default->toolTip())
                       .toUtf8());

        response.write("<div style=\"display:inline-block;padding-left: 25px;\">");
        response.write(QString("<img class=\"%1\" src=\"/images/overmodulated.png\" title=\"%2\" />")
                       .arg(mw->ui->label_audio_overmodulated->isEnabled() ? "img_norm" : "img_grayscale")
                       .arg(mw->ui->label_audio_overmodulated->toolTip())
                       .toUtf8());
        response.write(QString("<img class=\"%1\" src=\"/images/sound_high.png\" title=\"%2\" />")
                       .arg(mw->ui->label_audio_asq_triggered_high->isEnabled() ? "img_norm" : "img_grayscale")
                       .arg(mw->ui->label_audio_asq_triggered_high->toolTip())
                       .toUtf8());
        response.write(QString("<img class=\"%1\" src=\"/images/sound_mute.png\" title=\"%2\" />")
                       .arg(mw->ui->label_audio_asq_triggered_low->isEnabled() ? "img_norm" : "img_grayscale")
                       .arg(mw->ui->label_audio_asq_triggered_low->toolTip())
                       .toUtf8());
        response.write("</div>");
        response.write("<div style=\"display:inline-block;padding-left: 25px;vertical-align:top;padding-top:10px;\">");

        if(mw->deviceStatus && mw->ui->actionRead_from_Device->isEnabled()) {
            response.write(QString("<div style=\"display:block\">&nbsp;&nbsp;&nbsp;Audio Level: %1 dB</div>").arg(mw->ui->progressBar_audio_asq_measured->value()).toUtf8());
            response.write(QString("<div style=\"display:block\">%1TX Power: %2 dBuV</div>")
                           .arg(bullet_code)
                           .arg(mw->ui->spinBox_transmitter_power->value()).toUtf8());
        } else
            response.write("<font color=\"red\">Device is not connected.</font>");

        response.write("</div>");


        response.write("</div>");

        response.write("<div class=\"container\">");

        val = QString( "<form><input type=\"hidden\" name=\"action\" value=\"update_rt\">"
                       "<input id=\"update_rt_input\" type=\"hidden\" name=\"update_rt\" value=\"%1\" />"
                       "<div class=\"row\">"
                       "<div class=\"prop\">"
                       "<p><b>RDS RadioText:</b></p>"
                       "</div>"
                       "<div class=\"val\">"
                       "<p><div contenteditable=\"true\" id=\"update_rt_div\" style=\"display: inline-block; vertical-align: middle;width: 170px;height:80px; border: 3px solid #666; overflow:auto\">%2</div></p>"
                       "</div>"
                       "<div class=\"submit\">"
                       "<p><input onclick=\"document.getElementById('update_rt_input').value = document.getElementById('update_rt_div').innerHTML;\" type=\"submit\" value=\"&gt;&gt;\" /></p>"
                       "</div>"
                       "<div class=\"tooltip\">"
                       "<div style=\"display:inline-block;padding-left: 10px;vertical-align: middle;\">"
                       "  <div style=\"display:block\"><span style=\"background-color: #CCFFCC\">&nbsp;&nbsp;</span> - %3</div>"
                       "  <div style=\"display:block\"><span style=\"background-color: #CCCCFF\">&nbsp;&nbsp;</span> - %4</div>"
                       "  <div style=\"display:block\"><span style=\"background-color: #FFCCCC\">&nbsp;&nbsp;</span> - %5</div>"
                       "  <div style=\"display:block\"><span style=\"background-color: #FFFFCC\">&nbsp;&nbsp;</span> - %6</div>"
                       "</div>"
                       "</div>"
                       "</div></form>")
                .arg(mw->ui->plainTextEdit_rds_rt->toPlainText())
                .arg(mw->rtplus_text.length() > 0 ? mw->rtplus_text : mw->ui->plainTextEdit_rds_rt->toPlainText())
                .arg(mw->ui->comboBox_rds_content1_type->currentText())
                .arg(mw->ui->comboBox_rds_content2_type->currentText())
                .arg(mw->ui->comboBox_rds_content3_type->currentText())
                .arg(mw->ui->comboBox_rds_content4_type->currentText());


        response.write(val.toUtf8());
        val = QString("<form><input type=\"hidden\" name=\"action\" value=\"update_ps\">"
                "<div class=\"row\">"
                "<div class=\"prop\">"
                "<p><b>RDS PS: </b></p>"
                "</div>"
                "<div class=\"val\">"
                "<p><input type=\"text\" name=\"update_ps\" value=\"%1\" style=\"vertical-align: middle;\" /></p>"
                "</div>"
                "<div class=\"submit\">"
                "<p><b><input type=\"submit\" value=\"&gt;&gt;\" /></b></p>"
                "</div>"
                "<div class=\"tooltip\">"
                "<p><b>&nbsp;</b></p>"
                "</div>"
                "</div></form>")
                .arg(mw->ui->lineEdit_station_id->text());
        response.write(val.toUtf8());


        val = QString("<form><input type=\"hidden\" name=\"action\" value=\"update_power\">"
                "<div class=\"row\">"
                "<div class=\"prop\">"
                "<p><b>TX Power: </b></p>"
                "</div>"
                "<div class=\"val\">"
                "<p><input type=\"number\" name=\"update_power\" value=\"%1\" min=\"%2\" max=\"%3\" step=\"%4\" style=\"vertical-align: middle;\" /></p>"
                "</div>"
                "<div class=\"submit\">"
                "<p><b><input type=\"submit\" value=\"&gt;&gt;\" /></b></p>"
                "</div>"
                "<div class=\"tooltip\">"
                "<p><b>&nbsp;</b></p>"
                "</div>"
                "</div></form>")
                .arg(mw->ui->spinBox_transmitter_power->value())
                .arg(mw->ui->spinBox_transmitter_power->minimum())
                .arg(mw->ui->spinBox_transmitter_power->maximum())
                .arg(mw->ui->spinBox_transmitter_power->singleStep());
        response.write(val.toUtf8());

        val = QString("<form><input type=\"hidden\" name=\"action\" value=\"update_frequency\">"
                "<div class=\"row\">"
                "<div class=\"prop\">"
                "<p><b>TX Frequency: </b></p>"
                "</div>"
                "<div class=\"val\">"
                "<p><input type=\"number\" name=\"update_frequency\" value=\"%1\" min=\"%2\" max=\"%3\" step=\"%4\" style=\"vertical-align: middle;\" /></p>"
                "</div>"
                "<div class=\"submit\">"
                "<p><b><input type=\"submit\" value=\"&gt;&gt;\" /></b></p>"
                "</div>"
                "<div class=\"tooltip\">"
                "<p><b>&nbsp;</b></p>"
                "</div>"
                "</div></form>")
                .arg(mw->ui->doubleSpinBox_freq_main->value())
                .arg(mw->ui->doubleSpinBox_freq_main->minimum())
                .arg(mw->ui->doubleSpinBox_freq_main->maximum())
                .arg(mw->ui->doubleSpinBox_freq_main->singleStep());
        response.write(val.toUtf8());

        response.write("</div>");

    } else if(action == "connect"){
        emit updateItem(mw->ui->actionConnect_to_device, NULL);
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "transmit"){
        emit updateItem(mw->ui->actionToggle_transmission, NULL);
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "readcfg"){
        emit updateItem(mw->ui->actionRead_from_Device, NULL);
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "sendcfg"){
        emit updateItem(mw->ui->actionWrite_to_Device, NULL);
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "readeeprom"){
        emit updateItem(mw->ui->actionRead_from_EEPROM, NULL);
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "writeeeprom"){
        emit updateItem(mw->ui->actionWrite_to_EEPROM, NULL);
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "reset_to_default"){
        emit updateItem(mw->ui->actionReset_to_Default, NULL);
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "update_rt"){
        emit updateItem(mw->ui->plainTextEdit_rds_rt, new QString(request.getParameter("update_rt")));
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "update_ps"){
        emit updateItem(mw->ui->lineEdit_station_id, new QString(request.getParameter("update_ps")));
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "update_power"){
        emit updateItem(mw->ui->spinBox_transmitter_power, new QString(request.getParameter("update_power")));
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else if(action == "update_frequency"){
        emit updateItem(mw->ui->doubleSpinBox_freq_main, new QString(request.getParameter("update_frequency")));
        response.write(QString("<p><b><font color=green>Action \"%1\" is sent.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    } else {
        response.write(QString("<p><b><font color=red>Action \"%1\" is unknown.</font></b></p>").arg(action).toUtf8());
        response.write("<br/><a href=\"/\">[Back To Main Section]</a>");
    }

    response.write("</div></body></html>", true);
}


void RequestMapper::service(HttpRequest& request, HttpResponse& response) {
    MainWindow *mw = (MainWindow *) mw_ptr;

    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    QByteArray auth = request.getHeader("Authorization");
    QList<QByteArray> auth_list;
    auth_list = auth.split(' ');
    QString our_auth = QString("%1:%2").arg(mw->ui->lineEdit_sw_remote_login->text()).arg(mw->ui->lineEdit_sw_remote_pwd->text());

    if(!auth_list.count() || auth_list.at(0) != "Basic" || auth_list.at(1) != our_auth.toAscii().toBase64()) {
        response.setStatus(401, "Not Authorized");
        response.setHeader("WWW-Authenticate", QString("Basic realm=\"%1-%2\"").arg("FmStick").arg(mw->CurrentVersion).toAscii());
        response.write("401 You - a stranger!",true);
        return;
    } else {
        qDebug("RequestMapper: auth success");
    }


    if(path.length() <= 1) {
        mainPageStaff(request, response);
        return;
    }
    else if (path.startsWith("/favicon.ico")) {
        response.setStatus(302, "Found");
        response.setHeader("Location", "/images/fmstick.ico");
        return;
    }
    else if (path.startsWith("/audio")) {
        AudioController(this).service(request, response);
        return;
    }

    else if (path.startsWith("/tx")) {
        TxController(this).service(request, response);
        return;
    }

    else if (path.startsWith("/rds")) {
        RdsController(this).service(request, response);
        return;
    }
    else if (path.startsWith("/hw")) {
        HardwareController(this).service(request, response);
        return;
    }
    else if (path.startsWith("/sw")) {
        SoftwareController(this).service(request, response);
        return;
    }
    else {
        StaticFileController(&web_cfg, (QObject *) this).service(request, response);
        //Static::staticFileController->service(request, response);
        return;
    }
}
