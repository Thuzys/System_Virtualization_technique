#!/bin/bash

SYSD_DIR=/etc/systemd/system


cp tvsapp@.service $SYSD_DIR 
systemctl daemon-reload
