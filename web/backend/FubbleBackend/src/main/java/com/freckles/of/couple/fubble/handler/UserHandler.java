
package com.freckles.of.couple.fubble.handler;

import java.util.UUID;

import com.freckles.of.couple.fubble.FubbleEndpoint;
import com.freckles.of.couple.fubble.entities.User;
import com.freckles.of.couple.fubble.tools.FubbleProperties;

import io.netty.util.internal.StringUtil;

public class UserHandler {

    public User createUser(FubbleEndpoint connection, int counter, String userName) {
        User newUser = new User();
        newUser.setId(UUID.randomUUID().toString());
        if (StringUtil.isNullOrEmpty(userName)) {
            newUser.setName(FubbleProperties.getInstance().getUserNamePrefix() + counter);
        } else {
            newUser.setName(userName);
        }
        newUser.setSession(connection.getSession());
        connection.setUser(newUser);

        return newUser;
    }

}
