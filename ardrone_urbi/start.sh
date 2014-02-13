#!/bin/sh

SDKPATH=$(dirname $(readlink -f $0))
UOBPATH=$SDKPATH/share/uobjects/
USPATH=$SDKPATH/share/scripts/

# Edit this script as you want :)
SCRIPT=balltracking.u

$SDKPATH/bin/urbi-launch --start $UOBPATH/* -- --host 0.0.0.0 --port 54000 -f $USPATH/$SCRIPT
