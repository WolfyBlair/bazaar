#!/bin/bash

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

"$PROJECT_DIR/web-server.py" "$PROJECT_DIR/web" 8080
