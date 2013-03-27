#ifndef HARDWARECONTROLLER_H
#define HARDWARECONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include "requestmapper.h"

/**
  This controller displays a HTML Hardware and dumps the submitted input.
*/


class HardwareController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(HardwareController);
public:

    /** Constructor */
    HardwareController(QObject* parent=0);

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
    RequestMapper* rm_ptr;
};

#endif // HARDWARECONTROLLER_H
