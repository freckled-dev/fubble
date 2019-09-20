package com.freckles.of.couple.fubble.wsResponse;

import android.content.Context;

import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;

public interface MessageHandler {

    void handleMessage(Context context, MessageContainerClient container);

}
