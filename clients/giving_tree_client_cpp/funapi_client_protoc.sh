#!/bin/bash -e

SRC=src
GEN=$SRC/gen

mkdir -p $GEN/funapi_client
rm -rf $GEN/funapi_client/*.pb.*

protoc --cpp_out=$GEN \
       --proto_path=$SRC \
       $SRC/funapi_client/account_messages.proto
