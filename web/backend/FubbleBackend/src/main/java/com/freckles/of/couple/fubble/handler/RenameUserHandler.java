
package com.freckles.of.couple.fubble.handler;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameUser;

public class RenameUserHandler implements FubbleMessageHandler {

    @Override
    public void handleMessage(MessageContainer container, FubbleEndpoint connection) {
        RenameUser renameUser = container.getRenameUser();
    }

}
