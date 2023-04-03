-- :name get_team :one
SELECT id, name, leader_id
FROM teams
WHERE id = :team_id;

-- :name get_team_by_name :one
SELECT id, name, leader_id
FROM teams
WHERE name = :name;

-- :name get_student_team :one
SELECT t.id, t.name, t.leader_id
FROM team_members tu
         INNER JOIN teams t ON tu.team_id = t.id
WHERE tu.student_id = :student_id;

-- :name update_team_name
UPDATE teams
SET name = coalesce(:new_name, name)
WHERE id = :team_id;

-- :name update_team_leader
UPDATE teams
SET leader_id = coalesce(:new_leader_id, leader_id)
WHERE id = :team_id;

-- :name change_team
UPDATE team_members
SET team_id = :team_id
WHERE student_id = :student_id;

-- :name get_team_member :one
SELECT team_id, student_id
FROM team_members
WHERE team_id = :team_id
  AND student_id = :student_id;

-- :name get_team_members :many
SELECT u.id, u.nickname
FROM team_members tu
         INNER JOIN students u ON tu.student_id = u.id
WHERE tu.team_id = :team_id;

-- :name remove_student_from_team
DELETE
FROM team_members
WHERE student_id = :student_id;

-- :name get_team_invitation :one
SELECT team_id, student_id
FROM team_invitations
WHERE team_id = :team_id
  AND student_id = :student_id;

-- :name get_team_invitations :many
SELECT s.id, s.nickname
FROM team_invitations ti
         INNER JOIN students s ON ti.student_id = s.id
WHERE ti.team_id = :team_id;

-- :name get_student_invitations :many
SELECT t.id, t.name, s.nickname AS leader
FROM team_invitations ti
         INNER JOIN teams t ON ti.team_id = t.id
         INNER JOIN students s ON s.id = t.leader_id
WHERE ti.student_id = :student_id;

-- :name invite_student_to_team :one
INSERT INTO team_invitations(student_id, team_id)
VALUES (:student_id, :team_id)
RETURNING (:student_id, :team_id);

-- :name remove_invite :one
DELETE
FROM team_invitations
WHERE student_id = :student_id
  AND team_id = :team_id
RETURNING team_id;

-- :name remove_all_teams
DELETE
FROM teams;