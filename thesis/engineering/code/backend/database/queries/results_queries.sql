-- :name get_ref_results :many
SELECT *
FROM ref_results
WHERE submission_id = :submission_id;


-- :name insert_ref_result :scalar
INSERT INTO ref_results(submission_id, reference_id, result)
VALUES (:submission_id, :reference_id, :result)
RETURNING id;

-- :name update_ref_result
UPDATE ref_results
SET result     = :result,
    sub_stdout = :sub_stdout,
    ref_stdout = :ref_stdout
WHERE id = :result_id;

-- :name remove_ref_results
DELETE
FROM ref_results
WHERE submission_id = :submission_id;

-- :name remove_all_ref_results
DELETE
FROM ref_results;

-- :name remove_all_tournament_results
DELETE
FROM tournament_results;
