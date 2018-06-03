
package com.freckles.of.couple.fubble.handler;

import javax.websocket.Session;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;

public interface FubbleMessageHandler {

    public void handleMessage(MessageContainer container, Session session);

}
