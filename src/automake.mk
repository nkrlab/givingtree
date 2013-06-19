# Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
#
# This work is confidential and proprietary to Nexon Korea Corporation and
# must not be used, disclosed, copied, or distributed without the prior
# consent of Nexon Korea Corporation.


# THIS FILE IS AUTOMATICALLY GENERATED.
#
# YOU MAY WANT TO EDIT TO MEET YOUR TASTE BUT YOU MUST NOT CALL
# "funapi-admin.py create_project", again.

################################################################################
# Build 를 위한 기본 환경 변수 설정
################################################################################

# For gcc lib
AM_CPPFLAGS = -D_GNU_SOURCE=1
AM_CPPFLAGS += -D_REENTRANT

# For g++ lib
AM_CPPFLAGS += -D__STDC_LIMIT_MACROS=1
AM_CPPFLAGS += -D__STDC_FORMAT_MACROS=1
AM_CPPFLAGS += -D__STDC_CONSTANT_MACROS=1

AM_CPPFLAGS += -D_GLIBCXX_CONCEPT_CHECKS=1

# For boost
AM_CPPFLAGS += $(BOOST_CPPFLAGS)
AM_CPPFLAGS += -DBOOST_FILESYSTEM_VERSION=3

# For FunAPI
AM_CPPFLAGS += $(FUNAPI_CPPFLAGS)

# To access own header files.
AM_CPPFLAGS += -I$(top_srcdir)/include
AM_CPPFLAGS += -I$(top_srcdir)/src
AM_CPPFLAGS += -I$(top_builddir)/include
AM_CPPFLAGS += -I$(top_builddir)/src

# For unit tests
UNITTESTCPPFLAGS = -DBOOST_TEST_DYN_LINK
UNITTESTCPPFLAGS += -DBOOST_TEST_NO_MAIN
UNITTESTLDFLAGS = -dlopen self

# For hooks
LOCAL_HOOKS += src-local
INSTALL_DATA_HOOKS += src-install-data-hook
INSTALL_EXEC_HOOKS += src-install-exec-hook

if DEBUGGING
# For debugging
# Commented out because boost compiled w/o below flags isn't compatible.
#AM_CPPFLAGS += -D_GLIBCXX_DEBUG=1
#AM_CPPFLAGS += -D_GLIBCXX_DEBUG_PEDANTIC=1
else
AM_CPPFLAGS += -DNDEBUG
endif

if PROFILING
AM_CPPFLAGS += -DPROFILING
endif

AM_CXXFLAGS = -Wall
AM_CXXFLAGS += -Wextra
AM_CXXFLAGS += -Werror
AM_CXXFLAGS += -Wformat-nonliteral
AM_CXXFLAGS += -Wformat-security
AM_CXXFLAGS += -Winit-self
AM_CXXFLAGS += -Wno-sign-compare
AM_CXXFLAGS += -fno-omit-frame-pointer
AM_CXXFLAGS += -fno-strict-aliasing

AM_LDFLAGS = -rdynamic
AM_LDFLAGS += $(BOOST_LDFLAGS)
AM_LDFLAGS += $(FUNAPI_LDFLAGS)

LIBS += $(BOOST_FILESYSTEM_LIB)
LIBS += $(BOOST_SYSTEM_LIB)
LIBS += $(BOOST_THREAD_LIB)
LIBS += $(BOOST_UNIT_TEST_FRAMEWORK_LIB)
LIBS += $(FUNAPI_LIBS)

# compile 이전에 생성되어야할 file 들.
BUILT_SOURCES += \
  $(abs_top_builddir)/object_models.json


################################################################################
# 이 특정 project build 를 위한 환경 변수 설정
################################################################################

# project 가 export 하는 component list.
MANIFEST_FILES += $(top_srcdir)/src/MANIFEST.json

# compilie 할 program
pkgbin_PROGRAMS += giving_tree

# library 를 만들기 위해서 필요한 file 들.
giving_tree_SOURCES = \
  $(top_srcdir)/src/event_handlers.cc \
  $(top_srcdir)/src/event_handlers.h \
  $(top_srcdir)/src/giving_tree_server.cc \
  $(top_srcdir)/src/giving_tree_types.h

