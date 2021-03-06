#!/usr/bin/env python
# coding=utf8
# Copyright (C) 2013 iFunFactory Inc. All Rights Reserved.
#
# This work is confidential and proprietary to iFunFactory Inc. and
# must not be used, disclosed, copied, or distributed without the prior
# consent of iFunFactory Inc.

"""
Funapi GivingTree client.
"""

import gflags
import logging
import os
import re
import sys
import datetime
import uuid

from twisted.internet import reactor
from twisted.internet import stdio
from twisted.internet import endpoints
from twisted.protocols import basic

import funapi.account.account_messages_pb2 as account_pb
import app_messages_pb2 as app_pb

# apps 의 top-level tools directory 를 python path 에 추가한다.
# 이로써 funapi_server_stub 을 include 할 수 있다.
exec_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(exec_dir + '/tools')
import funapi_server_stub

LOG_FORMAT = '%(asctime)-15s %(message)s'

FLAGS = gflags.FLAGS
gflags.DEFINE_string('protocol',
                     'tcp',
                     'protocol for the server connection')
gflags.DEFINE_string('ip',
                     '127.0.0.1',
                     'server ip address to connect')
gflags.DEFINE_integer('port',
                      8012,
                      'server port to connect')

WHITE_SPACE = re.compile(r'\s+')


