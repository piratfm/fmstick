/**
  @file
  @author Stefan Frings
*/

#ifndef REQUESTMAPPER_H
#define REQUESTMAPPER_H

#include "httprequesthandler.h"

/**
  The request mapper dispatches incoming HTTP requests to controller classes
  depending on the requested path.
*/

class RequestMapper : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(RequestMapper)
public:

    /**
      Constructor.
      @param parent Parent object
    */
    RequestMapper(QObject* parent=0);

    /**
      Dispatch a request to a controller.
      @param request The received HTTP request
      @param response Must be used to return the response
    */
    void service(HttpRequest& request, HttpResponse& response);
    QObject* mw_ptr;

    bool emitUpdateItem(QObject *obj, QString *val);
    QString fileBase64(QString path);

signals:
    bool updateItem(QObject *obj, QString *val);
private:
    QSettings web_cfg;
    void mainPageStaff(HttpRequest& request, HttpResponse& response);
};

#endif // REQUESTMAPPER_H
