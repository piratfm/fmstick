#!/usr/bin/python
#
# -*- coding: utf-8 -*-

import urllib2
import lxml.html
from time import sleep

title = [""]
wait_time = 180

def classical():
    global title
    global wait_time
    req = urllib2.Request('http://www.xfm.co.uk/js/NowPlayingDisplay.aspx?tzc=8&f=http%3A//rope.ixfm.fimc.net/Feeds/NowPlaying/GCap_Media/XFM_London/5893.xml&l=5893&sid=0.611205815570429')
    response = urllib2.urlopen(req)
    the_page = response.read()
    try:
        doc = lxml.html.document_fromstring(the_page)
        title = doc.xpath('/html/body/a[@href="/onair/playlist/recent"]/text()')
        to_wait = doc.xpath('/html/body/span[@id="RcsNextInSeconds"]/text()')
        #to_wait[0] = " 22 "
        time_out = int(to_wait[0])
        print "The parsed title:\"", title[0], "\", timeout:", time_out
        if time_out > 0 and time_out < 180:
            wait_time = time_out
        print "wait_time:\"", wait_time
        return 0
    except:
        wait_time = 60
        return 1


def back_up():
    global title
    global wait_time
    req = urllib2.Request('http://www.xfm.co.uk')
    response = urllib2.urlopen(req)
    the_page = response.read()
    try:
        doc = lxml.html.document_fromstring(the_page)
        title = doc.xpath('//*[@id="listen"]/div/ul/li[1]/a/text()[1]')
        #time_played = doc.xpath('/html/body/span[@id="RcsNextInSeconds"]/text()')
        print "The parsed title: \"", title[0], "\""
        return 0
    except:
        return 1



while (True):
    title = [""]
    wait_time = 30

    if classical():
        if back_up():
            wait_time = 60
        else:
            wait_time = 30
    else:
        if title[0][0] == ',':
            back_up();
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
