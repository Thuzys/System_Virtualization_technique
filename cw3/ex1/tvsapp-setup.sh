#!/bin/bash

# List of files to copy
files=(
  "tvsapp-reset.sh"
  "tvsapp-inc.sh"
  "tvsapp-dec.sh"
  "tvsapp-stop.sh"
  "tvsapp-start.sh"
  "tvsapp-status.sh"
)

# Target directory
target_directory="/opt/isel/tvsctld/bin/scripts"

# Check if the target directory exists
if [ ! -d "$target_directory" ]; then
  echo "Target directory $target_directory does not exist. Creating it..."
  sudo mkdir -p "$target_directory"
  if [ $? -ne 0 ]; then
    echo "Error: Failed to create target directory $target_directory."
    exit 1
  fi
fi

# Copy each file to the target directory
for file in "${files[@]}"; do
  if [ -f "$file" ]; then
    echo "Copying $file to $target_directory..."
    sudo cp "$file" "$target_directory"
    if [ $? -ne 0 ]; then
      echo "Error: Failed to copy $file."
      exit 1
    fi
  else
    echo "Error: Source file $file does not exist."
    exit 1
  fi
done

# Set proper permissions for the copied files (optional)
echo "Setting appropriate permissions for the files..."
sudo chmod 755 "$target_directory"/*.sh

echo "Files successfully copied to $target_directory."
