
package com.freckles.of.couple.fubble.handler;

import javax.websocket.Session;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameUser;

public class RenameUserHandler implements FubbleMessageHandler {

    @Override
    public void handleMessage(MessageContainer container, Session sessio) {
        RenameUser renameUser = container.getRenameUser();
    }

}
