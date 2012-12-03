#!/usr/bin/env python
# coding=utf8
# Copyright (C) 2012 Nexon Korea Corporation All Rights Reserved.
#
# This work is confidential and proprietary to Nexon Korea Corporation and
# must not be used, disclosed, copied, or distributed without the prior
# consent of Nexon Korea Corporation.
"""
FunAPI server 를 handling 하기 위한 module
"""

import logging
import struct
import uuid

from twisted.internet import protocol


LOGGER = logging.getLogger('funapi_server_stub')
LOGGER.setLevel(logging.DEBUG)


# pylint: disable-msg=R0923
class CallbackInterface:
  """
  funapi server stub 에서 발생하는 event 에 대한 callback interface.
  """
  def on_connection_made(self, proto):
    """
    새로운 연결이 시작되었을 때 호출.
    @param protocol HandlerProtocol instance.
    """
    pass

  def on_connection_lost(self, proto, reason):
    """
    연결이 끊어졌을 때 호출.
    @param protocol HandlerProtocol instance.
    @param reason 연결이 끊어진 이유.
    """
    pass

  def on_data_received(self, proto, data):
    """
    protocol 로부터 raw data 가 도착했을 때 호출.
    @param protocol HandlerProtocol instance.
    @param data raw data.
    """
    pass

  def on_session_id_changed(self, proto, session_id):
    """
    session id 가 바뀌었을 때 호출.
    @param protocol HandlerProtocol instance.
    @param session_id string.
    """
    pass

  def on_message_received(self, proto, message):
    """
    protocol 로부터 decoding 된 message 가 있을 때 호출.
    @param protocol HandlerProtocol instance.
    @param message decoding 된 message.
    """
    pass


class HandlerProtocol(protocol.Protocol):
  """
  funapi message 형태를 인식하는 protocol.
  """
  def __init__(self, callback_iface):
    """
    ctor
    @param callback_iface CallbackInterface 의 instance
    """
    self.callback_iface = callback_iface

  def write(self, message):
    """
    주어진 user-level message 를 protocol 의 transport 로 전송.
    """
    pass


class _HandlerProtocolImpl(HandlerProtocol):
  """
  FunAPI server stub handler implementation.
  """
  def __init__(self, callback_iface):
    """
    constructor
    @param callback_iface CallbackInterface 의 instance
    """
    HandlerProtocol.__init__(self, callback_iface)
    self.buffered = ''
    self.session_id = uuid.UUID(int=0)

  # pylint: disable-msg=C0103
  def connectionMade(self):
    """from protocol.Protocol"""
    LOGGER.debug('(Connected)')
    self.callback_iface.on_connection_made(self)

  # pylint: disable-msg=C0103,W0222
  def connectionLost(self, reason):
    """from protocol.Protocol"""
    LOGGER.debug('(Connection lost: %s)' % str(reason))
    self.callback_iface.on_connection_lost(self, reason)

  # pylint: disable-msg=C0103
  def dataReceived(self, data):
    """from protocol.Protocol"""
    # data 를 저장하고 message 단위로 처리해본다.
    self.buffered += data
    self.callback_iface.on_data_received(self, data)
    self.dispatch_message()

  def dispatch_message(self):
    """buffered 된 data 에서 message 를 decoding 함."""
    # Length 를 decoding 할
    while len(self.buffered) > 4:
      offset = 0
      payload_len = struct.unpack('!i', self.buffered[offset:offset + 4])[0]
      offset += 4
      if offset + payload_len > len(self.buffered):
        # Payload 를 완벽하게 decoding 할 수 없으므로 나중에 다시 시도함.
        return

      # session id 를 decoding 한다.
      uuid_length = len(self.session_id.bytes)
      session_id = uuid.UUID(bytes=self.buffered[offset:offset + uuid_length])
      offset += uuid_length
      if (session_id != self.session_id):
        LOGGER.debug('(Session ID changed: %s)' % str(session_id))
        self.session_id = session_id
        self.callback_iface.on_session_id_changed(self, session_id)

      # message 를 decoding 한다
      message_len = payload_len - len(session_id.bytes)
      message = self.buffered[offset:offset + message_len]
      offset += message_len
      self.callback_iface.on_message_received(self, message)

      # buffer 에서 처리된 부분을 버린다.
      self.buffered = self.buffered[offset:]

  def write(self, data):
    """Handler protocol 에서"""
    payload_length = len(data) + len(self.session_id.bytes)
    packet = struct.pack('!i', payload_length) + self.session_id.bytes + data
    self.transport.write(packet)


class HandlerFactory(protocol.Factory):
  """Handler factory"""
  def __init__(self, callback_iface):
    """
    FunAPI server message 를 처리하는 Handler 를 생성하는 Factory.
    @param callback_iface CallbackInterface 의 instance.
    """
    self.callback_iface = callback_iface

  # pylint: disable-msg=C0103,R0201
  def startedConnecting(self, connector):
    """Twisted 의 Factory 에서 override"""
    LOGGER.debug('(Connecting to %s)' % str(connector))

  # pylint: disable-msg=C0103,W0613
  def buildProtocol(self, addr):
    """Twisted 의 Factory 에서 override"""
    return _HandlerProtocolImpl(self.callback_iface)

  # pylint: disable-msg=C0103,W0613,R0201
  def clientConnectionLost(self, connector, reason):
    """Twisted 의 Factory 에서 override"""
    LOGGER.debug('(Connection closed for %s)' % str(reason))

  # pylint: disable-msg=C0103,W0613
  def clientConnectionFailed(self, connector, reason):
    """Twisted 의 Factory 에서 override"""
    LOGGER.debug('(Connection failed for %s)' % str(reason))


# 오직 HandlerFactory 만을 export 한다.
__all__ = [CallbackInterface, HandlerProtocol, HandlerFactory]
