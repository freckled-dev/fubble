
package com.freckles.of.couple.fubble.entities;

import java.util.Date;

public class Chat {

    private Date   timestamp;
    private String comment;
    private String userName;

    public Date getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(Date timestamp) {
        this.timestamp = timestamp;
    }

    public String getComment() {
        return comment;
    }

    public void setComment(String comment) {
        this.comment = comment;
    }

    public String getUserName() {
        return userName;
    }

    public void setUserName(String userName) {
        this.userName = userName;
    }

    @Override
    public String toString() {
        return "Chat [timestamp=" + timestamp + ", comment=" + comment + "]";
    }

}
