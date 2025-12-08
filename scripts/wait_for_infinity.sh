#!/bin/bash

timeout 60s bash -c '
  until [[ "$(curl -s http://localhost:23820/admin/node/current | jq -r ".node.status")" =~ ^(started|alive)$ ]]; do
    echo "Waiting for Infinity to be healthy..."
    sleep 5
  done
  echo "Infinity is ready!"
' || {
  echo "Warning: Infinity is unhealthy in 60s"
  exit 1
}
