-- :name get_team_submission :one
SELECT *
FROM team_submissions
WHERE id = :submission_id;

-- :name get_ref_submission :one
SELECT *
FROM ref_submissions
WHERE id = :submission_id;

-- :name get_team_submissions :many
WITH results AS (SELECT submission_id,
                        round((count(result) FILTER (WHERE result = 'WINNER 1')) * 1.0 / count(result) * 100) AS result
                 FROM ref_results
                 GROUP BY submission_id
)
SELECT s.id,
       to_char(s.submission_time, 'DD/MM/YYYY HH24:MI:SS')               AS date,
       e.name                                                            AS env,
       s.status,
       s.is_primary                                                      AS "primary",
       (SELECT result FROM results WHERE submission_id = s.id) AS score
FROM team_submissions s
         JOIN environments e ON s.environment_id = e.id
WHERE s.team_id = :team_id;

-- :name insert_team_submission :scalar
INSERT INTO team_submissions (is_automake,
                              environment_id,
                              team_id)
VALUES (:is_automake, :environment_id, :team_id)
RETURNING id;

-- :name set_primary_submission
UPDATE team_submissions
SET is_primary = TRUE
WHERE id = :submission_id;

-- :name get_game_ref_submissions :many
SELECT id, submission_time, name, status, environment_id
FROM ref_submissions
WHERE game_id = :game_id;

-- :name insert_game_ref_submission :scalar
INSERT INTO ref_submissions (name,
                             environment_id,
                             teacher_id,
                             game_id)
VALUES (:name, :environment_id, :teacher_id, :game_id)
RETURNING id;

-- :name update_ref_submission
UPDATE ref_submissions
SET name            = coalesce(:new_name, name),
    environment_id  = coalesce(:new_environment_id, environment_id),
    submission_time = default
WHERE id = :submission_id;

-- :name update_ref_submission_path
UPDATE ref_submissions
SET files_path = :files_path
WHERE id = :submission_id;

-- :name update_team_submission_path
UPDATE team_submissions
SET files_path = :files_path
WHERE id = :submission_id;

-- :name update_team_submission_status
UPDATE team_submissions
SET status = :status
WHERE id = :submission_id;

-- :name remove_team_submission
DELETE
FROM team_submissions
WHERE id = :submission_id;

-- :name remove_ref_submission
DELETE
FROM ref_submissions
WHERE id = :submission_id;

-- :name remove_game_ref_submissions
DELETE
FROM ref_submissions
WHERE game_id = :game_id;

-- :name remove_all_team_submissions
DELETE
FROM team_submissions;