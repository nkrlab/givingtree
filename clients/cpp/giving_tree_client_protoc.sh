#!/bin/bash -e

SRC=src
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
       $SRC/app_messages_for_client.proto
