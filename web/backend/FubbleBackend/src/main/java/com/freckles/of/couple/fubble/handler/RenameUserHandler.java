
package com.freckles.of.couple.fubble.handler;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameUser;

public class RenameUserHandler implements FubbleMessageHandler {

    @Override
    public void handleMessage(MessageContainerServer container, FubbleEndpoint connection) {
        RenameUser renameUser = container.getRenameUser();
    }

}
