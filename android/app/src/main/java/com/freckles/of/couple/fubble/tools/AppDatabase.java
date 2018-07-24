package com.freckles.of.couple.fubble.tools;

import android.arch.persistence.room.Database;
import android.arch.persistence.room.RoomDatabase;

import com.freckles.of.couple.fubble.dao.RoomDao;
import com.freckles.of.couple.fubble.entity.FubbleRoom;

@Database(entities = {FubbleRoom.class}, version = 1)
public abstract class AppDatabase extends RoomDatabase {
    public abstract RoomDao roomDao();
}
