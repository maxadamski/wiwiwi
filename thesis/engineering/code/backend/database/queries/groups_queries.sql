-- :name get_groups :many
SELECT *
FROM groups;

-- :name get_group_by_name :one
SELECT *
FROM groups
WHERE name = :name;

-- :name insert_group :scalar
INSERT INTO groups (name)
VALUES (:name)
RETURNING id;

-- :name update_group
UPDATE groups
SET name = :name
WHERE id = :group_id;

-- :name remove_group
DELETE
FROM groups
WHERE id = :group_id;

-- :name remove_all_groups
DELETE
FROM groups;