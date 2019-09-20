package com.freckles.of.couple.fubble.entity;

import android.arch.persistence.room.ColumnInfo;
import android.arch.persistence.room.Entity;
import android.arch.persistence.room.PrimaryKey;
import android.arch.persistence.room.TypeConverters;
import android.support.annotation.NonNull;

import java.util.Date;

@Entity
public class FubbleRoom {

    @PrimaryKey(autoGenerate = true)
    private int id;

    @ColumnInfo(name = "roomId")
    private String roomId;

    @ColumnInfo(name = "name")
    @NonNull
    private String name;

    @ColumnInfo(name = "lastConnected")
    @TypeConverters({Converters.class})
    private Date lastConnected;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Date getLastConnected() {
        return lastConnected;
    }

    public void setLastConnected(Date lastConnected) {
        this.lastConnected = lastConnected;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getRoomId() {
        return roomId;
    }

    public void setRoomId(String roomId) {
        this.roomId = roomId;
    }


    @Override
    public String toString() {
        return "FubbleRoom{" +
                "id=" + id +
                ", roomId='" + roomId + '\'' +
                ", name='" + name + '\'' +
                ", lastConnected=" + lastConnected +
                '}';
    }
}
