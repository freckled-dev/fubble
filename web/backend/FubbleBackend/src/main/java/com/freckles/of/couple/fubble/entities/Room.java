
package com.freckles.of.couple.fubble.entities;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.Transient;

@Entity
public class Room {

    @Id
    @Column(name = "id")
    private String     id;

    @Column(name = "NAME")
    private String     name;

    @Column(name = "CONNECTEDCOUNTER")
    private int        connectedCounter = 1;

    @ManyToMany(fetch = FetchType.LAZY, cascade = CascadeType.ALL)
    @JoinTable(name = "ROOM_USER", //
        joinColumns = { @JoinColumn(name = "ROOM_ID", nullable = false, updatable = false) }, //
        inverseJoinColumns = { @JoinColumn(name = "USER_ID", nullable = false, updatable = false) })
    private List<User> users            = new ArrayList<>();

    @Transient
    private Lock       mutex            = new ReentrantLock();

    public Room() {

    }

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
