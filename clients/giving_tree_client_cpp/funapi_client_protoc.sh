#!/bin/bash -e

SRC=src
GEN=$SRC/gen

mkdir -p $GEN/funapi
rm -rf $GEN/funapi/*.pb.*

protoc --cpp_out=$GEN \
       --proto_path=$SRC \
       $SRC/funapi/account_messages.proto
