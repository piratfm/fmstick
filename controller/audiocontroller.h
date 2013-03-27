/**
  @file
  @author tipok
*/

#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

#include "httprequest.h"
#include "httpresponse.h"
#include "httprequesthandler.h"
#include "requestmapper.h"

/**
  This controller displays a HTML audio and dumps the submitted input.
*/


class AudioController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(AudioController);
public:

    /** Constructor */
    AudioController(QObject* parent=0);

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
    RequestMapper* rm_ptr;
};

#endif // AUDIOCONTROLLER_H
