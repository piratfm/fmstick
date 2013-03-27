#!/usr/bin/python
#
# -*- coding: utf-8 -*-

import urllib2
import lxml.html
from time import sleep
import re

title = [" "]
wait_time = 180

def classical():
    global title
    global wait_time
    req = urllib2.Request('http://polling.bbc.co.uk/modules/onairpanel/include/bbc_radio_one.jsonp')
    response = urllib2.urlopen(req)
    the_page = response.read()
    print the_page
    #<div id=\"programmes-oap-now\" title=\"In New DJs We Trust\"> ON NOW : <a href=\"http:\/\/www.bbc.co.uk\/programmes\/b01p2397\">In New DJs We Trust<\/a> <\/div>
    #<div id=\"programmes-oap-info-scroll\">Playing: Dig Your Own Hole by The Chemical Brothers<\/div>
    #programmes-oap-info-scroll\\\">(.[^<]+)<\\\/div>
    m = re.search("programmes-oap-info-scroll\\\\\">(.[^<]+)<\\\\/div>", the_page)
    text = m.group(1)
    print text
    if text.startswith("Playing: "):
        text = text[9:]
    print text
    if text.find(" by ") != -1:
        m = re.search("(.+) by (.+)", text)
        text = m.group(2) + " - " + m.group(1)
    print text
        #print text

    title = [text]
    #print the_page
    return 0;

def back_up():
    return 1



while (True):
    title = [""]
    wait_time = 30

    classical()
#        if back_up():
#            wait_time = 60
#        else:
#            wait_time = 30
#    else:
#        if title[0][0] == ',':
#            back_up();
            #wait_time = 30

    try:
        f = open('/home/tipok/sample4.txt', 'w')
        f.write(title[0])
        f.flush()
        f.close()
    except:
        print "cant write to file"

    print "Final title: \"", title[0], "\", sleeping:", wait_time
    sleep(wait_time)
