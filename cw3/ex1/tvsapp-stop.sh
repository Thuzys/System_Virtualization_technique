#!/bin/bash

# Default values
ports_file="/tmp/tvsapp_ports.txt"
config_file="tvsapp.conf"
elasticsearch_service="elasticsearch"
nginx_sites_enabled="/etc/nginx/sites-enabled/"

# Parse arguments
stop_elasticsearch=false

while getopts "db" opt; do
  case $opt in
    d) stop_elasticsearch=true ;;  # If -d option is provided, stop Elasticsearch
    *) echo "Usage: $0 [-db]"; exit 1 ;;
  esac
done

# Stop Elasticsearch if -d option is passed
if [ "$stop_elasticsearch" = true ]; then
  echo "Stopping Elasticsearch..."
  if systemctl is-active --quiet "$elasticsearch_service"; then
    systemctl stop "$elasticsearch_service"
    echo "Elasticsearch stopped successfully."
  else
    echo "Elasticsearch is not running."
  fi
fi

# Disable the Nginx site (remove the symbolic link in sites-enabled)
echo "Disabling the Nginx site..."
if [ -L "$nginx_sites_enabled$config_file" ]; then
  rm "$nginx_sites_enabled$config_file"
  echo "Nginx site disabled successfully."
else
  echo "Nginx site is not enabled or the symbolic link does not exist."
fi

# Read the ports from the ports_file and stop the corresponding web app instances
echo "Stopping web app instances..."

if [ -f "$ports_file" ]; then
  while IFS= read -r port; do
    if systemctl is-active --quiet "tvsapp@$port.service"; then
      echo "Stopping tvsapp@$port.service..."
      systemctl stop "tvsapp@$port.service"
      echo "tvsapp@$port.service stopped successfully."
    else
      echo "tvsapp@$port.service is not running."
    fi
  done < "$ports_file"
else
  echo "Error: Ports file $ports_file not found."
  exit 1
fi

# Reload Nginx to apply changes
echo "Reloading Nginx..."
if systemctl is-active --quiet nginx; then
  systemctl reload nginx
  echo "Nginx reloaded successfully."
else
  echo "Nginx is not running."
fi

echo "All specified instances stopped successfully."
