package com.freckles.of.couple.fubble.dao;

import android.arch.persistence.room.Dao;
import android.arch.persistence.room.Delete;
import android.arch.persistence.room.Insert;
import android.arch.persistence.room.Query;
import android.arch.persistence.room.Update;

import com.freckles.of.couple.fubble.entity.FubbleRoom;

import java.util.List;

@Dao
public interface RoomDao {

    @Query("SELECT * FROM fubbleRoom")
    List<FubbleRoom> getAll();

    @Query("SELECT * FROM fubbleRoom WHERE name LIKE :name")
    FubbleRoom findByName(String name);

    @Update
    void updateRoom(FubbleRoom room);

    @Insert
    void insertAll(FubbleRoom... rooms);

    @Insert
    void insert(FubbleRoom room);

    @Delete
    void delete(FubbleRoom room);
}
