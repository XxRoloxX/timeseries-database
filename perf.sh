#!/bin/bash

# {
#   for i in {1..10000}; do
#     key="$i"
#     value="$i"
# 		printf "i %s %s" "$key" "$value" | nc localhost 9000
# 		sleep 0.05
#   done
# } 
# #| nc localhost 9000
# *
# #!/bin/bash


#!/bin/bash

for i in {1..1000}; do
  key="$i"
  value="$i"
  nc -N -q 0 localhost 9001 <<EOF
i $key $value
EOF
done

# Clean up temporary file
#rm temp_command.txt
