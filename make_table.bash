#!/bin/bash

# Output CSV file
output_file="mandelmovie_times.csv"

# Write the header row with thread numbers (columns)
echo -n "Process\\Thread," > "$output_file"
for j in {1..20}
do
    echo -n "$j," >> "$output_file"
done
echo "" >> "$output_file"

# Iterate through each process and capture the real time for each thread
for i in {1..20}
do
    # Write the process number (row header)
    echo -n "$i," >> "$output_file"

    # For each thread, capture the real time
    for j in {1..20}
    do
        # Capture the real time
        real_time=$( { time ./mandelmovie -p $i -t $j; } 2>&1 | grep real | awk '{print $2}' )

        # Output the real time in the corresponding cell, followed by a comma
        echo -n "$real_time," >> "$output_file"
    done

    # End the current row with a newline
    echo "" >> "$output_file"
done

echo "CSV file generated: $output_file"
