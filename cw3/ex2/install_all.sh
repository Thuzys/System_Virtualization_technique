#!/bin/bash

if [ $UID != 0 ] ; then
	echo "must be called as superuser"
	exit 1
fi

TVSD=tvsctld
TVSD_SRC=./tvsctld.c

if [ ! -x $TVSD ]; then
	if [ -f $TVSD_SRC ]; then
		echo make before install!
	else
		echo wrong dir!
	fi
	exit
fi

TVSD_DIR=/opt/isel/tvs/$TVSD
SYSTD_DIR=/etc/systemd/system


if [ ! -d $TVSD_DIR ]; then
	if ! mkdir -p $TVSD_DIR ; then
		echo Failed to create $TVSD_DIR
		exit
	fi
fi

cp $TVSD $TVSD_DIR
cp $TVSD.service $SYSTD_DIR
cp $TVSD.socket $SYSTD_DIR

systemctl daemon-reload

sudo systemctl start $TVSD.socket

echo "done"