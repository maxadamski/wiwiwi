## Submissions

- Name
- Upload one file (source/archive)
- Container list
- Execution type (auto one file/makefile)

## Team Rules

1. Joining a game creates a team for this game with only you as a leader
2. You can invite a user to join your team
3. You can select a user in your team as a new leader
4. Accepting an invite results in leaving your team, and joining the other team as a non-leader
5. You may leave a team with other members, if you are not the leader
6. When a team has no members, it is deleted

## Schema

	Team = (game-id, name, leader-id)
	Submission = (team-id, ...)
	User-Game = (user-id, game-id, team)
	User = (id, name, team-id)

