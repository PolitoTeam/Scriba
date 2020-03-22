#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: ./generate_server_crt.sh <server_addr>"
    exit 1
fi

openssl req -new -sha256 \
		-key server.key \
		-subj "/C=IT/ST=ITALY/O=POLITO/CN=$1" \
    		-out server.csr

openssl x509 -req -in server.csr -CA ../../Client/certificates/rootCA.crt \
		-CAkey ../../Client/certificates/rootCA.key -CAcreateserial \
		-out server.crt -days 365 -sha256

openssl x509 -in server.crt -text -noout

