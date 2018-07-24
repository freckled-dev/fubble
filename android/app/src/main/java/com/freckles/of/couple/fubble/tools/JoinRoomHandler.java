package com.freckles.of.couple.fubble.tools;

import android.app.Activity;
import android.support.design.widget.Snackbar;

import com.freckles.of.couple.fubble.R;
import com.freckles.of.couple.fubble.proto.WebContainer;

public class JoinRoomHandler {

    private final Activity activity;
    private WebsocketHandler websocket;

    public JoinRoomHandler(Activity activity) {
        this.activity = activity;
        websocket = new WebsocketHandler(activity);
    }

    public void joinRoom(String roomName, String password) {
        final WebContainer.JoinRoom joinRoom = WebContainer.JoinRoom.newBuilder() //
                .setRoomName(roomName)//
                .setPassword(password) //
                .build();

        try {
            WebContainer.MessageContainerServer container = WebContainer.MessageContainerServer.newBuilder().setJoinRoom(joinRoom).build();
            websocket.sendMessage(container);
        } catch (Exception ex) {
            String errorText = activity.getResources().getString(R.string.web_server_error);
            Snackbar errorBar = Snackbar.make(activity.findViewById(R.id.cl_main_content), errorText, Snackbar.LENGTH_LONG);
            errorBar.show();
        }
    }

}
