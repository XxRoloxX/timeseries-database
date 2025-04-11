#!/bin/bash

# Default values
output_file="perf_results.csv"
verbose=false
batch_size=100

# --- Parse arguments ---
while [[ "$#" -gt 0 ]]; do
  case "$1" in
    -f|--file)
      output_file="$2"
      shift 2
      ;;
    -v|--verbose)
      verbose=true
      shift
      ;;
    -*)
      echo "Unknown option: $1"
      exit 1
      ;;
    *)
      break
      ;;
  esac
done

# Init output file with header
echo "key,time_ms" > "$output_file"

# Buffers
records=()
durations=()

# Function to flush the record buffer to disk
flush_records() {
  for record in "${records[@]}"; do
    echo "$record"
  done >> "$output_file"
  records=()  # clear the buffer
}

# Loop and measure time
for i in {8000..70000}; do
  key="$i"
  value="$i"

  start_ns=$(date +%s%N)

  nc -N -q 0 localhost 9002 <<EOF > /dev/null 2>&1
perf_tests i int $key $value
EOF

  end_ns=$(date +%s%N)
  duration_ms=$(( (end_ns - start_ns) / 1000000 ))

  durations+=("$duration_ms")
  records+=("$key,$duration_ms")

  $verbose && echo "[$key] took ${duration_ms}ms"

  if (( ${#records[@]} >= batch_size )); then
    flush_records
  fi
done

# Flush any remaining records
if (( ${#records[@]} > 0 )); then
  flush_records
fi

# --- Calculate statistics ---
total=0
for d in "${durations[@]}"; do
  total=$((total + d))
done

count=${#durations[@]}
mean=$(echo "scale=2; $total / $count" | bc)

# Sort and find median
sorted=($(printf '%s\n' "${durations[@]}" | sort -n))
if (( count % 2 == 1 )); then
  median="${sorted[$((count / 2))]}"
else
  mid1="${sorted[$((count / 2 - 1))]}"
  mid2="${sorted[$((count / 2))]}"
  median=$(echo "scale=2; ($mid1 + $mid2) / 2" | bc)
fi

# Output statistics
echo
echo "==== Stats ===="
echo "Total time: ${total} ms"
echo "Mean time: ${mean} ms"
echo "Median time: ${median} ms"
echo "Results saved to: $output_file"
