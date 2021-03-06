# Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
#
# This work is confidential and proprietary to iFunFactory Inc. and
# must not be used, disclosed, copied, or distributed without the prior
# consent of iFunFactory Inc.


# Generated by funapi_initiator.py
# You can edit this file to meet your taste.

###############################################################################
# Sets Funapi-specific environment variables to use utility programs.
###############################################################################

set(FUNAPI_ACTIVITY_LOGGER_GENERATOR
    ${FUNAPI_BIN_DIR}/funapi_activity_logger_generator.py)


set(FUNAPI_APP_MESSAGE_GENERATOR
    ${FUNAPI_BIN_DIR}/funapi_app_message_generator.py)


set(FUNAPI_MESSAGE_DISPATCHER_GENERATOR
    ${FUNAPI_BIN_DIR}/funapi_message_dispatcher_generator.py)


set(FUNAPI_OBJECT_GENERATOR
    ${FUNAPI_BIN_DIR}/funapi_object_generator.py)


set(FUNAPI_APP_MESSAGES_PROTO_TEMPLATE
    ${FUNAPI_DATA_DIR}/app_messages.proto.template)


set(FUNAPI_MESSAGE_DISPATCHER_H_TEMPLATE
    ${FUNAPI_DATA_DIR}/message_dispatcher.template.h)



###############################################################################
# Extends the CPP inclusion path
###############################################################################

include_directories(${FUNAPI_INCLUDE_DIR})


include_directories(${CMAKE_CURRENT_BINARY_DIR})



###############################################################################
# Targets to build
###############################################################################

# Main target to generate an application.
# Funapi game app is in the linux shared object form (i.e., .so)
# 'funapi_runner' which is bundled in the Funapi package will
# load the shared object to run the game app.
add_library(
    ${PROJECT_NAME} MODULE
    app_messages.pb.cc
    app_messages.pb.h
    event_handlers.cc
    event_handlers.h
    ${PROJECT_NAME}.cc
    ${PROJECT_NAME}.h
    ${PROJECT_NAME}_loggers.h
    ${PROJECT_NAME}_client_messages.pb.cc
    ${PROJECT_NAME}_client_messages.pb.h
    ${PROJECT_NAME}_server.cc
    ${PROJECT_NAME}_server_messages.pb.cc
    ${PROJECT_NAME}_server_messages.pb.h
    ${PROJECT_NAME}_types.h
    message_dispatcher.h)



###############################################################################
# Generates intermediate output.
###############################################################################

add_custom_command(
    OUTPUT ${PROJECT_NAME}_loggers.h
    COMMAND ${FUNAPI_ACTIVITY_LOGGER_GENERATOR}
            ARGS --output_file=${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_loggers.h
                 --definition_file=${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_loggers.json
    DEPENDS ${FUNAPI_ACTIVITY_LOGGER_GENERATOR}
            ${PROJECT_NAME}_loggers.json)


add_custom_command(
    OUTPUT ${PROJECT_NAME}_server_messages.pb.cc
           ${PROJECT_NAME}_server_messages.pb.h
    COMMAND ${PROTOC}
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_server_messages.proto
    DEPENDS ${PROJECT_NAME}_server_messages.proto)


add_custom_command(
    OUTPUT ${PROJECT_NAME}_client_messages.pb.cc
           ${PROJECT_NAME}_client_messages.pb.h
    COMMAND ${PROTOC}
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_client_messages.proto
    DEPENDS ${PROJECT_NAME}_client_messages.proto)


add_custom_command(
    OUTPUT app_messages.proto
    COMMAND ${FUNAPI_APP_MESSAGE_GENERATOR}
            --client_proto=${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_client_messages.proto
            --server_proto=${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}_server_messages.proto
            --template=${FUNAPI_APP_MESSAGES_PROTO_TEMPLATE}
            --output=${CMAKE_CURRENT_BINARY_DIR}/app_messages.proto
    DEPENDS ${FUNAPI_APP_MESSAGE_GENERATOR}
            ${FUNAPI_APP_MESSAGES_PROTO_TEMPLATE}
            ${PROJECT_NAME}_client_messages.proto
            ${PROJECT_NAME}_server_messages.proto)


add_custom_command(
    OUTPUT message_dispatcher.h
    COMMAND ${FUNAPI_MESSAGE_DISPATCHER_GENERATOR}
            --protobuf_files=${CMAKE_CURRENT_BINARY_DIR}/app_messages.proto
            --cpp_template=${FUNAPI_MESSAGE_DISPATCHER_H_TEMPLATE}
            --output_name=${CMAKE_CURRENT_BINARY_DIR}/message_dispatcher
    DEPENDS ${FUNAPI_MESSAGE_DISPATCHER_GENERATOR}
            ${FUNAPI_MESSAGE_DISPATCHER_H_TEMPLATE}
            app_messages.proto)


add_custom_command(
    OUTPUT app_messages.pb.cc
           app_messages.pb.h
    COMMAND ${PROTOC}
            --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
            --proto_path=${CMAKE_CURRENT_BINARY_DIR}
            --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
            --proto_path=${FUNAPI_INCLUDE_DIR}
            ${CMAKE_CURRENT_BINARY_DIR}/app_messages.proto
    DEPENDS app_messages.proto
            ${PROJECT_NAME}_client_messages.proto
            ${PROJECT_NAME}_server_messages.proto)


add_custom_command(
    OUTPUT ${PROJECT_NAME}.cc
           ${PROJECT_NAME}.h
    COMMAND ${FUNAPI_OBJECT_GENERATOR}
            --definition_file=${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.json
            --template_directory=${FUNAPI_DATA_DIR}
            --template_name=app_object
            --output_directory=${CMAKE_CURRENT_BINARY_DIR}
            --output_name=${PROJECT_NAME}
    DEPENDS ${FUNAPI_OBJECT_GENERATOR}
            ${FUNAPI_DATA_DIR}/app_object.cc.template
            ${FUNAPI_DATA_DIR}/app_object.h.template
            ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.json)



###############################################################################
# Installation rules
###############################################################################

install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib)


install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.json
    DESTINATION share/${PROJECT_NAME}/manifests)


install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/object_models.json
    DESTINATION share/${PROJECT_NAME})
