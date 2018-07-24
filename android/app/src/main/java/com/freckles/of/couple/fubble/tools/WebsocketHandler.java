package com.freckles.of.couple.fubble.tools;

import android.content.Context;
import android.util.Log;

import com.freckles.of.couple.fubble.proto.WebContainer;

import org.java_websocket.client.WebSocketClient;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;

public class WebsocketHandler {

    public static final String FUBBLE_BACKEND_URL = "ws://moony.tips:8080/FubbleBackend/";
    private static WebSocketClient client;
    private Context context;

    public WebsocketHandler(Context context) {
        this.context = context;
    }

    public void open() {
        try {
            client = new ClientEndpoint(context, new URI(FUBBLE_BACKEND_URL));
            client.connect();
        } catch (URISyntaxException e) {
            Log.e(WebsocketHandler.class.getName(), e.toString());
        }
    }


    public void sendMessage(WebContainer.MessageContainerServer message) {
        try {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            message.writeTo(output);

            client.send(output.toByteArray());

            output.close();
        } catch (IOException ex) {
            Log.e(WebsocketHandler.class.getName(), ex.getLocalizedMessage());
        }
    }

}
