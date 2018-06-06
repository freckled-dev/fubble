
package com.freckles.of.couple.fubble.handler;

import java.util.UUID;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.tools.FubbleProperties;

public class UserHandler {

    public User createUser(FubbleEndpoint connection, int counter) {
        User newUser = new User();
        newUser.setId(UUID.randomUUID().toString());
        newUser.setName(FubbleProperties.getInstance().getUserNamePrefix() + counter);
        newUser.setSession(connection.getSession());
        connection.setUser(newUser);

        return newUser;
    }

}
