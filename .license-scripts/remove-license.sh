#!/bin/bash

# Function to process each file
process_file() {
  local file="$1"

  # Read the first two lines of the file
  first_two_lines=$(head -n 2 "$file")

  # Check if the first two lines match the specified pattern
  if [[ "$first_two_lines" == "/*
 * Copyright (c) 2023 Muhammad Nawaz" ]]; then
    # Remove the first six lines from the file
    tail -n +7 "$file" > "$file.tmp" && mv "$file.tmp" "$file"
    echo "Processed: $file"
  fi
}

# Export the function so it can be used by find
export -f process_file

# Find all .cpp and .hpp files and process them
find . -type f \( -name "*.cpp" -o -name "*.hpp" \) -exec bash -c 'process_file "$0"' {} \;

echo "Processing complete."
