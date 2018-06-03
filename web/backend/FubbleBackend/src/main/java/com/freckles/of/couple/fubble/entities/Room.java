
package com.freckles.of.couple.fubble.entities;

import java.util.ArrayList;
import java.util.List;

public class Room {

    private String     name;
    private List<User> users            = new ArrayList<>();
    private int        connectedCounter = 1;

    public Room(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public List<User> getUsers() {
        return users;
    }

    public void setUsers(List<User> users) {
        this.users = users;
    }

    public int getConnectedCounter() {
        return connectedCounter;
    }

    public void setConnectedCounter(int connectedCounter) {
        this.connectedCounter = connectedCounter;
    }

}
