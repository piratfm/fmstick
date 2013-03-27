#ifndef TABLEITEM_H
#define TABLEITEM_H

#include "requestmapper.h"

class TableItem {
public:
    TableItem(RequestMapper *c, QString s, QString d, QObject *o);
    QByteArray GenHTMLForm();
    QByteArray doHTMLParse(HttpRequest *request);

private:
    QString short_d;
    QString desc;
    QObject *obj;
    RequestMapper *ctrl;
    //int obj_type;
};


#endif // TABLEITEM_H
