
package com.freckles.of.couple.fubble.handler;

import javax.websocket.Session;

import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.tools.FubbleProperties;

public class UserHandler {

    public User createUser(Session session, int counter) {
        User newUser = new User();
        newUser.setId(session.getId());
        newUser.setName(FubbleProperties.getInstance().getUserNamePrefix() + counter);
        newUser.setSession(session);

        return newUser;
    }

}
