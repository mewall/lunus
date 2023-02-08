# generate_iobs.sh

#!/bin/bash
if [ -e iobs.mtz ]; then
  rm iobs.mtz
fi

sftools <<EOF
read $1
calc J col Iobs = col 1 col 1 * 100. /
calc Q col SIGIobs = col 1 10. /
write $2
quit
EOF
