package com.freckles.of.couple.fubble.wsResponse;

import android.content.Context;
import android.util.Log;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.RenamedUser;

class RenamedRoomHandler implements MessageHandler {
    @Override
    public void handleMessage(Context context, MessageContainerClient container) {
        RenamedUser renamedUser = container.getRenamedUser();
        Log.i(MessageSwitch.class.getName(), renamedUser.toString());
    }
}
