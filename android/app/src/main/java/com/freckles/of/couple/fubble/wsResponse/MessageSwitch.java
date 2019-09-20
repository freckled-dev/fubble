package com.freckles.of.couple.fubble.wsResponse;

import android.content.Context;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;

public class MessageSwitch {

    public void handleContainer(Context context, MessageContainerClient container) {
        MessageTypeCase messageTypeCase = container.getMessageTypeCase();
        MessageHandler handler = null;

        if (MessageTypeCase.JOINED_ROOM.equals(messageTypeCase)) {
            handler = new JoinedRoomHandler();
        }

        if (MessageTypeCase.USER_JOINED.equals(messageTypeCase)) {
            handler = new UserJoinedHandler();
        }

        if (MessageTypeCase.RENAMED_USER.equals(messageTypeCase)) {
            handler = new RenamedRoomHandler();
        }

        if (MessageTypeCase.CHAT_MESSAGE.equals(messageTypeCase)) {
            handler = new ChatMessageHandler();
        }

        if (MessageTypeCase.USER_LEFT.equals(messageTypeCase)) {
            handler = new UserLeftHandler();
        }

        if (MessageTypeCase.LOCK_ROOM.equals(messageTypeCase)) {
            handler = new LockRoomHandler();
        }

        if (MessageTypeCase.ERROR.equals(messageTypeCase)) {
            handler = new ErrorHandler();
        }

        if (handler != null) {
            handler.handleMessage(context, container);
        }
    }
}
