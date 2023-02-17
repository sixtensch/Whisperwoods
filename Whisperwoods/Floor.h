#pragma once
#include "Room.h"

struct Floor
{
	bool active;
	Vec3 startPos;
	int startRoom;
	cs::List<shared_ptr<Room>> rooms;
	//cs::List<RoomConnector> roomConnections;
};