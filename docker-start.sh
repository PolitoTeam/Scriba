#!/bin/bash

echo "Cleaning..."
docker stop shared_editor_server 2> /dev/null
docker stop shared_editor_db 2> /dev/null
docker network rm shared_editor_network 2> /dev/null

(docker network create shared_editor_network || true) 2> /dev/null

VOLUME=""
if [ "$#" -eq 1 ]; then
    VOLUME='-v '$1':/data/db'
fi

docker build -t shared_editor_db -f db.Dockerfile .
docker run --rm -d --name shared_editor_db -p 27017:27017 $VOLUME --network=shared_editor_network shared_editor_db

echo "Waiting for server connection to db..."
sleep 5

docker build -t shared_editor_server .
docker run --rm --name shared_editor_server -it -p 1500:1500 --network=shared_editor_network shared_editor_server
