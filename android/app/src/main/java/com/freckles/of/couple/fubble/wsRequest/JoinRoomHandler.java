package com.freckles.of.couple.fubble.wsRequest;

import android.app.Activity;
import android.support.design.widget.Snackbar;

import com.freckles.of.couple.fubble.R;
import com.freckles.of.couple.fubble.proto.WebContainer;
import com.freckles.of.couple.fubble.tools.WebsocketHandler;

public class JoinRoomHandler extends RequestHandler {

    private final Activity activity;
    private WebsocketHandler websocket;

    public JoinRoomHandler(Activity activity) {
        this.activity = activity;
        websocket = new WebsocketHandler(activity);
    }

    public void joinRoom(String roomName, String password) {
        final WebContainer.JoinRoom joinRoom = WebContainer.JoinRoom.newBuilder()
                .setRoomName(roomName)
                .setPassword(password)
                .build();

        try {
            WebContainer.MessageContainerServer container = WebContainer.MessageContainerServer.newBuilder().setJoinRoom(joinRoom).build();
            websocket.sendMessage(container);
        } catch (Exception ex) {
            handleError(activity, ex);
        }
    }

}
