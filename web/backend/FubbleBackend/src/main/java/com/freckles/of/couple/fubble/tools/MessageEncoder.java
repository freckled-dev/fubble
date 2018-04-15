
package com.freckles.of.couple.fubble.tools;

import javax.websocket.EncodeException;
import javax.websocket.Encoder;
import javax.websocket.EndpointConfig;

import com.freckles.of.couple.fubble.entities.MessageContainer;
import com.google.gson.Gson;

public class MessageEncoder implements Encoder.Text<MessageContainer> {

    @Override
    public void destroy() {

    }

    @Override
    public void init(EndpointConfig arg0) {

    }

    @Override
    public String encode(MessageContainer message)
        throws EncodeException {
        Gson gson = new Gson();
        String json = gson.toJson(message);
        return json;
    }

}
