#!/bin/bash -e

PROJECT_ROOT=$1
if [[ "$1" == "" ]] ; then
  PROJECT_ROOT=.
fi

SRC=$PROJECT_ROOT/src
GEN=$SRC/gen

mkdir -p $GEN
rm -rf $GEN/*.pb.*

protoc --cpp_out=$GEN \
       --proto_path=$SRC \
       $SRC/giving_tree_client_messages.proto

protoc --cpp_out=$GEN \
       --proto_path=$SRC \
       $SRC/giving_tree_server_messages.proto

protoc --cpp_out=$GEN \
       --proto_path=$SRC \
       $SRC/app_messages.proto