# library 를 만들기 위해서 필요한 생성되는 file 들.
nodist_giving_tree_SOURCES = \
  $(top_builddir)/src/giving_tree_client_messages.pb.cc \
  $(top_builddir)/src/giving_tree_client_messages.pb.h \
  $(top_builddir)/src/giving_tree_loggers.h \
  $(top_builddir)/src/giving_tree_server_messages.pb.cc \
  $(top_builddir)/src/giving_tree_server_messages.pb.h \
  $(top_builddir)/src/giving_tree.cc \
  $(top_builddir)/src/giving_tree.h \
  $(top_builddir)/src/app_messages.pb.cc \
  $(top_builddir)/src/app_messages.pb.h \
  $(top_builddir)/src/message_dispatcher.h

# program 를 만들 때 preprocessing option.
giving_tree_CPPFLAGS = \
  $(AM_CPPFLAGS)

# program 를 만들 때 linker option.
giving_tree_LDFLAGS = \
  $(AM_LDFLAGS) \
  -dlopen self

# program 를 만들 때 linker option.
giving_tree_LDADD = \
  $(FUNAPI_LIBS) \
  -lfunapi_framework


# Activity log function 생성
$(top_builddir)/src/giving_tree_loggers.h: \
$(top_srcdir)/src/giving_tree_loggers.json
	@$(MKDIR_P) $(top_builddir)/src
	$(FUNAPI_BINDIR)/activity_logger_generator.py \
  --output_file=$@ --definition_file=$<

BUILT_SOURCES += $(top_builddir)/src/giving_tree_loggers.h


# Object class 를 생성.
$(top_builddir)/src/giving_tree.cc \
$(top_builddir)/src/giving_tree.h: \
$(FUNAPI_DATADIR)/app_object.cc.template \
$(FUNAPI_DATADIR)/app_object.h.template \
$(top_srcdir)/src/giving_tree.json \
$(FUNAPI_BINDIR)/object_generator.py
	@$(MKDIR_P) $(top_builddir)/src
	$(FUNAPI_BINDIR)/object_generator.py \
	  --definition_file=$(top_srcdir)/src/giving_tree.json \
	  --template_directory=$(FUNAPI_DATADIR) \
	  --template_name=app_object \
	  --output_directory=$(top_builddir)/src \
	  --output_name=giving_tree

BUILT_SOURCES += $(top_builddir)/src/giving_tree.cc
BUILT_SOURCES += $(top_builddir)/src/giving_tree.h


# app_messages.proto 생성.
# app_messages.proto 파일을 사용하는 곳은 다음과 같다:
# - message_dispatcher.h 생성
# - app_messages.pb.h/cc 생성
$(top_builddir)/./src/app_messages.proto: \
$(FUNAPI_DATADIR)/app_messages.proto.template \
$(top_srcdir)/./src/giving_tree_client_messages.proto \
$(top_srcdir)/./src/giving_tree_server_messages.proto \
$(FUNAPI_BINDIR)/app_message_generator.py
	@$(MKDIR_P) $(top_builddir)/./src
	$(FUNAPI_BINDIR)/app_message_generator.py \
	  --client_proto=$(top_srcdir)/./src/giving_tree_client_messages.proto \
	  --server_proto=$(top_srcdir)/./src/giving_tree_server_messages.proto \
	  --template=$(FUNAPI_DATADIR)/app_messages.proto.template \
	  --output=$(top_builddir)/./src/app_messages.proto

BUILT_SOURCES += $(top_builddir)/./src/app_messages.proto


# Message dispatcher class 를 생성.
$(top_builddir)/src/message_dispatcher.h: \
$(FUNAPI_DATADIR)/message_dispatcher.template.h \
$(top_builddir)/./src/app_messages.proto
	@$(MKDIR_P) $(top_builddir)/src
	$(FUNAPI_BINDIR)/message_dispatcher_generator.py \
	  --protobuf_files=$(top_builddir)/./src/app_messages.proto \
	  --cpp_template_files=$(FUNAPI_DATADIR)/message_dispatcher.template.h \
	  --output_name=$(top_builddir)/src/message_dispatcher

