
package com.freckles.of.couple.fubble.entities;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Room {

    private String     id;
    private String     name;
    private List<User> users            = new ArrayList<>();
    private int        connectedCounter = 1;
    private Lock       mutex            = new ReentrantLock();

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

    public void setId(String id) {
        this.id = id;
    }

    public String getId() {
        return id;
    }

    public Lock getMutex() {
        return mutex;
    }

}
