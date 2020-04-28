#!/bin/bash
set -e
(docker network create shared_editor || true) 2> /dev/null

VOLUME=""
if [ "$#" -eq 1 ]; then
    VOLUME='-v '$1':/data/db'
fi

docker build -t db db
docker run --rm -d --name db -p 27017:27017 $VOLUME --network=shared_editor db

echo "Waiting for server connection to db..."
sleep 5

docker build -t server server
docker run --rm --name server -it -p 1500:1500 --network=shared_editor server
