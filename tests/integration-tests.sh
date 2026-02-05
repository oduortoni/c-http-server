#!/usr/bin/env bash

cleanup() {
    kill "$SERVER_PID"
}
trap cleanup EXIT

die() {
  echo "$1" >&2
  exit 1
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
    die "Server not responding after 5 seconds"
  fi
done

# Run Hurl tests
hurl --test tests/integration-tests.hurl
