- api/join_room/<room-name>
* success first, HTTP 201
{
  "role": "owner",
  "name": "change me"
}
* success not first, HTTP 200
{
  "role": "guest",
  "name": "change me"
}
* error, HTTP 500
{
  "description": "error description"
}

- api/disconnect/<user>/<room-name>
* success, HTTP 200
{
}

* error, HTTP 500
{
  "description": "error description"
}

- api/rename/<user>/<room-name>
* REQUEST
{
  "name": "new name"
}
* success, HTTP 200
{
}

* error, HTTP 500
{
  "description": "error description"
}