# pylint: disable-msg=R0923,R0904
class ChatReader(basic.LineReceiver, funapi_server_stub.CallbackInterface):
  """
  stdin 으로부터 line 단위로 읽고 funapi server stub 으로 전송함.

  또한 funapi server stub 의 event 들을 처리함.
  """

  delimiter = os.linesep

  def __init__(self):
    """ctor"""
    self.protocol = None

  def got_protocol(self, protocol):
    """Defer callback"""
    assert isinstance(protocol, funapi_server_stub.HandlerProtocol)
    self.protocol = protocol

  def connection_failed(self, err):
    """Defer errback"""
    print >> sys.stdout, 'Cannot make a connection: %s' % str(err)
    sys.stdout.flush()
    # pylint: disable-msg=E1101
    reactor.crash()

  # pylint: disable-msg=C0103
  def lineReceived(self, line):
    """LineReceiver 로부터."""
    account_msg = account_pb.AccountMessage()
    request = account_msg.client_message

    cmds = WHITE_SPACE.split(line.strip())
    cmd = cmds[0]

    if (cmd == "login" or cmd == "i"):
      if (len(cmds) == 2):
        request.login.authentication_service_provider = "free"
        request.login.local_account = cmds[1]
      elif (len(cmds) == 4):
        request.login.authentication_service_provider = cmds[1]
        request.login.local_account = cmds[2]
        request.login.authentication_key = cmds[3]
      else:
        print >> sys.stdout, 'error: len(cmds) != 2 and != 4 : %d' % len(cmds)
        print >> sys.stdout, 'example> login id'
        print >> sys.stdout, 'example> login service-provider id key'
        sys.stdout.flush()
        return
      request.type = account_pb.ClientAccountMessage.kAccountLoginRequest

    elif (cmd == "logout" or cmd == "o"):
      if (len(cmds) != 1):
        print >> sys.stdout, 'error: len(cmds) != 1: %d' % len(cmds)
        print >> sys.stdout, 'example> logout'
        sys.stdout.flush()
        return
      request.type = account_pb.ClientAccountMessage.kAccountLogoutRequest

    elif (cmd == "quit" or cmd == "q"):
      if (len(cmds) != 1):
        print >> sys.stdout, 'error: len(cmds) != 1: %d' % len(cmds)
        print >> sys.stdout, 'example> quit'
        sys.stdout.flush()
        return
      reactor.crash()

    else:
      request.type = account_pb.ClientAccountMessage.kClientAppMessage
      if (cmd == ""):
        request.app_message.Extensions[app_pb.client_message_type] = \
            app_pb.ClientAppMessageType.kPlayerTakeApple

      elif (cmd == "give"):
        if (len(cmds) != 3):
          print >> sys.stdout, 'error: len(cmds) != 3: %d' % len(cmds)
          print >> sys.stdout, 'example> give account_id 1'
          sys.stdout.flush()
          return

        target_id = cmds[1]
        if (len(target_id) <= 0):
          print >> sys.stdout, 'error: wrong target account_id.'
          sys.stdout.flush()
          return

        apple_count = long(cmds[2])

        request.app_message.Extensions[app_pb.client_message_type] = \
            app_pb.ClientAppMessageType.kPlayerGiveApples
        give_msg = request.app_message.Extensions[app_pb.player_give_apples]
        give_msg.target_service_provider = "free"
        give_msg.target_local_account = target_id
        give_msg.apple_count = apple_count

      elif (cmd == "talk"):
        if (len(cmds) != 3):
          print >> sys.stdout, 'error: len(cmds) != 3: %d' % len(cmds)
          print >> sys.stdout, 'example> talk account_id hello,you'
          sys.stdout.flush()
          return

        receiver_id = cmds[1]
        if (len(receiver_id) <= 0):
          print >> sys.stdout, 'error: wrong receiver account_id.'
          sys.stdout.flush()
          return

        talk = cmds[2]
        if (len(talk) <= 0):
          print >> sys.stdout, 'error: wrong talk.'
          sys.stdout.flush()
          return

        request.app_message.Extensions[app_pb.client_message_type] = \
            app_pb.ClientAppMessageType.kAccountSendTalk
        talk_msg = \
            request.app_message.Extensions[app_pb.account_send_talk]
        talk_msg.receiver_service_provider = "free"
        talk_msg.receiver_local_account = receiver_id
        talk_msg.talk = talk

      else:
        request.app_message.Extensions[app_pb.client_message_type] = \
            app_pb.ClientAppMessageType.kPlayerRegisterName
        request.app_message.Extensions[app_pb.player_register_name].name = cmd

    self.protocol.write(account_msg.SerializeToString())
    print >> sys.stdout, 'on_message_sent: ' + str(datetime.datetime.now())

  # pylint: disable-msg=C0103
  def rawDataReceived(self, data):
    """LineReceiver 로부터."""
    pass

  def on_connection_lost(self, protocol, reason):
    """funapi_server_stub.EventHandlerInterface"""
    assert self.protocol == protocol
    self.protocol = None
    reactor.crash()

  def on_message_received(self, protocol, message):
    """funapi_server_stub.EventHandlerInterface"""
    print >> sys.stdout, 'on_message_received: ' + str(datetime.datetime.now())

    account_msg = account_pb.AccountMessage()
    account_msg.ParseFromString(message)
    server_msg = account_msg.server_message
    msg_type = server_msg.type

    if (msg_type == account_pb.ServerAccountMessage.kAttributeUpdatesMessage):
      attribute_updates = server_msg.attribute_updates
      for attr_update in attribute_updates.attribute_update:
        obj_uuid = uuid.UUID(bytes = attr_update.object_uuid)
        print >> sys.stdout, 'AttrUpdate[' + str(obj_uuid)[:8] \
                             + ']:[' + str(attr_update.attribute_name) \
                             + ']: [' + str(attr_update.new_json) + '].'
        sys.stdout.flush()
      print >> sys.stdout, ''
      sys.stdout.flush()

    elif (msg_type == account_pb.ServerAccountMessage.kServerAppMessage):
      app_msg = server_msg.app_message
      app_msg_type = app_msg.Extensions[app_pb.server_message_type]
      if (app_msg_type == app_pb.ServerAppMessageType.kAccountReceiveTalk):
        talk_msg = app_msg.Extensions[app_pb.account_receive_talk]
        sender_service_provider = talk_msg.sender_service_provider
        sender_local_account = talk_msg.sender_local_account
        talk = talk_msg.talk
        sender = sender_service_provider + ':' + sender_local_account
        print >> sys.stdout, 'Talk from [' + sender + ']: [' + talk + '].\n'
        sys.stdout.flush()
      elif (app_msg_type == \
            app_pb.ServerAppMessageType.kAccountSendTalkResponse):
        response = app_msg.Extensions[app_pb.account_send_talk_response]
        fail_code = response.fail_code
        fail_description = response.fail_description
        if (fail_code == 0):
          print >> sys.stdout, 'TalkSend: success.\n'
          sys.stdout.flush()
        else:
          print >> sys.stdout, 'TalkSend: error: ' + fail_description + '.\n'
          sys.stdout.flush()
      else:
        print >> sys.stdout, 'Unknown Message Type ' + str(app_msg_type) \
                             + '.\n'
        sys.stdout.flush()

    elif (msg_type == account_pb.ServerAccountMessage.kAccountLoginResponse):
      login_response = server_msg.login
      success = (login_response.fail_code == 0)
      print >> sys.stdout, 'AccountLoginResponse: ' + str(success) + '.\n'
      sys.stdout.flush()

    elif (msg_type == account_pb.ServerAccountMessage.kAccountLogoutResponse):
      logout_response = server_msg.logout
      success = (logout_response.fail_code == 0)
      print >> sys.stdout, 'AccountLogoutResponse: ' + str(success) + '.\n'
      sys.stdout.flush()

    elif (msg_type == account_pb.ServerAccountMessage.kAccountTimeout):
      print >> sys.stdout, 'AccountTimeout.\n'
      sys.stdout.flush()

    else:
      print >> sys.stdout, 'Unknown Message Type: ' + str(msg_type) + '.\n'
      sys.stdout.flush()

def main(argv):
  """Main function"""
  logging.basicConfig(format=LOG_FORMAT)

  # Command line 인자를 parsing 한다.
  FLAGS(argv)

  # Standard input 을 multiplexing 한다.
  chat_reader = ChatReader()
  stdio.StandardIO(chat_reader)

  # Connection 을 시작한다.
  if FLAGS.protocol == 'tcp':
    end_point = endpoints.TCP4ClientEndpoint(reactor, FLAGS.ip, FLAGS.port)
  elif FLAGS.protocol == 'ssl':
    end_point = endpoints.SSL4ClientEndpoint(reactor, FLAGS.ip, FLAGS.port)
  else:
    assert False, 'Invalid protocol type: %s' % FLAGS.protocol
  factory = funapi_server_stub.HandlerFactory(chat_reader)
  defer = end_point.connect(factory)
  defer.addCallbacks(chat_reader.got_protocol, chat_reader.connection_failed)

  # Polling 을 시작하고, stdin 혹은 server socket 으로부터의
  # 입력을 처리한다.
  # pylint: disable-msg=E1101
  reactor.run()


if __name__ == '__main__':
  main(sys.argv)
