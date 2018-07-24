package com.freckles.of.couple.fubble.clientHandler;

import android.content.Context;
import android.util.Log;

import com.freckles.of.couple.fubble.proto.WebContainer.FubbleError;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;

public class ErrorHandler implements MessageHandler {

    @Override
    public void handleMessage(Context context, MessageContainerClient container) {
        FubbleError error = container.getError();
        Log.i(MessageSwitch.class.getName(), error.toString());
    }
}
