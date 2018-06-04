
package com.freckles.of.couple.fubble.handler;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;

public interface FubbleMessageHandler {

    public void handleMessage(MessageContainer container, FubbleEndpoint connection);

}
