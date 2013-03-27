#!/bin/bash

DO_LOOP=1

#wget -O- -dq "http://stream.radiotime.com/listen.stream?streamIds=1046002,1046003,1046004,1108975&rti=dihyG20zJ10fVhgjFxtFWkl0WlkXUAosDApbVV1JJlgXRRN0N1YPDAdQdHQQRlI1WAcVXFZDRGFHGwJzHBsAQAQXCXRqGAVEBEQFUltZYg0%3d%7e%7e%7e"

while :
do

    mplayer -ao alsa:device=hw=1.0 http://media-ice.musicradio.com/XFM

if [ $DO_LOOP -le 0 ]; then
    exit;
fi

done