BUILT_SOURCES += $(top_builddir)/src/message_dispatcher.h


# Protobuf cc/h files 생성.

# App message
$(top_builddir)/src/app_messages.pb.cc \
$(top_builddir)/src/app_messages.pb.h: \
$(top_builddir)/./src/app_messages.proto
	@$(LN_S) $$(readlink -f $(top_srcdir)/./src/giving_tree_client_messages.proto) $(top_builddir)/./src/giving_tree_client_messages.proto
	@$(LN_S) $$(readlink -f $(top_srcdir)/./src/giving_tree_server_messages.proto) $(top_builddir)/./src/giving_tree_server_messages.proto
	@$(MKDIR_P) $(top_builddir)/src
	target=`readlink -f $(top_builddir)/./src/app_messages.proto`; \
	protoc --cpp_out=$(top_builddir)/src \
	  --proto_path=`dirname $$target` \
	  --proto_path=$(FUNAPI_INCLUDEDIR) \
	  $$target

BUILT_SOURCES += $(top_builddir)/src/app_messages.pb.cc
BUILT_SOURCES += $(top_builddir)/src/app_messages.pb.h


# Client message
$(top_builddir)/src/giving_tree_client_messages.pb.cc \
$(top_builddir)/src/giving_tree_client_messages.pb.h: \
$(top_srcdir)/./src/giving_tree_client_messages.proto
	@$(MKDIR_P) $(top_builddir)/src
	target=`readlink -f $(top_srcdir)/./src/giving_tree_client_messages.proto`; \
	protoc --cpp_out=$(top_builddir)/src \
	  --proto_path=`dirname $$target` \
	  --proto_path=$(FUNAPI_INCLUDEDIR) \
	  $$target

BUILT_SOURCES += $(top_builddir)/src/giving_tree_client_messages.pb.cc
BUILT_SOURCES += $(top_builddir)/src/giving_tree_client_messages.pb.h


# Server message
$(top_builddir)/src/giving_tree_server_messages.pb.cc \
$(top_builddir)/src/giving_tree_server_messages.pb.h: \
$(top_srcdir)/./src/giving_tree_server_messages.proto
	@$(MKDIR_P) $(top_builddir)/src
	target=`readlink -f $(top_srcdir)/./src/giving_tree_server_messages.proto`; \
	protoc --cpp_out=$(top_builddir)/src \
	  --proto_path=`dirname $$target` \
	  --proto_path=$(FUNAPI_INCLUDEDIR) \
	  $$target

BUILT_SOURCES += $(top_builddir)/src/giving_tree_server_messages.pb.cc
BUILT_SOURCES += $(top_builddir)/src/giving_tree_server_messages.pb.h


# object_models.json 파일을 복사.
$(abs_top_builddir)/object_models.json: \
$(top_srcdir)/src/object_models.json
	ln -s `readlink -f $<` $@


# object_models.json을 /usr/share/app_name에 설치
pkgdata_DATA += $(top_srcdir)/src/object_models.json


################################################################################
# make 시에 발생하는 hook 들.
################################################################################

