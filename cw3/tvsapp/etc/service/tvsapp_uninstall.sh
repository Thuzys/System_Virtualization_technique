#!/bin/bash

SYSD_DIR=/etc/systemd/system

systemctl stop tvsapp@.service
rm -rf $SYSD_DIR/tvsapp@.service  

systemctl daemon-reload
