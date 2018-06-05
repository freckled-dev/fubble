
package com.freckles.of.couple.fubble.handler;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;
import com.freckles.of.couple.fubble.proto.WebContainer.RenameRoom;

public class RenameRoomHandler implements FubbleMessageHandler {

    @Override
    public void handleMessage(MessageContainerServer container, FubbleEndpoint connection) {
        RenameRoom renameRoom = container.getRenameRoom();
    }

}
