#!/bin/bash

if [ $UID != 0 ] ; then
    echo "must be called as superuser"
    exit 1
fi

TVSD=tvsctld
TVSD_DIR=/opt/isel/tvs/$TVSD
SYSTD_DIR=/etc/systemd/system

if [ ! -d $TVSD_DIR ]; then
    echo "TVSD not installed"
    exit
fi

if systemctl is-active --quiet $TVSD ; then
    echo "stopping $TVSD"
    systemctl stop $TVSD
fi

systemctl stop $TVSD.socket

rm -rf $TVSD_DIR
rm -f $SYSTD_DIR/$TVSD.service
rm -f $SYSTD_DIR/$TVSD.socket

systemctl daemon-reload

echo "done"