#!/usr/bin/bash

lxc-destroy -f player-1
lxc-destroy -f player-2
lxc-destroy -f player-3
lxc-destroy -f ref-1
lxc-destroy -f ref-2

curl -X PUT "http://localhost:8001/game/1" -F env_id=0 -F data=@judge.zip
echo
curl -X PUT "http://localhost:8001/ref_player/1" -F game_id=1 -F env_id=0 -F data=@player_rand.zip
echo
curl -X PUT "http://localhost:8001/ref_player/2" -F game_id=1 -F env_id=0 -F data=@player_py.zip
echo
curl -X PUT "http://localhost:8001/player/1" -F env_id=0 -F automake=false -F data=@player_rand.zip
echo
curl -X PUT "http://localhost:8001/player/2" -F env_id=0 -F automake=false -F data=@player_py.zip
echo
curl -X PUT "http://localhost:8001/player/3" -F env_id=0 -F automake=false -F data=@player_ab.zip
echo
curl -X PUT "http://localhost:8001/job/1001?game_id=1&p1_id=1&p2_id=1&is_ref=true" &
echo
curl -X PUT "http://localhost:8001/job/1002?game_id=1&p1_id=1&p2_id=1" &
echo
curl -X PUT "http://localhost:8001/job/1003?game_id=1&p1_id=1&p2_id=2" &
echo

