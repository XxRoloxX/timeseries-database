#!/bin/bash



for i in {6000..7000}; do
  key="$i"
  value="$i"
  nc -N -q 0 localhost 9002 <<EOF
perf_tests i int $key $value
EOF
done

