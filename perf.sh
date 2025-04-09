#!/bin/bash



for i in {6000..7000}; do
  key="$i"
  value="$i"
  nc -N -q 0 localhost 9001 <<EOF
i $key $value
EOF
done

