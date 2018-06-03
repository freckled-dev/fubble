
package com.freckles.of.couple.fubble.tools;

import java.io.IOException;
import java.util.List;

import javax.websocket.EncodeException;

import com.freckles.of.couple.fubble.entities.User;

public class BroadcastHandler {

    public void broadcast(List<User> users, String message) {
        users.forEach(user -> {
            try {
                user.getSession().getBasicRemote().sendObject(message);
            } catch (IOException | EncodeException e) {
                e.printStackTrace();
            }
        });
    }

}
