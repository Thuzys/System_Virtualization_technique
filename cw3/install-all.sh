#!/bin/bash

# Ensure the script is run as superuser
if [ $UID != 0 ] ; then
    echo "must be called as superuser"
    exit 1
fi

# Define variables
TVSD=tvsctld
TVSD_SRC=./tvsctld.c
TVSD_DIR=/opt/isel/tvs/$TVSD
SYSTD_DIR=/etc/systemd/system

# Check if the binary exists
if [ ! -x $TVSD ]; then
    if [ -f $TVSD_SRC ]; then
        echo "make before install!"
    else
        echo "wrong dir!"
    fi
    exit
fi

# Create installation directory if it doesn't exist
if [ ! -d $TVSD_DIR ]; then
    if ! mkdir -p $TVSD_DIR ; then
        echo "Failed to create $TVSD_DIR"
        exit
    fi
fi

# Copy necessary files
cp $TVSD $TVSD_DIR
cp $TVSD.service $SYSTD_DIR
cp $TVSD.socket $SYSTD_DIR

# Ensure the ownership of the installed binary
echo "Updating ownership of the binary..."
chown isel:tvsgrp $TVSD_DIR/$TVSD

# Ensure the ownership of the service file
echo "Updating ownership of the service file..."
chown root:tvsgrp $SYSTD_DIR/$TVSD.service

# Ensure the ownership of the socket file (if it exists)
SOCK_PATH=/run/isel/tvsctld/request/tvsctld.sock

# Reload systemd and start the socket
systemctl daemon-reload
systemctl start $TVSD.socket

# Ensure socket file is created and set ownership
if [ -S $SOCK_PATH ]; then
    # Change ownership of the socket to isel:tvsgrp
    chown isel:tvsgrp $SOCK_PATH
    
    # Ensure the socket has the right permissions (660 - rw for user and group)
    chmod 660 $SOCK_PATH
else
    echo "Socket file $SOCK_PATH not found. Please ensure the service is running correctly."
    exit 1
fi

# Call the tvsapp-setup.sh script
if [ -f ./ex1/tvsapp-setup.sh ]; then
    ./ex1/tvsapp-setup.sh
else
    echo "tvsapp-setup.sh not found!"
    exit 1
fi

echo "Installation complete"
