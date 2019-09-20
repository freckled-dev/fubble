package com.freckles.of.couple.fubble.wsResponse;

import android.content.Context;
import android.util.Log;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.UserLeft;

class UserLeftHandler implements MessageHandler {
    @Override
    public void handleMessage(Context context, MessageContainerClient container) {
        UserLeft userLeft = container.getUserLeft();
        Log.i(MessageSwitch.class.getName(), userLeft.toString());
    }
}
