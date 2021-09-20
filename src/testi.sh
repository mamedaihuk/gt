#!/usr/bin/env dash
find ./data/*/*.xml.lz -type f \( -name "*game*" -o -name "*demo*" -o -name "*chapter*" \) |
	while read file; do ./scripts/nlzss_cli.py -d -i "$file" -o "${file%.*}"; done
