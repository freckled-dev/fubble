
package com.freckles.of.couple.fubble.handler;

import javax.websocket.Session;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameRoom;

public class RenameRoomHandler implements FubbleMessageHandler {

    @Override
    public void handleMessage(MessageContainer container, Session sessio) {
        RenameRoom renameRoom = container.getRenameRoom();
    }

}
