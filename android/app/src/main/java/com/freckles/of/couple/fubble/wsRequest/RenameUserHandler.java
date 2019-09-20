package com.freckles.of.couple.fubble.wsRequest;

import android.app.Activity;

import com.freckles.of.couple.fubble.proto.WebContainer;
import com.freckles.of.couple.fubble.tools.WebsocketHandler;

public class RenameUserHandler extends RequestHandler {

    private final Activity activity;
    private WebsocketHandler websocket;

    public RenameUserHandler(Activity activity) {
        this.activity = activity;
        websocket = new WebsocketHandler(activity);
    }

    public void renameUser(String newName) {
        final WebContainer.RenameUser renameUser = WebContainer.RenameUser.newBuilder()
                .setNewName(newName)
                .build();

        try {
            WebContainer.MessageContainerServer container = WebContainer.MessageContainerServer.newBuilder().setRenameUser(renameUser).build();
            websocket.sendMessage(container);
        } catch (Exception ex) {
            handleError(activity, ex);
        }
    }
}
