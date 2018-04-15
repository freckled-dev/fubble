
package com.freckles.of.couple.fubble.entities;

import com.google.gson.JsonElement;

public class MessageContainer {

    public enum MessageType {
        CHAT, RENAME_USER;
    }

    private MessageType type;
    private JsonElement message;

    public MessageContainer() {}

    public MessageType getType() {
        return type;
    }

    public void setType(MessageType type) {
        this.type = type;
    }

    public JsonElement getMessage() {
        return message;
    }

    public void setMessage(JsonElement message) {
        this.message = message;
    }

    @Override
    public String toString() {
        return "Message [type=" + type + ", message=" + message + "]";
    }

}