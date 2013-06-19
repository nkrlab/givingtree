AC_DEFUN([AX_PROTOBUF],[
  AC_PATH_PROG([PROTOC], [protoc],
               AC_ERROR([Funapi requires protobuf compiler]))
  AC_CHECK_HEADERS(google/protobuf/message.h, ,
                   AC_MSG_ERROR([protobuf header is required]))
])
