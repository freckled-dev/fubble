
package com.freckles.of.couple.fubble.handler;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public interface FubbleMessageHandler {

    public void handleMessage(MessageContainerServer container, FubbleEndpoint connection);

}
