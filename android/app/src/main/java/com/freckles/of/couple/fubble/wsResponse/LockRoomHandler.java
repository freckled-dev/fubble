package com.freckles.of.couple.fubble.wsResponse;

import android.content.Context;
import android.util.Log;

import com.freckles.of.couple.fubble.proto.WebContainer.LockedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;

class LockRoomHandler implements MessageHandler {
    @Override
    public void handleMessage(Context context, MessageContainerClient container) {
        LockedRoom roomLocked = container.getLockRoom();
        Log.i(MessageSwitch.class.getName(), roomLocked.toString());
    }
}
