/**
  @file
  @author tipok
*/

#ifndef RDSCONTROLLER_H
#define RDSCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include "requestmapper.h"

/**
  This controller displays a HTML rds and dumps the submitted input.
*/


class RdsController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(RdsController);
public:

    /** Constructor */
    RdsController(QObject* parent=0);

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
    RequestMapper *rm_ptr;
};

#endif // RDSCONTROLLER_H
