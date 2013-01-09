#!/usr/bin/env python
# coding=utf8
# Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
#
# This work is confidential and proprietary to Nexon Korea Corporation and
# must not be used, disclosed, copied, or distributed without the prior
# consent of Nexon Korea Corporation.

"""
FunAPI GivingTree client.
"""

import account_messages_pb2 as account_pb
import app_messages_for_client_pb2 as app_pb
import gflags
import logging
import os
import re
import sys

from twisted.internet import reactor
from twisted.internet import stdio
from twisted.internet import endpoints
from twisted.protocols import basic

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
    # pylint: disable-msg=E1101
    reactor.crash()

  # pylint: disable-msg=C0103
  def lineReceived(self, line):
    """LineReceiver 로부터."""
    request = account_pb.ClientAccountMessage()

    cmds = WHITE_SPACE.split(line.strip())
    cmd = cmds[0]

    if (cmd == "login"):
      if (len(cmds) != 3):
        print >> sys.stdout, 'error: len(cmds) != 3: %d' % len(cmds)
        print >> sys.stdout, 'example> login id password'
        return

      request.type = account_pb.ClientAccountMessage.kAccountLoginRequest
      request.login.account_id = cmds[1]
      request.login.auth_key = cmds[2]

    elif (cmd == "logout"):
      if (len(cmds) != 1):
        print >> sys.stdout, 'error: len(cmds) != 1: %d' % len(cmds)
        print >> sys.stdout, 'example> logout'
        return
      request.type = account_pb.ClientAccountMessage.kAccountLogoutRequest

    else:
      request.type = account_pb.ClientAccountMessage.kClientAppMessage
      if (cmd == ""):
        request.app_message.Extensions[app_pb.client_message_type] = \
            app_pb.ClientAppMessageType.kPlayerTakeApple
      else:
        request.app_message.Extensions[app_pb.client_message_type] = \
            app_pb.ClientAppMessageType.kPlayerRegisterName
        request.app_message.Extensions[app_pb.player_register_name].name = cmd

    self.protocol.write(request.SerializeToString())

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
    server_msg = account_pb.ServerAccountMessage()
    server_msg.ParseFromString(message)
    print >> sys.stdout, "Message received: " + str(server_msg)


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
