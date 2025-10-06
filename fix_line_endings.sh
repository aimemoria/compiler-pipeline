#!/bin/bash
# Fix Windows line endings for all source files

echo "Converting line endings from Windows (CRLF) to Unix (LF)..."

# Convert all relevant files
for file in *.l *.y *.c *.h *.src Makefile_new; do
    if [ -f "$file" ]; then
        dos2unix "$file" 2>/dev/null || sed -i 's/\r$//' "$file"
        echo "✓ Fixed: $file"
    fi
done

echo ""
echo "All files converted!"
echo "Now run: make -f Makefile_new"
