-- :name get_games :many
SELECT id, name, is_active
FROM games;

-- :name get_game :one
SELECT *
FROM games
WHERE id = :game_id;

-- :name get_game_by_name :one
SELECT *
FROM games
WHERE name = :name;

-- :name get_active_game :one
SELECT id, name, description, to_char(deadline, 'YYYY-MM-DD') as deadline
FROM games
WHERE is_active = TRUE;

-- :name insert_game :scalar
INSERT INTO games (name,
                   description,
                   deadline,
                   environment_id)
VALUES (:name, :description, :deadline, :environment_id)
RETURNING id;

-- :name update_game_path
UPDATE games
SET files_path = :files_path
WHERE id = :game_id;

-- :name update_game
UPDATE games
SET name           = coalesce(:new_name, name),
    description       = coalesce(:new_description, description),
    deadline       = coalesce(:new_deadline, deadline),
    environment_id = coalesce(:new_environment_id, environment_id)
WHERE id = :game_id;

-- :name activate_game
UPDATE games
SET is_active = TRUE
WHERE id = :game_id;

-- :name deactivate_games
UPDATE games
SET is_active = FALSE
WHERE is_active = TRUE;

-- :name remove_game
DELETE
FROM games
WHERE id = :game_id;