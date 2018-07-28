package com.freckles.of.couple.fubble.wsResponse;

import android.content.Context;
import android.util.Log;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.UserJoined;

class UserJoinedHandler implements MessageHandler {

    @Override
    public void handleMessage(Context context, MessageContainerClient container) {
        UserJoined userJoined = container.getUserJoined();
        Log.i(UserJoinedHandler.class.getName(), userJoined.toString());
    }

}
