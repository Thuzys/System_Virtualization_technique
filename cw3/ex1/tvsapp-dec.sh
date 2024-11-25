#!/bin/bash

# Default values for config file and ports file
config_file="tvsapp"
target_directory="/etc/nginx/sites-available/"
ports_file="/tmp/tvsapp_ports.txt"
delta=1  # Default to removing 1 instance if no delta is provided

# Parse arguments (if provided)
while getopts "d:" opt; do
  case $opt in
    d) delta=$OPTARG ;;   # Set delta to specify the number of instances to remove
    *) echo "Usage: $0 [-d delta]"; exit 1 ;;
  esac
done

# Check if delta is a valid positive integer
if ! [[ "$delta" =~ ^[0-9]+$ ]] || [ "$delta" -le 0 ]; then
  echo "Error: delta must be a positive integer."
  exit 1
fi

# Ensure that the number of ports is greater than delta
num_ports=$(wc -l < "$ports_file")
if [ "$num_ports" -le "$delta" ]; then
  echo "Error: The number of ports ($num_ports) must be greater than delta ($delta)."
  exit 1
fi


# Read the ports from the ports_file and store them in an array
mapfile -t ports < "$ports_file"

# Sort the ports in descending order (highest ports first)
sorted_ports=($(echo "${ports[@]}" | tr ' ' '\n' | sort -nr))

# Calculate the number of ports to remove
ports_to_remove=${sorted_ports[@]:0:$delta}

echo "Removing $delta instance(s) with the following ports: ${ports_to_remove[@]}"

# Remove the selected ports from the ports_file
for port in ${ports_to_remove[@]}; do
  # Remove the port from the ports_file
  sed -i "/^$port$/d" "$ports_file"
  
  # Update the configuration file by removing the corresponding server block
  sed -i "/server 127.0.0.1:$port;/d" "$config_file"
done

# Rebuild the Nginx configuration
echo "Rebuilding the Nginx configuration..."

# Remove the upstream block and rewrite it
echo "upstream tvsapp {" > "$config_file"
while IFS= read -r port; do
  echo "    server 127.0.0.1:$port;" >> "$config_file"
done < "$ports_file"
echo "}" >> "$config_file"

# Add the server block if it's not already there
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

echo "Nginx config updated successfully at $config_file."

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
