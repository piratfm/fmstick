/**
  @file
  @author tipok
*/

#ifndef TXCONTROLLER_H
#define TXCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include "requestmapper.h"

/**
  This controller displays a HTML tx and dumps the submitted input.
*/


class TxController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(TxController);
public:

    /** Constructor */
    TxController(QObject* parent=0);

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
    RequestMapper* rm_ptr;
};

#endif // TXCONTROLLER_H
