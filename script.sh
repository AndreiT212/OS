#!/bin/bash

# Ensure the C file path is provided as an argument
if [ $# -lt 1 ]; then
  echo "Compiling <c_file>"
  exit 1
fi

# Extract the C file path from the argument
c_file="$1"

# Compile the C file and check for errors and warnings
output=$(gcc -Wall -Wextra "$c_file" 2>&1)

# Extract the error and warning counts
error_count=$(echo "$output" | grep -c "error:")
warning_count=$(echo "$output" | grep -c "warning:")

# Print the counts
echo "Errors: $error_count"
echo "Warnings: $warning_count"

# Exit with the error count as the exit code
exit "$error_count"
