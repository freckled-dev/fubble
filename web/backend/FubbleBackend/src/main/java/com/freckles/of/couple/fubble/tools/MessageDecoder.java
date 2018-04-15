
package com.freckles.of.couple.fubble.tools;

import javax.websocket.DecodeException;
import javax.websocket.Decoder;
import javax.websocket.EndpointConfig;

import com.freckles.of.couple.fubble.entities.MessageContainer;
import com.google.gson.Gson;

public class MessageDecoder implements Decoder.Text<MessageContainer> {

    @Override
    public void destroy() {

    }

    @Override
    public void init(EndpointConfig arg0) {

    }

    @Override
    public MessageContainer decode(String text)
        throws DecodeException {
        MessageContainer message = null;

        Gson gson = new Gson();
        try {
            message = gson.fromJson(text, MessageContainer.class);
        } catch (Exception ex) {
            ex.printStackTrace();
        }

        return message;
    }

    @Override
    public boolean willDecode(String text) {
        return (text != null);
    }

}
