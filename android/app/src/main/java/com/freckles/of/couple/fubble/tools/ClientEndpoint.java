package com.freckles.of.couple.fubble.tools;


import android.content.Context;
import android.util.Log;

import com.freckles.of.couple.fubble.wsResponse.MessageSwitch;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.google.protobuf.ByteString;
import com.google.protobuf.InvalidProtocolBufferException;

import org.java_websocket.client.WebSocketClient;
import org.java_websocket.handshake.ServerHandshake;

import java.net.URI;
import java.nio.ByteBuffer;

import static com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.parseFrom;

public class ClientEndpoint extends WebSocketClient {

    private MessageSwitch handler = new MessageSwitch();
    private Context context;

    public ClientEndpoint(Context context, URI serverURI) {
        super(serverURI);
        this.context = context;
    }


    @Override
    public void onOpen(ServerHandshake data) {
        Log.i(ClientEndpoint.class.getName(), "onOpen()");
    }

    @Override
    public void onMessage(String message) {

    }

    @Override
    public void onMessage(ByteBuffer message) {
        ByteString messageBytes = ByteString.copyFrom(message);
        MessageContainerClient container;
        try {
            container = parseFrom(messageBytes);
            handler.handleContainer(context, container);
        } catch (InvalidProtocolBufferException ex) {
            Log.e(ClientEndpoint.class.getName(), "error(): " + ex);
        }
    }

    @Override
    public void onClose(int code, String reason, boolean remote) {
        Log.i(ClientEndpoint.class.getName(), "reason(): " + reason);
    }

    @Override
    public void onError(Exception ex) {
        Log.i(ClientEndpoint.class.getName(), "ex(): " + ex);
    }
}
