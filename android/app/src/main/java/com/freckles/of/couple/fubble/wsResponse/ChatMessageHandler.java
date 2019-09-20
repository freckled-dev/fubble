package com.freckles.of.couple.fubble.wsResponse;

import android.content.Context;
import android.util.Log;

import com.freckles.of.couple.fubble.proto.WebContainer.ChatMessageClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;

class ChatMessageHandler implements MessageHandler {

    @Override
    public void handleMessage(Context context, MessageContainerClient container) {
        ChatMessageClient chat = container.getChatMessage();
        Log.i(MessageSwitch.class.getName(), chat.toString());
    }
}
