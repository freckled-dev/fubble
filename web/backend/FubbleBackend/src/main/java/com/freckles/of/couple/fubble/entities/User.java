
package com.freckles.of.couple.fubble.entities;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Transient;
import javax.websocket.Session;

@Entity(name = "FUBBLEUSER")
public class User {

    @Id
    @Column(name = "id")
    private String  id;

    @Column(name = "NAME")
    private String  name;

    @Transient
    private Session session;

    public User() {

    }

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

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

}
