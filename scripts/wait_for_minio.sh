#!/bin/bash

MINIO_PORT=${MINIO_PORT:-9005}
MINIO_HOST=${MINIO_HOST:-localhost}

timeout 60s bash -c "
  until curl -s http://${MINIO_HOST}:${MINIO_PORT}/minio/health/live > /dev/null 2>&1; do
    echo \"Waiting for MinIO to be healthy...\"
    sleep 2
  done
  echo \"MinIO is ready!\"
" || {
  echo "Warning: MinIO is unhealthy in 60s"
  exit 1
}
