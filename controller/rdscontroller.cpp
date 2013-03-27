/**
  @file
  @author tipok
*/

#include "rdscontroller.h"
#include "tableitem.h"

#include "requestmapper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"


RdsController::RdsController(QObject *parent) {
    rm_ptr = (RequestMapper *) parent;
}

void RdsController::service(HttpRequest& request, HttpResponse& response) {
    MainWindow *mw_ptr = (MainWindow *) rm_ptr->mw_ptr;
    QMap<QString, TableItem *> map;

    map.insert("prog_id", new TableItem(rm_ptr, "prog_id", "Program ID", mw_ptr->ui->lineEdit_rds_progid));
    map.insert("dynamic_pt", new TableItem(rm_ptr, "dynamic_pt", "Dynamic Payload type", mw_ptr->ui->checkBox_rds_dynPT));
    map.insert("compressed_code", new TableItem(rm_ptr, "compressed_code", "Compressed Code", mw_ptr->ui->checkBox_rds_compr_code));
    map.insert("artifical_head", new TableItem(rm_ptr, "artifical_head", "Artifical Head Code", mw_ptr->ui->checkBox_rds_art_head));
    map.insert("is_stereo", new TableItem(rm_ptr, "is_stereo", "Is Stereo", mw_ptr->ui->checkBox_rds_is_stereo));
    map.insert("force_b", new TableItem(rm_ptr, "force_b", "Force B-Code", mw_ptr->ui->checkBox_rds_forceb));
    map.insert("is_music", new TableItem(rm_ptr, "is_music", " Is Music", mw_ptr->ui->radioButton_rds_music));
    map.insert("pty_id", new TableItem(rm_ptr, "pty_id", "Program Type", mw_ptr->ui->comboBox_rds_pty));
    map.insert("use_af", new TableItem(rm_ptr, "use_af", "Use Alternative Frequency", mw_ptr->ui->checkBox_rds_useaf));
    map.insert("af", new TableItem(rm_ptr, "af", "Alternative Frequency", mw_ptr->ui->doubleSpinBox_rds_af));
    map.insert("use_fifo", new TableItem(rm_ptr, "use_fifo", "Use FIFO", mw_ptr->ui->checkBox_rds_fifo));
    map.insert("fifo_sz", new TableItem(rm_ptr, "fifo_sz", "FIFO size", mw_ptr->ui->spinBox_rds_fifo_sz));
    map.insert("ta", new TableItem(rm_ptr, "ta", "Traffic Announcement", mw_ptr->ui->checkBox_rds_ta));
    map.insert("tp", new TableItem(rm_ptr, "tp", "Traffic Program", mw_ptr->ui->checkBox_rds_ta_tp));

/*  writer->setValue("prog_id", ui->lineEdit_rds_progid->text());
    writer->setValue("dynamic_pt", ui->checkBox_rds_dynPT->isChecked());
    writer->setValue("compressed_code", ui->checkBox_rds_compr_code->isChecked());
    writer->setValue("artifical_head", ui->checkBox_rds_art_head->isChecked());
    writer->setValue("is_stereo", ui->checkBox_rds_is_stereo->isChecked());
    writer->setValue("force_b", ui->checkBox_rds_forceb->isChecked());
    writer->setValue("is_music", ui->radioButton_rds_music->isChecked());
    writer->setValue("pty_id", ui->comboBox_rds_pty->currentIndex());
    writer->setValue("use_af", ui->checkBox_rds_useaf->isChecked());
    writer->setValue("af", ui->doubleSpinBox_rds_af->value());
    writer->setValue("use_fifo", ui->checkBox_rds_fifo->isChecked());
    writer->setValue("fifo_sz", ui->spinBox_rds_fifo_sz->value());
    writer->setValue("ta", ui->checkBox_rds_ta->isChecked());
    writer->setValue("tp", ui->checkBox_rds_ta_tp->isChecked());*/

    map.insert("ps_percentage_id", new TableItem(rm_ptr, "ps_percentage_id", "PS Percentage", mw_ptr->ui->comboBox_rds_ps_percentage));
    map.insert("ps_repeats", new TableItem(rm_ptr, "ps_repeats", "PS Repeats", mw_ptr->ui->spinBox_rds_ps_repeats));
    map.insert("ps_num", new TableItem(rm_ptr, "ps_num", "PS Number:", mw_ptr->ui->spinBox_rds_ps_msgs));
    map.insert("ps_1", new TableItem(rm_ptr, "ps_1", "PS 1", mw_ptr->ui->lineEdit_rds_ps_1));
    int ps_num=mw_ptr->ui->spinBox_rds_ps_msgs->value();
    if(ps_num>1)
        map.insert("ps_2", new TableItem(rm_ptr, "ps_2", "PS 2", mw_ptr->ui->lineEdit_rds_ps_2));
    if(ps_num>2)
        map.insert("ps_3", new TableItem(rm_ptr, "ps_3", "PS 3", mw_ptr->ui->lineEdit_rds_ps_3));
    if(ps_num>3)
        map.insert("ps_4", new TableItem(rm_ptr, "ps_4", "PS 4", mw_ptr->ui->lineEdit_rds_ps_4));
    if(ps_num>4)
        map.insert("ps_5", new TableItem(rm_ptr, "ps_5", "PS 5", mw_ptr->ui->lineEdit_rds_ps_5));
    if(ps_num>5)
        map.insert("ps_6", new TableItem(rm_ptr, "ps_6", "PS 6", mw_ptr->ui->lineEdit_rds_ps_6));
    if(ps_num>6)
        map.insert("ps_7", new TableItem(rm_ptr, "ps_7", "PS 7", mw_ptr->ui->lineEdit_rds_ps_7));
    if(ps_num>7)
        map.insert("ps_8", new TableItem(rm_ptr, "ps_8", "PS 8", mw_ptr->ui->lineEdit_rds_ps_8));
    if(ps_num>8)
        map.insert("ps_9", new TableItem(rm_ptr, "ps_9", "PS 9", mw_ptr->ui->lineEdit_rds_ps_9));
    if(ps_num>9)
        map.insert("ps_10", new TableItem(rm_ptr, "ps_10", "PS 10", mw_ptr->ui->lineEdit_rds_ps_10));
    if(ps_num>10)
        map.insert("ps_11", new TableItem(rm_ptr, "ps_11", "PS 11", mw_ptr->ui->lineEdit_rds_ps_11));
    if(ps_num>11)
        map.insert("ps_12", new TableItem(rm_ptr, "ps_12", "PS 12", mw_ptr->ui->lineEdit_rds_ps_12));

/*  writer->setValue("ps_percentage_id", ui->comboBox_rds_ps_percentage->currentIndex());
    writer->setValue("ps_repeats", ui->spinBox_rds_ps_repeats->value());
    writer->setValue("ps_num", ui->spinBox_rds_ps_msgs->value());
    writer->setValue("ps_1", ui->lineEdit_rds_ps_1->text());
    writer->setValue("ps_2", ui->lineEdit_rds_ps_2->text());
    writer->setValue("ps_3", ui->lineEdit_rds_ps_3->text());
    writer->setValue("ps_4", ui->lineEdit_rds_ps_4->text());
    writer->setValue("ps_5", ui->lineEdit_rds_ps_5->text());
    writer->setValue("ps_6", ui->lineEdit_rds_ps_6->text());
    writer->setValue("ps_7", ui->lineEdit_rds_ps_7->text());
    writer->setValue("ps_8", ui->lineEdit_rds_ps_8->text());
    writer->setValue("ps_9", ui->lineEdit_rds_ps_9->text());
    writer->setValue("ps_10", ui->lineEdit_rds_ps_10->text());
    writer->setValue("ps_11", ui->lineEdit_rds_ps_11->text());
    writer->setValue("ps_12", ui->lineEdit_rds_ps_12->text());*/

    map.insert("rt", new TableItem(rm_ptr, "rt", "RadioText", mw_ptr->ui->plainTextEdit_rds_rt));
    map.insert("rt_update", new TableItem(rm_ptr, "rt_update", "RDS RadioText update from file", mw_ptr->ui->checkBox_rds_title_file));
    map.insert("rt_update_path", new TableItem(rm_ptr, "rt_update_path", "RDS RadioText update file path", mw_ptr->ui->lineEdit_rds_title_file));
    map.insert("rt_plus", new TableItem(rm_ptr, "rt_plus", "RadioText Plus", mw_ptr->ui->checkBox_rds_title_rtplus));
    map.insert("rt_plus_dis_item", new TableItem(rm_ptr, "rt_plus_dis_item", "Disable Item if nothing found", mw_ptr->ui->checkBox_rds_dis_item));

    map.insert("rt_plus_content1_type", new TableItem(rm_ptr, "rt_plus_content1_type", "RT+ Content1 Type", mw_ptr->ui->comboBox_rds_content1_type));
    map.insert("rt_plus_content1_reg", new TableItem(rm_ptr, "rt_plus_content1_reg", "RT+ Content1 Regexp", mw_ptr->ui->lineEdit_rds_content1_reg));
    map.insert("rt_plus_content2_type", new TableItem(rm_ptr, "rt_plus_content2_type", "RT+ Content2 Type", mw_ptr->ui->comboBox_rds_content2_type));
    map.insert("rt_plus_content2_reg", new TableItem(rm_ptr, "rt_plus_content2_reg", "RT+ Content2 Regexp", mw_ptr->ui->lineEdit_rds_content2_reg));
    map.insert("rt_plus_content3_type", new TableItem(rm_ptr, "rt_plus_content3_type", "RT+ Content3 Type", mw_ptr->ui->comboBox_rds_content3_type));
    map.insert("rt_plus_content3_reg", new TableItem(rm_ptr, "rt_plus_content3_reg", "RT+ Content3 Regexp", mw_ptr->ui->lineEdit_rds_content3_reg));
    map.insert("rt_plus_content4_type", new TableItem(rm_ptr, "rt_plus_content4_type", "RT+ Content4 Type", mw_ptr->ui->comboBox_rds_content4_type));
    map.insert("rt_plus_content4_reg", new TableItem(rm_ptr, "rt_plus_content4_reg", "RT+ Content4 Regexp", mw_ptr->ui->lineEdit_rds_content4_reg));

    //map.insert("", new TableItem(this, "", "", mw_ptr->ui->));

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    QFile file(":/fmstick/html/index.html");
    if (file.open(QIODevice::ReadOnly)) {
        response.write(file.readAll());
        file.close();
    }

    response.write("<div class=\"logo_style\">"
                   "<img src=\"/images/rds-logo.png\">"
                   "<h2>RDS Configuration Controller</h2></div><div class=\"container\">");

    QString action(request.getParameter("action"));

    bool have_any_action=FALSE;
    if (map.contains(action)) {
        TableItem *table_data = map.value(action);
        response.write(table_data->doHTMLParse(&request));
        have_any_action=TRUE;
    }

    if(!have_any_action && action == "rds_custom1"){
        rm_ptr->emitUpdateItem((QObject *) mw_ptr->ui->pushButton_sw_send_custom_rds, new QString(request.getParameter("rds_custom1")));
        have_any_action=TRUE;
    }
    if(have_any_action){
        response.write("<br/><a href=\"/rds\">[Back To RDS Section]</a> | <a href=\"/\">[Back To Main Section]</a>");
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
#if 1
        response.write("<form><input type=\"hidden\" name=\"action\" value=\"rds_custom1\">"
                       "<div class=\"row\">"
                       "<div class=\"prop\">"
                       "<p>TODO! Send custom RDS message:</p>"
                       "</div>"
                       "<div class=\"val\">"
                       "<p><input type=\"text\" name=\"rds_custom1\" value=\"F:400000000000\" size=\"14\"></p>"
                       "</div>"
                       "<div class=\"submit\">"
                       "<p><input type=\"submit\" value=\"&gt;&gt;\"></p>"
                       "</div>"
                       "<div class=\"tooltip\">"
                       "<p>Send customized RDS message. Block A setted based on RDS PI-code, "
                       "other blocks: B,C and D - are avaliable to modify, also you can send "
                       "message to circular buffer instead of fifo to send it periodically. "
                       "To send message only once (using fifo buffer), as prefix write \"F:\", for example: \"F:301600004BD7\"."
                       "To send message periodically in circular buffer, write \"C:\" as prefix, for example: \"C:B00824A02005\"."
                       " Note circular buffe may be cleaned only when re-sending RadioText message.</p>"
                       "</div>"
                       "</div></form>");
#endif
    }
    response.write("</div></div></div></body></html>", true);

}
