#!/bin/bash

# Default values
ports_file="/tmp/tvsapp_ports.txt"
config_file="tvsapp.conf"
elasticsearch_service="elasticsearch"
nginx_sites_available="/etc/nginx/sites-available/"
nginx_sites_enabled="/etc/nginx/sites-enabled/"

# Start Elasticsearch if it's not already running
echo "Starting Elasticsearch..."

if ! systemctl is-active --quiet "$elasticsearch_service"; then
  systemctl start "$elasticsearch_service"
  echo "Elasticsearch started successfully."
else
  echo "Elasticsearch is already running."
fi

# Ensure the Nginx site is enabled (create symlink if it doesn't exist)
echo "Ensuring the Nginx site is enabled..."

if [ ! -L "$nginx_sites_enabled$config_file" ]; then
  ln -s "$nginx_sites_available$config_file" "$nginx_sites_enabled/"
  echo "Nginx site enabled successfully."
else
  echo "Nginx site is already enabled."
fi

# Start the web app instances for all ports listed in the ports file
echo "Starting web app instances..."

if [ -f "$ports_file" ]; then
  while IFS= read -r port; do
    if ! systemctl is-active --quiet "tvsapp@$port.service"; then
      echo "Starting tvsapp@$port.service..."
      systemctl start "tvsapp@$port.service"
      echo "tvsapp@$port.service started successfully."
    else
      echo "tvsapp@$port.service is already running."
    fi
  done < "$ports_file"
else
  echo "Error: Ports file $ports_file not found."
  exit 1
fi

# Reload Nginx to apply the changes
echo "Reloading Nginx..."

if systemctl is-active --quiet nginx; then
  systemctl reload nginx
  echo "Nginx reloaded successfully."
else
  echo "Nginx is not running. Starting Nginx..."
  systemctl start nginx
  echo "Nginx started successfully."
fi

echo "All specified instances started successfully."
