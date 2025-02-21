#!/bin/bash

# Define base URL for the Infinity instance
BASE_URL="http://127.0.0.1:23820"

# Show information about the current node
echo -e '-- Show information about the current node'
curl --request GET \
     --url "${BASE_URL}/admin/node/current" \
     --header 'accept: application/json' \
     --header 'content-type: application/json'
echo ""

echo -e '-- Test done'