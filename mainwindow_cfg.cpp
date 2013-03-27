/*
 * settings management
 * $Id$
 * (part of FmStick).
 *
 * (C) 2011 Serge <piratfm@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <QFileDialog>
#include <QtCore/QDir>

#include "mainwindow.h"
#include "ui_mainwindow.h"


void MainWindow::resetFmTxSettings()
{
    qDebug() << "Resetting transmitter to defaults";
    settings.beginGroup("audio");
    settings.remove("");
    settings.endGroup();

    settings.beginGroup("transmitter");
    settings.remove("");
    settings.endGroup();

    settings.beginGroup("rds");
    settings.remove("");
    settings.endGroup();
    readSettings(&settings, true);
    SetAudioCompLimPresetByParams();
}

void MainWindow::SaveFrontendParams()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                    tr("Choose a file to save"),
                    QDir::homePath(),
                    tr("Config Files (*.ini *.cfg *.conf)"));
    if(!fileName.isNull()) {
        qDebug() << "saving tx settings to " << fileName;
        QSettings *saveTo = new QSettings(fileName, QSettings::IniFormat);
        writeSettings(saveTo, true);
        delete saveTo;
    }
}

void MainWindow::ReadFrontendParams()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Choose a file to open"),
                    QDir::homePath(),
                    tr("Config Files (*.ini *.cfg *.conf)"));
    if(!fileName.isNull()) {
        qDebug() << "reading tx settings from " << fileName;
        QSettings *readFrom = new QSettings(fileName, QSettings::IniFormat);
        readSettings(readFrom, true);
        delete readFrom;
    }
}

void MainWindow::writeSettings(QSettings *writer, bool txOnly)
{
    qDebug() << __FUNCTION__;
    if(!txOnly) {
        writer->beginGroup("ui");
        writer->setValue("pos", pos());
        writer->setValue("mode", ui->checkBox_mini_mode->isChecked());
        writer->setValue("toolbar", toolBarIsVisible);
        writer->setValue("ontop", ui->checkBox_be_on_the_top->isChecked());
        writer->setValue("intray", ui->checkBox_sw_hideInTray->isChecked());
        writer->setValue("language", UsedLanguage);
        writer->endGroup();

        writer->beginGroup("rdstime");
        writer->setValue("flag", ui->checkBox_rds_time->isChecked());
        writer->setValue("interval", ui->spinBox_rds_time_int->value());
        writer->endGroup();

        writer->beginGroup("core");
        writer->setValue("softcfg", ui->checkBox_rewriteCfg->isChecked());
        writer->setValue("autoconnect", ui->checkBox_sw_autoconnect->isChecked());
        writer->setValue("liveedit", ui->checkBox_sw_live_edit->isChecked());
        writer->setValue("http_serve", ui->checkBox_sw_remote->isChecked());
        writer->setValue("http_port", ui->spinBox_sw_remote_port->value());
        writer->setValue("http_login", ui->lineEdit_sw_remote_login->text());
        writer->setValue("http_password", ui->lineEdit_sw_remote_pwd->text());
        writer->endGroup();

        writer->beginGroup("hardware");
        writer->endGroup();
    }

    writer->beginGroup("audio");
    writer->setValue("compressor", ui->checkBox_audio_enablecompressor->isChecked());
    writer->setValue("comp_threshold", ui->spinBox_audiocompressorThreshold->value());
    writer->setValue("comp_gain", ui->spinBox_audioCompressorGain->value());
    writer->setValue("comp_reltime_id", ui->comboBox_audioCompressorReleasetime->currentIndex());
    writer->setValue("comp_attacktime", ui->doubleSpinBox_audioCompressorAttackTime->value());
    writer->setValue("limiter", ui->checkBox_audioLimiterEnable->isChecked());
    writer->setValue("lim_releasetime", ui->doubleSpinBox_audio_lim_release_time->value());
    writer->setValue("asq_overmod", ui->checkBox_asq_overmoddetect->isChecked());
    writer->setValue("asq_low", ui->checkBox_asq_detectLow->isChecked());
    writer->setValue("asq_low_threshold", ui->spinBox_asq_detectLowThreshold->value());
    writer->setValue("asq_low_duration", ui->spinBox_asq_detectLowDuration->value());
    writer->setValue("asq_high", ui->checkBox_asq_detectHigh->isChecked());
    writer->setValue("asq_high_threshold", ui->spinBox_asq_detectHighThreshold->value());
    writer->setValue("asq_high_duration", ui->spinBox_asq_detectHighDuration->value());
    writer->setValue("standalone_level", ui->comboBox_audio_input_level->currentIndex());
    writer->setValue("standalone_amplitude", ui->spinBox_audio_input_amplitude->value());
    writer->setValue("standalone_mute", ui->comboBox_audio_input_mute->currentIndex());
    writer->endGroup();

    writer->beginGroup("transmitter");
    writer->setValue("preemphasis_id", ui->comboBox_transmitter_preemph->currentIndex());
    writer->setValue("audio_deviation", ui->spinBox_transmitter_audioDev->value());
    writer->setValue("pilot", ui->checkBox_transmitter_pilot->isChecked());
    writer->setValue("pilot_deviation", ui->spinBox_transmitter_pilot_devi->value());
    writer->setValue("pilot_frequency", ui->spinBox_transmitter_pilot_freq->value());
    writer->setValue("lmr", ui->checkBox_transmitter_lmr->isChecked());
    writer->setValue("rds", ui->checkBox_transmitter_rds->isChecked());
    writer->setValue("rds_deviation", ui->spinBox_transmitter_rds_devi->value());
    writer->setValue("tx_enabled", ui->checkBox_transmitter_enabled->isChecked());
    writer->setValue("tx_frequency", ui->doubleSpinBox_transmitter_freq->value());
    writer->setValue("tx_power", ui->spinBox_transmitter_power->value());
    writer->setValue("tx_antcap", ui->checkBox_transmitter_antcap->isChecked());
    writer->setValue("tx_antcap_value", ui->doubleSpinBox_transmitter_antcap_val->value());
    writer->endGroup();

    writer->beginGroup("rds");
    writer->setValue("prog_id", ui->lineEdit_rds_progid->text());
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
    writer->setValue("tp", ui->checkBox_rds_ta_tp->isChecked());

    writer->setValue("ps_percentage_id", ui->comboBox_rds_ps_percentage->currentIndex());
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
    writer->setValue("ps_12", ui->lineEdit_rds_ps_12->text());

    writer->setValue("rt_update", ui->checkBox_rds_title_file->isChecked());
    writer->setValue("rt_update_path", ui->lineEdit_rds_title_file->text());
    writer->setValue("rt", ui->plainTextEdit_rds_rt->toPlainText()); //TODO: save prev. text

    writer->setValue("rt_plus", ui->checkBox_rds_title_rtplus->isChecked());
    writer->setValue("rt_plus_dis_item", ui->checkBox_rds_dis_item->isChecked());
    writer->setValue("rt_plus_content1_reg", ui->lineEdit_rds_content1_reg->text());
    writer->setValue("rt_plus_content1_type", ui->comboBox_rds_content1_type->currentIndex());
    writer->setValue("rt_plus_content2_reg", ui->lineEdit_rds_content2_reg->text());
    writer->setValue("rt_plus_content2_type", ui->comboBox_rds_content2_type->currentIndex());
    writer->setValue("rt_plus_content3_reg", ui->lineEdit_rds_content3_reg->text());
    writer->setValue("rt_plus_content3_type", ui->comboBox_rds_content3_type->currentIndex());
    writer->setValue("rt_plus_content4_reg", ui->lineEdit_rds_content4_reg->text());
    writer->setValue("rt_plus_content4_type", ui->comboBox_rds_content4_type->currentIndex());

    //ui->plainTextEdit_rds_rt
    writer->endGroup();
}

void MainWindow::readSettings(QSettings *reader, bool txOnly)
{
    qDebug() << __FUNCTION__;
    if(!txOnly) {
        reader->beginGroup("ui");
        move(reader->value("pos", QPoint(200, 200)).toPoint());
        ui->checkBox_mini_mode->setChecked(reader->value("mode", "false").toBool());
        toolBarIsVisible = reader->value("toolbar", "false").toBool();
        ui->checkBox_be_on_the_top->setChecked(reader->value("ontop", "false").toBool());
        ui->checkBox_sw_hideInTray->setChecked(reader->value("intray", "false").toBool());
        UsedLanguage = reader->value("language", "auto").toString();
        reader->endGroup();

        reader->beginGroup("rdstime");
        ui->checkBox_rds_time->setChecked(reader->value("flag", "false").toBool());
        ui->spinBox_rds_time_int->setValue(reader->value("interval", "5").toInt());
        reader->endGroup();

        reader->beginGroup("core");
        ui->checkBox_rewriteCfg->setChecked(reader->value("softcfg", "false").toBool());
        ui->checkBox_sw_autoconnect->setChecked(reader->value("autoconnect", "false").toBool());
        ui->checkBox_sw_live_edit->setChecked(reader->value("liveedit", "false").toBool());
        /* http server staff */
        ui->checkBox_sw_remote->setChecked(reader->value("http_serve", "false").toBool());
        ui->spinBox_sw_remote_port->setValue(reader->value("http_port", "8080").toInt());
        ui->lineEdit_sw_remote_login->setText(reader->value("http_login", "admin").toString());
        ui->lineEdit_sw_remote_pwd->setText(reader->value("http_password", "12345").toString());
        reader->endGroup();

        reader->beginGroup("hardware");
        reader->endGroup();
    }

    reader->beginGroup("audio");
    ui->checkBox_audio_enablecompressor->setChecked(reader->value("compressor", "false").toBool());
    ui->spinBox_audiocompressorThreshold->setValue(reader->value("comp_threshold", "-40").toInt());
    ui->spinBox_audioCompressorGain->setValue(reader->value("comp_gain", "15").toInt());
    ui->comboBox_audioCompressorReleasetime->setCurrentIndex(reader->value("comp_reltime_id", "4").toInt());
    ui->doubleSpinBox_audioCompressorAttackTime->setValue(reader->value("comp_attacktime", "0.5").toDouble());
    ui->checkBox_audioLimiterEnable->setChecked(reader->value("limiter", "true").toBool());
    ui->doubleSpinBox_audio_lim_release_time->setValue(reader->value("lim_releasetime", "5.0196").toDouble());
    ui->checkBox_asq_overmoddetect->setChecked(reader->value("asq_overmod", "false").toBool());
    ui->checkBox_asq_detectLow->setChecked(reader->value("asq_low", "false").toBool());
    ui->spinBox_asq_detectLowThreshold->setValue(reader->value("asq_low_threshold", "-40").toInt());
    ui->spinBox_asq_detectLowDuration->setValue(reader->value("asq_low_duration", "300").toInt());
    ui->checkBox_asq_detectHigh->setChecked(reader->value("asq_high", "false").toBool());
    ui->spinBox_asq_detectHighThreshold->setValue(reader->value("asq_high_threshold", "-20").toInt());
    ui->spinBox_asq_detectHighDuration->setValue(reader->value("asq_high_duration", "300").toInt());
    ui->comboBox_audio_input_level->setCurrentIndex(reader->value("standalone_level", "3").toInt());
    ui->spinBox_audio_input_amplitude->setValue(reader->value("standalone_amplitude", "636").toInt());
    ui->comboBox_audio_input_mute->setCurrentIndex(reader->value("standalone_mute", "0").toInt());
    reader->endGroup();

    reader->beginGroup("transmitter");
    ui->comboBox_transmitter_preemph->setCurrentIndex(reader->value("preemphasis_id", "1").toInt());
    ui->spinBox_transmitter_audioDev->setValue(reader->value("audio_deviation", "68250").toInt());
    ui->checkBox_transmitter_pilot->setChecked(reader->value("pilot", "true").toBool());
    ui->spinBox_transmitter_pilot_devi->setValue(reader->value("pilot_deviation", "6750").toInt());
    ui->spinBox_transmitter_pilot_freq->setValue(reader->value("pilot_frequency", "19000").toInt());
    ui->checkBox_transmitter_lmr->setChecked(reader->value("lmr", "true").toBool());
    ui->checkBox_transmitter_rds->setChecked(reader->value("rds", "false").toBool());
    ui->spinBox_transmitter_rds_devi->setValue(reader->value("rds_deviation", "2000").toInt());
    ui->checkBox_transmitter_enabled->setChecked(reader->value("tx_enabled", "true").toBool());
    ui->doubleSpinBox_transmitter_freq->setValue(reader->value("tx_frequency", "100.0").toDouble());
    ui->spinBox_transmitter_power->setValue(reader->value("tx_power", "115").toInt());
    ui->checkBox_transmitter_antcap->setChecked(reader->value("tx_antcap", "false").toBool());
    ui->doubleSpinBox_transmitter_antcap_val->setValue(reader->value("tx_antcap_value", "24.5").toDouble());
    reader->endGroup();

    reader->beginGroup("rds");
    ui->lineEdit_rds_progid->setText(reader->value("prog_id", "40A7").toString());
    ui->checkBox_rds_dynPT->setChecked(reader->value("dynamic_pt", "false").toBool());
    ui->checkBox_rds_compr_code->setChecked(reader->value("compressed_code", "false").toBool());
    ui->checkBox_rds_art_head->setChecked(reader->value("artifical_head", "false").toBool());
    ui->checkBox_rds_is_stereo->setChecked(reader->value("is_stereo", "true").toBool());
    ui->checkBox_rds_forceb->setChecked(reader->value("force_b", "false").toBool());
    ui->radioButton_rds_music->setChecked(reader->value("is_music", "true").toBool());
    ui->radioButton_rds_speech->setChecked(!ui->radioButton_rds_music->isChecked());
    ui->comboBox_rds_pty->setCurrentIndex(reader->value("pty_id", "0").toInt());
    ui->checkBox_rds_useaf->setChecked(reader->value("use_af", "false").toBool());
    ui->doubleSpinBox_rds_af->setValue(reader->value("af", "87.6").toDouble());
    ui->checkBox_rds_fifo->setChecked(reader->value("use_fifo", "false").toBool());
    ui->spinBox_rds_fifo_sz->setValue(reader->value("fifo_sz", "4").toInt());
    ui->checkBox_rds_ta->setChecked(reader->value("ta", "false").toBool());
    ui->checkBox_rds_ta_tp->setChecked(reader->value("tp", "false").toBool());
    ui->checkBox_rds_title_file->setChecked(reader->value("rt_update", "false").toBool());
    ui->lineEdit_rds_title_file->setText(reader->value("rt_update_path", "").toString());


    ui->checkBox_rds_title_rtplus->setChecked(reader->value("rt_plus", "false").toBool());
    ui->checkBox_rds_dis_item->setChecked(reader->value("rt_plus_dis_item", "false").toBool());
    ui->lineEdit_rds_content1_reg->setText(reader->value("rt_plus_content1_reg", "\\s-\\s(.+)").toString());
    ui->comboBox_rds_content1_type->setCurrentIndex(reader->value("rt_plus_content1_type", "1").toInt());
    ui->lineEdit_rds_content2_reg->setText(reader->value("rt_plus_content2_reg", "^(?:Now Playing\\:)?\\s*([^\\s].+)\\s-\\s").toString());
    ui->comboBox_rds_content2_type->setCurrentIndex(reader->value("rt_plus_content2_type", "4").toInt());
    ui->lineEdit_rds_content3_reg->setText(reader->value("rt_plus_content3_reg", "(\\d{6,})").toString());
    ui->comboBox_rds_content3_type->setCurrentIndex(reader->value("rt_plus_content3_type", "42").toInt());
    ui->lineEdit_rds_content4_reg->setText(reader->value("rt_plus_content4_reg", "((http(s?)\\://|www\\.){1}\\S+)").toString());
    ui->comboBox_rds_content4_type->setCurrentIndex(reader->value("rt_plus_content4_type", "29").toInt());

