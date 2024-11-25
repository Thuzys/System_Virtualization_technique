#!/bin/bash

# Default values
ports_file="/tmp/tvsapp_ports.txt"
elasticsearch_service="elasticsearch"
nginx_service="nginx"
nginx_sites_enabled="/etc/nginx/sites-enabled/"
config_file="tvsapp"

# Check the status of Nginx
echo "Checking Nginx status..."
if systemctl is-active --quiet "$nginx_service"; then
  echo "Nginx: Running"
else
  echo "Nginx: Not running"
fi

# Check if the Nginx site is enabled
if [ -L "$nginx_sites_enabled$config_file" ]; then
  echo "Nginx site: Enabled"
else
  echo "Nginx site: Disabled"
fi

# Check the status of web app instances
echo "Checking web app instances status..."
if [ -f "$ports_file" ]; then
  while IFS= read -r port; do
    if systemctl is-active --quiet "tvsapp@$port.service"; then
      echo "Web app instance (port $port): Running"
    else
      echo "Web app instance (port $port): Not running"
    fi
  done < "$ports_file"
else
  echo "Error: Ports file $ports_file not found."
  exit 1
fi

# Check the status of Elasticsearch (DB)
echo "Checking Elasticsearch (DB) status..."
if systemctl is-active --quiet "$elasticsearch_service"; then
  echo "Elasticsearch: Running"
else
  echo "Elasticsearch: Not running"
fi

echo "Status check completed."
