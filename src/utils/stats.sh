#!/bin/bash

set -e

BLUE='\033[0;34m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo ""
echo "════════════════════════════════════════════════════════════════"
echo "    WildFoxEngine Code Statistics"
echo "════════════════════════════════════════════════════════════════"
echo ""

if command -v tokei &> /dev/null; then
    echo -e "${GREEN}Using tokei${NC}"
    echo ""
    
    echo "══ Overall Statistics ══"
    tokei src \
        --exclude build \
        --exclude external \
        --exclude cmake-build-* \
        --sort lines
    
    echo ""
    echo "══ Statistics by Module ══"
    echo ""
    
    for dir in src/*/; do
        if [ -d "$dir" ]; then
            module=$(basename "$dir")
            echo -e "${BLUE}▸ $module${NC}"
            tokei "$dir" --compact
            echo ""
        fi
    done
    
elif command -v cloc &> /dev/null; then
    echo -e "${YELLOW}Using cloc (tokei not found)${NC}"
    echo ""
    
    cloc src \
        --exclude-dir=build,external,cmake-build-debug \
        --quiet
    
else
    echo -e "${YELLOW}Using basic wc (install tokei or cloc for better stats)${NC}"
    echo ""
    
    total=$(find src -type f \( -name "*.cpp" -o -name "*.cppm" -o -name "*.h" \) | xargs wc -l | tail -1 | awk '{print $1}')
    files=$(find src -type f \( -name "*.cpp" -o -name "*.cppm" -o -name "*.h" \) | wc -l)
    
    echo "Total files: $files"
    echo "Total lines: $total"
fi

echo ""
echo "══ Project Info ══"
echo "Modules:      $(ls -d src/*/ 2>/dev/null | wc -l)"
echo "Git commits:  $(git rev-list --count HEAD 2>/dev/null || echo 'N/A')"
echo "Contributors: $(git shortlog -sn --all 2>/dev/null | wc -l || echo 'N/A')"

echo ""
echo "════════════════════════════════════════════════════════════════"
echo ""

if [ "$1" = "--save" ]; then
    echo "Saving to STATISTICS.md..."
    {
        echo "# WildFoxEngine Code Statistics"
        echo ""
        echo "Generated: $(date)"
        echo ""
        if command -v tokei &> /dev/null; then
            echo '```'
            tokei src --exclude build --exclude external
            echo '```'
        else
            echo "Total files: $files"
            echo "Total lines: $total"
        fi
    } > STATISTICS.md
    echo "Saved to STATISTICS.md"
fi