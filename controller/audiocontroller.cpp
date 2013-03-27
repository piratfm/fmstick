/**
  @file
  @author tipok
*/

#include "audiocontroller.h"

#include "tableitem.h"

#include "requestmapper.h"

#include "ui_mainwindow.h"
#include "mainwindow.h"


AudioController::AudioController(QObject *parent) {
    rm_ptr = (RequestMapper *) parent;
}

void AudioController::service(HttpRequest& request, HttpResponse& response) {
    MainWindow *mw_ptr = (MainWindow *) rm_ptr->mw_ptr;
    QMap<QString, TableItem *> map;

    //map.insert("", new TableItem(rm_ptr, "", "", mw_ptr->ui->));
    map.insert("audio_preset", new TableItem(rm_ptr, "audio_preset", "Compressor/Limiter Presets", mw_ptr->ui->comboBox_audioPreset));
    map.insert("compressor", new TableItem(rm_ptr, "compressor", "Enable Audio Compressor", mw_ptr->ui->checkBox_audio_enablecompressor));
    map.insert("comp_threshold", new TableItem(rm_ptr, "comp_threshold", "Audio Compressor threshold", mw_ptr->ui->spinBox_audiocompressorThreshold));
    map.insert("comp_gain", new TableItem(rm_ptr, "comp_gain", "Audio Compressor gain", mw_ptr->ui->spinBox_audioCompressorGain));
    map.insert("comp_reltime_id", new TableItem(rm_ptr, "comp_reltime_id", "Audio Compressor release time", mw_ptr->ui->comboBox_audioCompressorReleasetime));
    map.insert("comp_attacktime", new TableItem(rm_ptr, "comp_attacktime", "Audio Compressor attack time", mw_ptr->ui->doubleSpinBox_audioCompressorAttackTime));
    map.insert("limiter", new TableItem(rm_ptr, "limiter", "Audio Liminter Enabled", mw_ptr->ui->checkBox_audioLimiterEnable));
    map.insert("lim_releasetime", new TableItem(rm_ptr, "lim_releasetime", "Audio Limiter release time", mw_ptr->ui->doubleSpinBox_audio_lim_release_time));
    map.insert("asq_overmod", new TableItem(rm_ptr, "asq_overmod", "Overmodulation Detection", mw_ptr->ui->checkBox_asq_overmoddetect));
    map.insert("asq_low", new TableItem(rm_ptr, "asq_low", "Audio Low Level Detection", mw_ptr->ui->checkBox_asq_detectLow));
    map.insert("asq_low_threshold", new TableItem(rm_ptr, "asq_low_threshold", "Audio Low Level Detection Threshold", mw_ptr->ui->spinBox_asq_detectLowThreshold));
    map.insert("asq_low_duration", new TableItem(rm_ptr, "asq_low_duration", "Audio Low Level Detection Duration", mw_ptr->ui->spinBox_asq_detectLowDuration));
    map.insert("asq_high", new TableItem(rm_ptr, "asq_high", "Audio High Level Detection", mw_ptr->ui->checkBox_asq_detectHigh));
    map.insert("asq_high_threshold", new TableItem(rm_ptr, "asq_high_threshold", "Audio High Level Detection Threshold", mw_ptr->ui->spinBox_asq_detectHighThreshold));
    map.insert("asq_high_duration", new TableItem(rm_ptr, "asq_high_duration", "Audio High Level Detection Duration", mw_ptr->ui->spinBox_asq_detectHighDuration));
    map.insert("standalone_level", new TableItem(rm_ptr, "standalone_level", "Max Level (in stand-alone mode)", mw_ptr->ui->comboBox_audio_input_level));
    map.insert("standalone_amplitude", new TableItem(rm_ptr, "standalone_amplitude", "Max Amplitude (in stand-alone mode)", mw_ptr->ui->spinBox_audio_input_amplitude));
    map.insert("standalone_mute", new TableItem(rm_ptr, "standalone_mute", "Mute Channel (in stand-alone mode)", mw_ptr->ui->comboBox_audio_input_mute));
    //map.insert("", new TableItem(this, "", "", mw_ptr->ui->));

    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    QFile file(":/fmstick/html/index.html");
    if (file.open(QIODevice::ReadOnly)) {
        response.write(file.readAll());
        file.close();
    }

    response.write("<div class=\"logo_style\">"
                   "<img src=\"/images/Music_Note.png\">"
                   "<h2>Audio Configuration Controller</h2>");
    response.write(QString("</div><div>FLAGS: %1 %2 %3 Level: %4 dB</div>")
            .arg(mw_ptr->ui->label_audio_overmodulated->isEnabled() ? "<img src=\"/images/overmodulated.png\" width=\"16\" height=\"16\">" : "")
            .arg(mw_ptr->ui->label_audio_asq_triggered_high->isEnabled() ? "<img src=\"/images/sound_high.png\" width=\"16\" height=\"16\">" : "")
            .arg(mw_ptr->ui->label_audio_asq_triggered_low->isEnabled() ? "<img src=\"/images/sound_mute.png\" width=\"16\" height=\"16\">" : "")
            .arg(mw_ptr->ui->progressBar_audio_asq_measured->value()).toUtf8());
    response.write("<div class=\"container\">");

    QString action(request.getParameter("action"));

    bool have_any_action=FALSE;
    if (map.contains(action)) {
        TableItem *table_data = map.value(action);
        response.write(table_data->doHTMLParse(&request));
        have_any_action=TRUE;
    }

    if(!have_any_action && action == "audio_custom"){
        qDebug() << "TODO: custom_Audio action";
        have_any_action=TRUE;
    }
    if(have_any_action){
        response.write("<br/><a href=\"/audio\">[Back To Audio Section]</a> | <a href=\"/\">[Back To Main Section]</a>");
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
        QString audio_custom = QString(
                    "<form method=\"post\">"
                    "  <input type=\"hidden\" name=\"action\" value=\"audio_custom\">"
                    "<div class=\"form_info\">Custom Audio message:</div>"
                    "<div class=\"form_editor\">"
                    "<input type=\"text\" name=\"audio_custom_b\" value=\"4000\" size=\"4\">"
                    "<input type=\"text\" name=\"audio_custom_c\" value=\"0000\" size=\"4\">"
                    "<input type=\"text\" name=\"audio_custom_d\" value=\"0000\" size=\"4\">"
                    "Circular:<input type=\"checkbox\" name=\"audio_custom_circular\" value=\"0\">"
                    "</div>"
                    "<div class=\"form_submitter\"><input type=\"submit\" value=\"&gt;&gt;\"></div>"
                    "<div class=\"form_tooltip\"></div>"
                    "</form>");
         response.write(audio_custom.toUtf8());
#endif
    }

    response.write("</div></div></div></body></html>", true);

}
