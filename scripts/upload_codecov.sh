#!/bin/bash   

echo "ARTIFACTS_DIR=${ARTIFACTS_DIR}"

upload_file() {
  local name="$1"
  local flag="$2"
  local file="$3"
  echo "Uploading report with flag $flag..."
  RETRY_COUNT=0
  while [ $RETRY_COUNT -lt 5 ]; do
    if codecov upload-process --disable-search --fail-on-error -t "$CODECOV_TOKEN" -n "$name" -F "$flag" -f "$file"; then
      echo "Upload successful for $flag"
      return 0
    fi
    RETRY_COUNT=$((RETRY_COUNT+1))
    sleep 10s
  done
  if [ $RETRY_COUNT -eq 5 ]; then
    echo "Upload failed for $flag"
    return 1
  fi 
}

success=true
upload_file "debug_unit_test_coverage" "debug unit test" "${ARTIFACTS_DIR}/coverage/debug_unit_test/coverage.lcov" || success=false
upload_file "debug_sqllogical_test_coverage" "debug sqllogical test" "${ARTIFACTS_DIR}/coverage/debug_sqllogical_test/coverage.lcov" || success=false
upload_file "debug_pysdk_test_coverage" "debug pysdk test" "${ARTIFACTS_DIR}/coverage/debug_pysdk_test/coverage.lcov" || success=false
upload_file "debug_http_test_coverage" "debug http test" "${ARTIFACTS_DIR}/coverage/debug_http_test/coverage.lcov" || success=false
upload_file "debug_parallel_test_coverage" "debug parallel test" "${ARTIFACTS_DIR}/coverage/debug_parallel_test/coverage.lcov" || success=false

if [ "$success" = false ]; then
  exit 1
fi

echo "All uploads successful!"