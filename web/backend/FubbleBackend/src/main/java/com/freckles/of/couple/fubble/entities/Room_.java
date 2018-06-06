
package com.freckles.of.couple.fubble.entities;

import javax.persistence.metamodel.SingularAttribute;
import javax.persistence.metamodel.StaticMetamodel;

@StaticMetamodel(Room.class)
public abstract class Room_ {

    public static volatile SingularAttribute<Room, String>  id;
    public static volatile SingularAttribute<Room, String>  name;
    public static volatile SingularAttribute<Room, Integer> connectedCounter;

}