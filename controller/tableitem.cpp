#include "tableitem.h"

#include "requestmapper.h"

#include "ui_mainwindow.h"



TableItem::TableItem(RequestMapper *c, QString s, QString d, QObject *o)
{
    ctrl=c;
    short_d=s;
    desc=d;
    obj=o;
    //obj_type=ot;
};

QByteArray TableItem::GenHTMLForm() {
    QString ret;
    QString objTypeName = obj->metaObject()->className();

    if(objTypeName == "QPlainTextEdit") {
        QPlainTextEdit *item = (QPlainTextEdit *) obj;

#if 0
        ret = QString("<form method=\"post\">"
                   "  <input type=\"hidden\" name=\"action\" value=\"%2\">"
                   "<div class=\"form_info\">%1</div>"
                   "<div class=\"form_editor\"><textarea name=\"%2\" cols=\"20\" rows=\"4\">%3</textarea></div>"
                   "<div class=\"form_submitter\"><input type=\"submit\" value=\"&gt;&gt;\"></div>"
                   "<div class=\"form_tooltip\">%4</div>"
                   "</form>")
                .arg(desc).arg(short_d).arg(item->toPlainText()).arg(item->toolTip());
#endif
        ret = QString("<form method=\"post\"><input type=\"hidden\" name=\"action\" value=\"%2\" />"
                      "<div class=\"row\">"
                      "<div class=\"prop\"><p>%1:</p></div>"
                      "<div class=\"val\"><p><textarea name=\"%2\" cols=\"16\" rows=\"3\">%3</textarea></p></div>"
                      "<div class=\"submit\"><p> %4</p></div>"
                      "<div class=\"tooltip\"><p> %5</p></div>"
                      "</div></form>\n")
                .arg(desc)
                .arg(short_d)
                .arg(item->toPlainText())
                .arg((!item->isEnabled() || item->isReadOnly()) ? "" : "<input type=\"submit\" value=\"&gt;&gt;\" />")
                .arg(item->toolTip());
    }
    else if(objTypeName == "QLineEdit") {
        QLineEdit *item = (QLineEdit *) obj;

        ret = QString("<form method=\"post\"><input type=\"hidden\" name=\"action\" value=\"%2\" />"
                      "<div class=\"row\">"
                        "<div class=\"prop\"><p>%1:</p></div>"
                        "<div class=\"val\"><p><input type=\"text\" name=\"%2\" value=\"%3\" /></p></div>"
                        "<div class=\"submit\"><p> %4</p></div>"
                        "<div class=\"tooltip\"><p> %5</p></div>"
                        "</div></form>\n")
                .arg(desc)
                .arg(short_d)
                .arg(item->text())
                .arg((!item->isEnabled() || item->isReadOnly()) ? "" : "<input type=\"submit\" value=\"&gt;&gt;\" />")
                .arg(item->toolTip());
    }
    else if(objTypeName == "QCheckBox") {
        QCheckBox *item = (QCheckBox *) obj;
        ret = QString("<form method=\"post\"><input type=\"hidden\" name=\"action\" value=\"%2\" />"
                      "<div class=\"row\">"
                        "<div class=\"prop\"><p>%1:</p></div>"
                        "<div class=\"val\"><p><input type=\"checkbox\" name=\"%2\" value=\"true\" %3/></p></div>"
                        "<div class=\"submit\"><p> %4</p></div>"
                        "<div class=\"tooltip\"><p> %5</p></div>"
                        "</div></form>\n")
                .arg(desc)
                .arg(short_d)
                .arg(item->isChecked() ? "checked" : "")
                .arg((!item->isEnabled()) ? "" : "<input type=\"submit\" value=\"&gt;&gt;\" />")
                .arg(item->toolTip());
    }
    else if(objTypeName == "QSpinBox") {
        QSpinBox *item = (QSpinBox *) obj;

        ret = QString("<form method=\"post\"><input type=\"hidden\" name=\"action\" value=\"%2\" />"
                      "<div class=\"row\">"
                        "<div class=\"prop\"><p>%1:</p></div>"
                        "<div class=\"val\"><p><input type=\"number\" name=\"%2\" value=\"%3\" min=\"%4\" max=\"%5\" step=\"%6\" /></p></div>"
                        "<div class=\"submit\"><p> %7</p></div>"
                        "<div class=\"tooltip\"><p> %8</p></div>"
                        "</div></form>\n")
                .arg(desc)
                .arg(short_d)
                .arg(item->value())
                .arg(item->minimum())
                .arg(item->maximum())
                .arg(item->singleStep())
                .arg((!item->isEnabled() || item->isReadOnly()) ? "" : "<input type=\"submit\" value=\"&gt;&gt;\" />")
                .arg(item->toolTip());
    }
    else if(objTypeName == "QDoubleSpinBox") {
        QDoubleSpinBox *item = (QDoubleSpinBox *) obj;

        ret = QString("<form method=\"post\"><input type=\"hidden\" name=\"action\" value=\"%2\" />"
                      "<div class=\"row\">"
                        "<div class=\"prop\"><p>%1:</p></div>"
                        "<div class=\"val\"><p><input type=\"number\" name=\"%2\" value=\"%3\" min=\"%4\" max=\"%5\" step=\"%6\" /></p></div>"
                        "<div class=\"submit\"><p> %7</p></div>"
                        "<div class=\"tooltip\"><p> %8</p></div>"
                        "</div></form>\n")
                .arg(desc)
                .arg(short_d)
                .arg(item->value())
                .arg(item->minimum())
                .arg(item->maximum())
                .arg(item->singleStep())
                .arg((!item->isEnabled() || item->isReadOnly()) ? "" : "<input type=\"submit\" value=\"&gt;&gt;\" />")
                .arg(item->toolTip());
    }
    else if(objTypeName == "QComboBox") {
        QComboBox *item = (QComboBox *) obj;

        ret = QString("<form method=\"post\"><input type=\"hidden\" name=\"action\" value=\"%2\" />"
                      "<div class=\"row\">"
                        "<div class=\"prop\"><p>%1:</p></div>"
                        "<div class=\"val\"><p>\n<select name=\"%2\" style=\"max-width:170px;\">\n").arg(desc).arg(short_d);
                      int current = item->currentIndex();
                      for (int i = 0; i < item->count(); i++) {
                          ret.append(QString("<option value=\"%1\" %2>%3</option>\n").arg(i).arg(i==current ? "selected" : "").arg(item->itemText(i)));
                      }

                      ret.append(QString("</select>\n</p></div>"
                        "<div class=\"submit\"><p> %1</p></div>"
                        "<div class=\"tooltip\"><p> %2</p></div>"
                        "</div></form>\n")
                        .arg((!item->isEnabled()) ? "" : "<input type=\"submit\" value=\"&gt;&gt;\" />")
                        .arg(item->toolTip()));

    }
    else if(objTypeName == "QRadioButton") {
        QRadioButton *item = (QRadioButton *) obj;

        QString rb_vals;

        if(item->objectName() == "radioButton_rds_music") {
            rb_vals = QString("<input type=\"radio\" name=\"%1\" value=\"true\" %2/> Music <input type=\"radio\" name=\"%1\" value=\"false\" %3/> Speech")
                .arg(short_d).arg(item->isChecked() ? "checked" : "").arg(item->isChecked() ? "" : "checked");
        }

        ret = QString("<form method=\"post\"><input type=\"hidden\" name=\"action\" value=\"%2\" />"
                      "<div class=\"row\">"
                        "<div class=\"prop\"><p>%1:</p></div>"
                        "<div class=\"val\"><p>%3</p></div>"
                        "<div class=\"submit\"><p> %4</p></div>"
                        "<div class=\"tooltip\"><p> %5</p></div>"
                        "</div></form>\n")
                .arg(desc)
                .arg(short_d)
                .arg(rb_vals)
                .arg((!item->isEnabled()) ? "" : "<input type=\"submit\" value=\"&gt;&gt;\" />")
                .arg(item->toolTip());
    }
    else if(objTypeName == "QPushButton") {
        QPushButton *item = (QPushButton *) obj;

        ret = QString("<form method=\"post\"><input type=\"hidden\" name=\"action\" value=\"%2\" />"
                      "<div class=\"row\">"
                        "<div class=\"prop\"><p>%1:</p></div>"
                        "<div class=\"val\"><p>%3</p></div>"
                        "<div class=\"submit\"><p> %4</p></div>"
                        "<div class=\"tooltip\"><p> %5</p></div>"
                        "</div></form>\n")
                .arg(desc)
                .arg(short_d)
                .arg("action")
                .arg((!item->isEnabled()) ? "" : "<input type=\"submit\" value=\"&gt;&gt;\" />")
                .arg(item->toolTip());
    }
    else {
        qDebug() << "unimplemented obj_type: " << objTypeName;
    }

    return ret.toUtf8();
};

QByteArray TableItem::doHTMLParse(HttpRequest *request) {
    QString ret;
    QByteArray action_b = request->getParameter("action");
    QString action(action_b);
    QString *value = new QString(request->getParameter(action_b));

    qDebug() << "set up object: " << obj->objectName() << ", value:" << value;
    bool result = ctrl->emitUpdateItem(obj, value);
    result=TRUE;

    if(result) {
        ret = QString("<p><b><font color=green>Action \"%1\" (%2) done, new param:\"%3\".</font></b></p>")
            .arg(desc).arg(action).arg(*value);
    } else {
        ret = QString("<p><b><font color=red>Action \"%1\" (%2) with new param:\"%3\" - FAILED!</font></b></p>")
            .arg(desc).arg(action).arg(*value);
    }
    return ret.toUtf8();
};
