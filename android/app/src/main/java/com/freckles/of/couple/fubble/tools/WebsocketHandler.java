package com.freckles.of.couple.fubble.tools;

import android.content.Context;

import com.freckles.of.couple.fubble.proto.WebContainer;

import org.java_websocket.client.WebSocketClient;

import java.io.ByteArrayOutputStream;
import java.net.URI;

public class WebsocketHandler {

    public static final String FUBBLE_BACKEND_URL = "ws://moony.tips:8080/FubbleBackend/";
    private static WebSocketClient client;
    private Context context;

    public WebsocketHandler(Context context) {
        this.context = context;
    }

    public void open() throws Exception {
        client = new ClientEndpoint(context, new URI(FUBBLE_BACKEND_URL));
        client.connect();
    }


    public void sendMessage(WebContainer.MessageContainerServer message) throws Exception {
        ByteArrayOutputStream output = new ByteArrayOutputStream();
        message.writeTo(output);

        client.send(output.toByteArray());

        output.close();
    }

}
