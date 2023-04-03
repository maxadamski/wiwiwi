-- :name get_student :one
SELECT id, nickname, group_id
FROM students s
WHERE id = :student_id;

-- :name get_student_by_login :one
SELECT *
FROM students
WHERE login = :login;

-- :name get_student_by_nickname :one
SELECT *
FROM students
WHERE nickname = :nickname;

-- :name insert_student :scalar
INSERT INTO students (login, password, nickname, group_id)
VALUES (:login, :password, :nickname, :group_id)
RETURNING id;

-- :name update_student
UPDATE students
SET nickname = coalesce(:new_nickname, nickname),
    group_id = coalesce(:new_group_id, group_id)
WHERE id = :student_id;

-- :name remove_student
DELETE
FROM students
WHERE id = :student_id;

-- :name remove_all_students
DELETE
FROM students;