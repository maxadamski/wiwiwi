-- :name get_teacher :one
SELECT *
FROM teachers
WHERE id = :teacher_id;

-- :name get_teacher_by_login :one
SELECT *
FROM teachers
WHERE login = :login;


-- :name update_teacher
UPDATE teachers
SET login    = coalesce(:new_login, login),
    password = coalesce(:new_password, password)
WHERE id = :teacher_id;

-- :name insert_teacher :scalar
INSERT INTO teachers (login, password)
VALUES (:login, :password)
RETURNING id;