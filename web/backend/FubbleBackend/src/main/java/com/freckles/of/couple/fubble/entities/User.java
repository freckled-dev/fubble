
package com.freckles.of.couple.fubble.entities;

import javax.websocket.Session;

public class User {

    private String  name;
    private Session session;

    public User() {}

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Session getSession() {
        return session;
    }

    public void setSession(Session session) {
        this.session = session;
    }

}
