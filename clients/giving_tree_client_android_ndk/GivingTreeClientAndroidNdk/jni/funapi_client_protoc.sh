#!/bin/bash -e

SRC=src
GEN=$SRC/gen

mkdir -p $GEN/funapi/account
rm -rf $GEN/funapi/account/*.pb.*

protoc --cpp_out=$GEN \
       --proto_path=$SRC \
       $SRC/funapi/account/account_messages.proto
