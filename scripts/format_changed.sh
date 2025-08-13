#!/bin/bash

# Script to format only locally changed C++ files
# Usage: ./scripts/format_changed.sh [--staged] [--unstaged] [--all]

set -eo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
}

print_header() {
    echo -e "${BLUE}[HEADER]${NC} $1"
}

# Default behavior: format unstaged changes
FORMAT_STAGED=false
FORMAT_UNSTAGED=true
FORMAT_UPSTREAM=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --staged)
            FORMAT_STAGED=true
            FORMAT_UNSTAGED=false
            shift
            ;;
        --unstaged)
            FORMAT_STAGED=false
            FORMAT_UNSTAGED=true
            shift
            ;;
        --upstream)
            FORMAT_UPSTREAM=true
            FORMAT_STAGED=false
            FORMAT_UNSTAGED=false
            shift
            ;;
        --all)
            FORMAT_STAGED=true
            FORMAT_UNSTAGED=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --staged     Format only staged changes"
            echo "  --unstaged   Format only unstaged changes (default)"
            echo "  --upstream   Format files changed compared to upstream"
            echo "  --all        Format both staged and unstaged changes"
            echo "  -h, --help   Show this help message"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Check if we're in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    print_error "Not in a git repository."
    exit 1
fi

# Check if clang-format is available
if ! command -v clang-format &> /dev/null; then
    print_error "clang-format not found."
    exit 1
fi

# Get changed files safely
get_changed_files() {
    local diff_type="$1"
    local files=()
    
    case "$diff_type" in
        staged)
            mapfile -t files < <(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(cpp|h|hpp|cppm|cmake)$' | grep -v -E 'third_party/|build/|cmake-build/' || true)
            ;;
        unstaged)
            mapfile -t files < <(git diff --name-only --diff-filter=ACM | grep -E '\.(cpp|h|hpp|cppm|cmake)$' | grep -v -E 'third_party/|build/|cmake-build/' || true)
            ;;
        upstream)
            if git ls-remote --exit-code upstream main &> /dev/null; then
                mapfile -t files < <(git diff --name-only upstream/main..HEAD | grep -E '\.(cpp|h|hpp|cppm|cmake)$' | grep -v -E 'third_party/|build/|cmake-build/' || true)
            else
                mapfile -t files < <(git diff --name-only main..HEAD | grep -E '\.(cpp|h|hpp|cppm|cmake)$' | grep -v -E 'third_party/|build/|cmake-build/' || true)
            fi
            ;;
    esac
    
    printf '%s\n' "${files[@]}"
}

# Format files with error handling
format_files() {
    local files=()
    mapfile -t files <<< "$1"
    local diff_type="$2"
    
    if [[ ${#files[@]} -eq 0 ]]; then
        print_warning "No C++ files to format in $diff_type changes."
        return 0
    fi
    
    print_header "Formatting $diff_type changes (${#files[@]} files):"
    printf '  - %s\n' "${files[@]}"
    
    local errors=0
    for file in "${files[@]}"; do
        if [[ ! -f "$file" ]]; then
            print_warning "File not found: $file"
            continue
        fi
        
        if ! clang-format -i "$file"; then
            print_error "Failed to format: $file"
            ((errors++))
        fi
    done
    
    if [[ $errors -gt 0 ]]; then
        print_error "Failed to format $errors file(s)."
        return 1
    fi
    
    print_status "Successfully formatted ${#files[@]} file(s)."
}

# Main execution
print_header "C++ Code Formatter for Changed Files"

if [[ "$FORMAT_STAGED" == true ]]; then
    files=$(get_changed_files "staged")
    format_files "$files" "staged" || exit 1
fi

if [[ "$FORMAT_UNSTAGED" == true ]]; then
    files=$(get_changed_files "unstaged")
    format_files "$files" "unstaged" || exit 1
fi

if [[ "$FORMAT_UPSTREAM" == true ]]; then
    files=$(get_changed_files "upstream")
    format_files "$files" "upstream" || exit 1
fi

print_header "Formatting completed!"
exit 0