#!/bin/bash

# Check if the script is being run as superuser
if [ $UID != 0 ] ; then
    echo "must be called as superuser"
    exit 1
fi

TVSD=tvsctld
TVSD_DIR=/opt/isel/tvs/$TVSD
SYSTD_DIR=/etc/systemd/system
SOCK_PATH=/run/isel/tvsctld/request/tvsctld.sock

# Stop the service and disable the socket
echo "Stopping and disabling the systemd service and socket..."
systemctl stop $TVSD.socket
systemctl disable $TVSD.socket
systemctl stop $TVSD.service
systemctl disable $TVSD.service

# Reload systemd manager to apply changes
systemctl daemon-reload

# Remove systemd service and socket files
echo "Removing systemd service and socket files..."
rm -f $SYSTD_DIR/$TVSD.service
rm -f $SYSTD_DIR/$TVSD.socket

# Remove the installed binary from /opt
if [ -f $TVSD_DIR/$TVSD ]; then
    echo "Removing the installed binary from $TVSD_DIR..."
    rm -f $TVSD_DIR/$TVSD
else
    echo "Binary not found in $TVSD_DIR, skipping removal."
fi

# Remove the socket directory if it is empty
if [ -d /run/isel/tvsctld/request ]; then
    echo "Removing socket directory..."
    sudo rm -f /run/isel/tvsctld/request/*
    rmdir -fm /run/isel/tvsctld/request || echo "Directory not empty, skipping removal."
fi

# Optionally remove the main directory if it's empty
if [ -d $TVSD_DIR ]; then
    rmdir $TVSD_DIR || echo "$TVSD_DIR is not empty, skipping removal."
fi

# Feedback to the user
echo "Uninstallation complete."