//    qDebug() << "\n\ncomboBox_rds_content1_type: " << ui->comboBox_rds_content1_type->currentIndex() << "\n";
//                         "comboBox_rds_content2_type: " << ui->comboBox_rds_content2_type->currentIndex() << "\n\n";

    //QString RDSRTPlusAuthorReg = reader->value("rt_plus_author_reg", "").toString();
    //QString RDSRTPlusSongReg = reader->value("rt_plus_song_reg", "").toString();
    //rt_plus_author_reg
    //rt_plus_song_reg
    //ui->lineEdit_rds_author_reg->setText(RDSRTPlusAuthorReg);
    //ui->lineEdit_rds_song_reg->setText(RDSRTPlusAuthorReg);
    //ui->checkBox_rds_title_rtplus->setChecked(RDSRTPlusAuthorReg.length() > 0 || RDSRTPlusSongReg.length() > 0);

    ui->comboBox_rds_ps_percentage->setCurrentIndex(reader->value("ps_percentage_id", "3").toInt());
    ui->spinBox_rds_ps_repeats->setValue(reader->value("ps_repeats", "3").toInt());
    ui->spinBox_rds_ps_msgs->setValue(reader->value("ps_num", "2").toInt());
    ui->lineEdit_rds_ps_1->setText(reader->value("ps_1", "FmStick").toString());
    ui->lineEdit_rds_ps_2->setText(reader->value("ps_2", "Example").toString());
    ui->lineEdit_rds_ps_3->setText(reader->value("ps_3", "").toString());
    ui->lineEdit_rds_ps_4->setText(reader->value("ps_4", "").toString());
    ui->lineEdit_rds_ps_5->setText(reader->value("ps_5", "").toString());
    ui->lineEdit_rds_ps_6->setText(reader->value("ps_6", "").toString());
    ui->lineEdit_rds_ps_7->setText(reader->value("ps_7", "").toString());
    ui->lineEdit_rds_ps_8->setText(reader->value("ps_8", "").toString());
    ui->lineEdit_rds_ps_9->setText(reader->value("ps_9", "").toString());
    ui->lineEdit_rds_ps_10->setText(reader->value("ps_10", "").toString());
    ui->lineEdit_rds_ps_11->setText(reader->value("ps_11", "").toString());
    ui->lineEdit_rds_ps_12->setText(reader->value("ps_12", "").toString());
    ui->plainTextEdit_rds_rt->setPlainText(reader->value("rt", "").toString());
    reader->endGroup();

    SetDigitalAudioHack();
}
