#!/bin/bash

# Default values for base, delta, and config file
config_file="tvsapp.conf" 
target_directory="/etc/nginx/sites-available/"
ports_file="/tmp/tvsapp_ports.txt"
delta=1  # Default to 1 instance if no delta is provided
port=0
ports=()

# Parse arguments (if provided)
while getopts "d:" opt; do
  case $opt in
    d) delta=$OPTARG ;;   # Set delta to specify the number of new instances
    *) echo "Usage: $0 [-d delta]"; exit 1 ;;
  esac
done

# Check if delta is a valid positive integer
if ! [[ "$delta" =~ ^[0-9]+$ ]] || [ "$delta" -le 0 ]; then
  echo "Error: delta must be a positive integer."
  exit 1
fi

# Read the last used port from the base_port_file
if [ -f "$ports_file" ]; then
  last_port=$(tail -n 1 "$ports_file")
else
  last_port=$base
fi

# Calculate the new ports based on delta
echo "Adding $delta new instance(s) starting from port $((last_port + 1))"

# Ensure that config_file is not a directory
if [ -d "$config_file" ]; then
  echo "Error: $config_file is a directory, not a file."
  exit 1
fi

# Start creating the configuration file
echo "upstream tvsapp {" > "$config_file"

# Read each port from the ports_file and store in the array
while IFS= read -r port; do
  ports+=("$port")  # Append the port to the array
done < "$ports_file"

echo "Ports in the file:"
for p in "${ports[@]}"; do
  echo "    server 127.0.0.1:$p;" >> "$config_file"
done

# Add the new ports to the upstream block
for (( i=0; i<delta; i++ )); do
  port=$(( last_port + 1 + i ))  # Calculate the port for each new instance
  echo "    server 127.0.0.1:$port;" >> "$config_file"
  
  # Append each port to the ports file (if not already there)
  if ! grep -q "^$port$" "$ports_file"; then
    echo "$port" >> "$ports_file"
  fi
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

# Ensure proper copying to the target directory
if [ "$EUID" -eq 0 ]; then
  if [ ! -d "$target_directory" ]; then
    echo "Error: Target directory $target_directory does not exist."
    exit 1
  fi

  if ! cp "$config_file" "$target_directory"; then
    echo "Error: Failed to copy the configuration file to $target_directory."
    exit 1
  fi

  # Create symbolic link
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

if systemctl is-active --quiet nginx; then
  echo "Reloading Nginx configuration..."  
  if [ -f "/opt/isel/tvs/tvsctld/bin/scripts/tvsapp-start.sh" ]; then
    sudo bash /opt/isel/tvs/tvsctld/bin/scripts/tvsapp-start.sh
  else
    echo "Error: tvsapp-start.sh script not found."
    exit 1
  fi
fi
