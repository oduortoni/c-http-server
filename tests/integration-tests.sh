#!/usr/bin/env bash

cleanup() {
    kill "$SERVER_PID"
    wait $SERVER_PID
    EXIT_CODE=$?
    [ $EXIT_CODE -ne 0 ] && [ $EXIT_CODE -ne 143 ] && die "server failed with exit code $EXIT_CODE"
}
trap cleanup EXIT

die() {
  printf "\033[31mINTEGRATION TEST ERROR:\033[0m $1\n" >&2
  exit 1
}

./bin/server &
SERVER_PID=$!

PORT=${PORT:-9000}
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
if [ -z "$HURL" ]; then
  HURL="hurl"
fi

$HURL --test --variable PORT="$PORT" tests/integration-tests.hurl

[ $? -ne 0 ] && die "hurl failed"

# Check for closing connection
printf '' > /dev/tcp/127.0.0.1/9000
sleep 0.3