# 'make all' 을 한 다음에 수행될 hook
src-local:
	@echo "===== Linking manifest files ====="
	mkdir -p $(top_builddir)/manifests
	for file in $(MANIFEST_FILES); do \
	  destdir="$(top_builddir)/manifests/$$(basename $$(dirname $$file))"; \
	  base=$$(basename $$file); \
	  if test ! -d $$destdir; then mkdir -p $$destdir; fi; \
	  if test ! -f $$destdir/$$base; then \
	    ln -s $$(readlink -f $$file) $$destdir/$$base; \
	  fi; \
	done
	@echo "===== Creating wrapper for PROGRAMS ====="
	libpath=""; \
	libs=""; \
	for file in $(pkglib_LTLIBRARIES); do \
	  except=$$(echo $(PRELOAD_EXCEPTION) | grep -v $$file); \
	  if test -n "$$except" -a "$$except" = "$(PRELOAD_EXCEPTION)"; then \
	    libpath="$$libpath:$(abs_top_builddir)/$$(dirname $$file)/.libs"; \
	    libs="$$libs:$$(echo $$(basename $$file) | cut -f1 -d.).so"; \
	  fi; \
	done; \
	for file in $(PROGRAMS); do \
	  destdir="$(abs_top_builddir)/$$(dirname $$file)"; \
	  base=$$(basename $$file); \
	  launcher=$$destdir/$$base-launcher; \
	  if test ! -f $$launcher -o $$launcher -ot $(top_builddir)/Makefile; then \
	    mkdir -p `dirname $$launcher`; \
	    echo "#!/bin/bash" > $$launcher; \
	    echo >> $$launcher; \
	    echo "# Generated by FunFactory Makefile" >> $$launcher; \
	    echo >> $$launcher; \
	    echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:$(FUNAPI_LIBDIR):$$destdir/.libs:$$libpath" >> $$launcher; \
	    echo "export LD_PRELOAD=\$$LD_PRELOAD:$(FUNAPI_LD_PRELOAD):$$libs" >> $$launcher; \
	    echo >> $$launcher; \
	    echo "$(abs_top_builddir)/$$file --framework_manifest_path="$(FUNAPI_DATADIR)/manifests:$(abs_top_builddir)/manifests" --object_models_path="$(abs_top_builddir)/object_models.json" \"\$$@\"" >> $$launcher; \
	    chmod a+rx $$launcher; \
	  fi; \
	done


# 'make install' 시 data file 들을 복사하고 수행될 hook 들
src-install-data-hook:
	@echo "===== Installing manifest files ====="
	for file in $(MANIFEST_FILES); do \
	  destdir="$(DESTDIR)$(pkgdatadir)/manifests/$$(basename $$(dirname $$file))"; \
	  $(mkinstalldirs) $$destdir; \
	  base=$$(basename $$file); \
	  $(INSTALL_DATA) $$(readlink -f $$file) $$destdir/$$base; \
	done


# 'make install' 시 binrary file 들을 복사하고 수행될 hook 들
src-install-exec-hook:
	@echo "===== Creating wrapper for pkgbin_PROGRAMS ====="
	libpath=""; \
	libs=""; \
	for file in $(pkglib_LTLIBRARIES); do \
	  except=$$(echo $(PRELOAD_EXCEPTION) | grep -v $$file); \
	  if test -n "$$except" -a "$$except" = "$(PRELOAD_EXCEPTION)"; then \
	    libs="$$libs:$$(echo $$(basename $$file) | cut -f1 -d.).so"; \
	  fi; \
	done; \
	for file in $(pkgbin_PROGRAMS); do \
	  destdir="$(DESTDIR)/$(pkgbindir)"; \
	  base=$$(basename $$file); \
	  launcher=$$destdir/$$base-launcher; \
	  if test ! -f $$launcher -o $$launcher -ot $(top_builddir)/Makefile; then \
	    mkdir -p `dirname $$launcher`; \
	    echo "#!/bin/bash" > $$launcher; \
	    echo >> $$launcher; \
	    echo "# Generated by FunFactory Makefile" >> $$launcher; \
	    echo >> $$launcher; \
	    echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:$(FUNAPI_LIBDIR):$$destdir" >> $$launcher; \
	    echo "export LD_PRELOAD=\$$LD_PRELOAD:$(FUNAPI_LD_PRELOAD):$$libs" >> $$launcher; \
	    echo >> $$launcher; \
	    echo "$(pkgbindir)/$$base --framework_manifest_path="$(FUNAPI_DATADIR)/manifests:$(pkgdatadir)/manifests" --object_models_path="$(pkgdatadir)/object_models.json" \"\$$@\"" >> $$launcher; \
	    chmod a+rx $$launcher; \
	  fi; \
	done
