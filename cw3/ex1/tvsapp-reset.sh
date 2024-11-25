#!/bin/bash

# Default values for scale and base
scale=1
base=35000
config_file="tvsapp"
target_directory="/etc/nginx/sites-available/"
ports_file="/tmp/tvsapp_ports.txt"
port=0

# Parse arguments (if provided)
while getopts "s:b:" opt; do
  case $opt in
    s) scale=$OPTARG ;;   # Set scale from argument
    b) base=$OPTARG ;;    # Set base port from argument
    *) echo "Usage: $0 [-s scale] [-b base] "; exit 1 ;;
  esac
done

# Check if scale is a valid positive integer
if ! [[ "$scale" =~ ^[0-9]+$ ]]; then
  echo "Error: scale must be a positive integer."
  exit 1
fi

echo "Generating Nginx config with scale=$scale, base=$base, and output file $config_file..."

rm "$ports_file"&& touch "$ports_file"

# Start creating the configuration file
echo "upstream tvsapp {" > "$config_file"

# Add server entries to the upstream block
for (( i=0; i<scale; i++ )); do
  port=$(( base + i ))  # Calculate the port for each instance
  
  echo "    server 127.0.0.1:$port;" >> "$config_file"
  
  echo "$port" >> "$ports_file"
  
done

# End the upstream block
echo "}" >> "$config_file"

# Add the server block for Nginx
cat <<EOL >> "$config_file"
server {
    listen 44444 default_server;
    server_name _;

    location / {
        proxy_pass http://tvsapp/;
        proxy_set_header Host \$host;
    }
}
EOL

echo "Nginx config generated successfully at $config_file."

# Check if the script is being run as root; if not, use sudo to copy the file
if [ "$EUID" -eq 0 ]; then
  if [ ! -d "$target_directory" ]; then
    echo "Error: Target directory $target_directory does not exist."
    exit 1
  fi

  if ! cp "$config_file" "$target_directory"; then
    echo "Error: Failed to copy the configuration file to $target_directory."
    exit 1
  fi

  # Optionally create a symbolic link in sites-enabled
  if [ ! -L "/etc/nginx/sites-enabled/tvsapp" ]; then
    ln -s "$target_directory$config_file" /etc/nginx/sites-enabled/
    echo "Symbolic link created in /etc/nginx/sites-enabled/"
  else
    echo "Symbolic link already exists in /etc/nginx/sites-enabled/"
  fi
else
  echo "You need to have root privileges to copy the config file."
  echo "Please run the script with sudo to copy the file to $target_directory"
  sudo cp "$config_file" "$target_directory"
  echo "Configuration file copied to $target_directory"
  
  # Optionally create a symbolic link in sites-enabled
  if [ ! -L "/etc/nginx/sites-enabled/tvsapp" ]; then
    sudo ln -s "$target_directory$config_file" /etc/nginx/sites-enabled/
    echo "Symbolic link created in /etc/nginx/sites-enabled/"
  else
    echo "Symbolic link already exists in /etc/nginx/sites-enabled/"
  fi
fi
