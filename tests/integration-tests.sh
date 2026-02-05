#!/usr/bin/env bash

cleanup() {
    kill "$SERVER_PID"
}
trap cleanup EXIT

die() {
  echo "$1" >&2
  exit 1
}

assert_status_code() {
  local URL="$HOST$1"
  local EXPECTED_HTTP_CODE="$2"
  local MSG="$3"

  local RESPONSE=$(curl --silent --write-out '\n%{http_code}' "$URL")
  local CURL_EXIT_CODE=$?

  local ACTUAL_HTTP_CODE=$(echo "$RESPONSE" | tail --lines=1)
  local BODY=$(echo "$RESPONSE" | sed '$d')

  if [[ $CURL_EXIT_CODE != 0 ]]; then
    echo "Accessing $URL: failed with exit code '$CURL_EXIT_CODE'" >&2
    die "$MSG"
  fi

  if [[ $ACTUAL_HTTP_CODE != $EXPECTED_HTTP_CODE ]]; then
    echo "Accessing $URL: mismatched http code '$ACTUAL_HTTP_CODE'" >&2
    die "$MSG"
  fi
}

./bin/server &
SERVER_PID=$!

PORT=9000
HOST="http://localhost:$PORT"

# Wait at most 5 seconds for the server to start
for i in {1..25}; do
  sleep 0.2
  if curl --silent --output /dev/null "$HOST/" ; then
    break
  fi
  if [[ "$i" == 25 ]]; then
    die "Server in not responding after 5 seconds"
  fi
done

assert_status_code "/" 200 "Index page broken"
assert_status_code "/about" 200 "About page broken"
assert_status_code "/404" 404 "404 page broken"
assert_status_code "/garbage-url-asdfa123" 404 "garbage page broken"
assert_status_code "/static/index.html" 200 "static index page broken"
assert_status_code "/static/index.js" 404 "static garbage page broken"
