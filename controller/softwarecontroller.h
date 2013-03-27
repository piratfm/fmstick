#ifndef SOFTWARECONTROLLER_H
#define SOFTWARECONTROLLER_H

/**
  This controller displays a HTML Software and dumps the submitted input.
*/

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include "requestmapper.h"

class SoftwareController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(SoftwareController);
public:

    /** Constructor */
    SoftwareController(QObject* parent=0);

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
    RequestMapper* rm_ptr;
};

#endif // SOFTWARECONTROLLER_H
