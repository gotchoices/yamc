#!/bin/bash
# YAMC Installer Script
# Installs YAMC components to the system

set -e

# Default installation locations
PREFIX="/usr/local"
LIB_DIR="$PREFIX/lib/yamc"
BIN_DIR="$PREFIX/bin"
DOC_DIR="$PREFIX/share/doc/yamc"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print usage
usage() {
    echo "YAMC Installer"
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --prefix DIR    Installation prefix (default: /usr/local)"
    echo "  --lib-dir DIR   Library directory (default: \$PREFIX/lib/yamc)"
    echo "  --bin-dir DIR   Binary directory (default: \$PREFIX/bin)"
    echo "  --doc-dir DIR   Documentation directory (default: \$PREFIX/share/doc/yamc)"
    echo "  --help          Display this help message"
    echo "  --uninstall     Uninstall YAMC"
    echo ""
    echo "Example:"
    echo "  $0 --prefix /opt"
    exit 1
}

# Process command line arguments
UNINSTALL=false
while [ "$#" -gt 0 ]; do
    case "$1" in
        --prefix)
            PREFIX="$2"
            LIB_DIR="$PREFIX/lib/yamc"
            BIN_DIR="$PREFIX/bin"
            DOC_DIR="$PREFIX/share/doc/yamc"
            shift 2
            ;;
        --lib-dir)
            LIB_DIR="$2"
            shift 2
            ;;
        --bin-dir)
            BIN_DIR="$2"
            shift 2
            ;;
        --doc-dir)
            DOC_DIR="$2"
            shift 2
            ;;
        --uninstall)
            UNINSTALL=true
            shift
            ;;
        --help)
            usage
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            usage
            ;;
    esac
done

# Function to uninstall YAMC
uninstall_yamc() {
    echo -e "${YELLOW}Uninstalling YAMC...${NC}"
    
    # Remove binaries
    if [ -L "$BIN_DIR/yamc" ]; then
        echo "Removing $BIN_DIR/yamc"
        rm -f "$BIN_DIR/yamc"
    fi
    
    if [ -L "$BIN_DIR/yamcity" ]; then
        echo "Removing $BIN_DIR/yamcity"
        rm -f "$BIN_DIR/yamcity"
    fi
    
    # Remove library files
    if [ -d "$LIB_DIR" ]; then
        echo "Removing $LIB_DIR"
        rm -rf "$LIB_DIR"
    fi
    
    # Remove documentation
    if [ -d "$DOC_DIR" ]; then
        echo "Removing $DOC_DIR"
        rm -rf "$DOC_DIR"
    fi
    
    echo -e "${GREEN}YAMC has been uninstalled.${NC}"
    exit 0
}

# Uninstall if requested
if [ "$UNINSTALL" = true ]; then
    uninstall_yamc
fi

# Ensure we're in the YAMC directory
if [ ! -f "./yamc" ] || [ ! -f "./yamcity" ]; then
    echo -e "${RED}Error: Not in the YAMC directory or missing required files.${NC}"
    echo "Please run this script from the YAMC project directory."
    exit 1
fi

# Create directories
echo -e "${YELLOW}Creating directories...${NC}"
mkdir -p "$LIB_DIR"
mkdir -p "$BIN_DIR"
mkdir -p "$DOC_DIR"
mkdir -p "$LIB_DIR/modules"

# Install main scripts
echo -e "${YELLOW}Installing main scripts...${NC}"
cp -f "./yamc" "$LIB_DIR/yamc"
cp -f "./yamcity" "$LIB_DIR/yamcity"
chmod +x "$LIB_DIR/yamc"
chmod +x "$LIB_DIR/yamcity"

# Create symlinks
echo -e "${YELLOW}Creating symlinks...${NC}"
ln -sf "$LIB_DIR/yamc" "$BIN_DIR/yamc"
ln -sf "$LIB_DIR/yamcity" "$BIN_DIR/yamcity"

# Install modules
echo -e "${YELLOW}Installing modules...${NC}"
for module in */; do
    module_name="${module%/}"
    
    # Skip special directories and non-module directories
    if [[ "$module_name" == "." || "$module_name" == ".." || 
          "$module_name" == ".git" || "$module_name" == "test" ]]; then
        continue
    fi
    
    # Skip if it's not a directory or doesn't have a setup file
    if [ ! -d "$module_name" ] || [ ! -f "$module_name/setup" ]; then
        continue
    fi
    
    echo "Installing module: $module_name"
    mkdir -p "$LIB_DIR/modules/$module_name"
    cp -R "$module_name"/* "$LIB_DIR/modules/$module_name/"
done

# Install documentation
echo -e "${YELLOW}Installing documentation...${NC}"
cp -f "README.md" "$DOC_DIR/"
cp -f "LICENSE" "$DOC_DIR/" 2>/dev/null || true

# Create configuration directory
echo -e "${YELLOW}Creating configuration directory...${NC}"
mkdir -p "$PREFIX/etc/yamc" 2>/dev/null || true

echo -e "${GREEN}YAMC has been installed successfully.${NC}"
echo ""
echo "Main executable: $BIN_DIR/yamc"
echo "Automation tool: $BIN_DIR/yamcity"
echo "Module library: $LIB_DIR/modules"
echo "Documentation: $DOC_DIR"
echo ""
echo "To uninstall: $0 --uninstall"
echo ""

exit 0