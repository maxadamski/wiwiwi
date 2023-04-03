DROP TABLE IF EXISTS teachers CASCADE;
DROP TABLE IF EXISTS groups CASCADE;
DROP TABLE IF EXISTS students CASCADE;
DROP TABLE IF EXISTS teams CASCADE;
DROP TABLE IF EXISTS team_members CASCADE;
DROP TABLE IF EXISTS team_invitations CASCADE;
DROP TABLE IF EXISTS environments CASCADE;
DROP TABLE IF EXISTS games CASCADE;
DROP TABLE IF EXISTS team_submissions CASCADE;
DROP TABLE IF EXISTS ref_submissions CASCADE;
DROP TABLE IF EXISTS tournament_results CASCADE;
DROP TABLE IF EXISTS ref_results CASCADE;

DROP TYPE IF EXISTS STATUS;

CREATE TABLE teachers
(
    id       SERIAL PRIMARY KEY,
    login    VARCHAR(50) NOT NULL UNIQUE,
    password CHAR(226)   NOT NULL
);

CREATE TABLE groups
(
    id   SERIAL PRIMARY KEY,
    name VARCHAR(20) NOT NULL UNIQUE
);

CREATE TABLE students
(
    id       SERIAL PRIMARY KEY,
    login    VARCHAR(50) NOT NULL UNIQUE,
    password CHAR(226)   NOT NULL,
    nickname VARCHAR(50) NOT NULL UNIQUE,
    group_id INTEGER     REFERENCES groups (id) ON DELETE SET NULL
);

CREATE TABLE teams
(
    id        SERIAL PRIMARY KEY,
    name      VARCHAR(50) NOT NULL UNIQUE,
    leader_id INTEGER     REFERENCES students (id) ON DELETE SET NULL
);

CREATE TABLE team_members
(
    team_id    INTEGER NOT NULL REFERENCES teams (id) ON DELETE CASCADE,
    student_id INTEGER NOT NULL REFERENCES students (id) ON DELETE CASCADE,
    UNIQUE (team_id, student_id)
);

CREATE TABLE team_invitations
(
    team_id    INTEGER NOT NULL REFERENCES teams (id) ON DELETE CASCADE,
    student_id INTEGER NOT NULL REFERENCES students (id) ON DELETE CASCADE,
    UNIQUE (team_id, student_id)
);

CREATE TABLE environments
(
    id   SERIAL PRIMARY KEY,
    name VARCHAR(50) NOT NULL UNIQUE
);

CREATE TABLE games
(
    id             SERIAL PRIMARY KEY,
    name           VARCHAR(50) NOT NULL UNIQUE,
    description    VARCHAR(100),
    files_path     VARCHAR(1024) UNIQUE,
    deadline       TIMESTAMP   NOT NULL,
    is_active      BOOLEAN     NOT NULL DEFAULT FALSE,
    environment_id INTEGER     NOT NULL REFERENCES environments (id) ON DELETE CASCADE
);

CREATE TABLE team_submissions
(
    id              SERIAL PRIMARY KEY,
    submission_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    is_automake     BOOLEAN   NOT NULL,
    files_path      VARCHAR(1024) UNIQUE,
    status          VARCHAR(20),
    is_primary      BOOLEAN   NOT NULL DEFAULT FALSE,
    environment_id  INTEGER   NOT NULL REFERENCES environments (id) ON DELETE CASCADE,
    team_id         INTEGER REFERENCES teams (id) ON DELETE CASCADE
);

CREATE TABLE ref_submissions
(
    id              SERIAL PRIMARY KEY,
    submission_time TIMESTAMP   NOT NULL DEFAULT CURRENT_TIMESTAMP,
    name            VARCHAR(50) NOT NULL,
    files_path      VARCHAR(1024) UNIQUE,
    status          VARCHAR(20),
    environment_id  INTEGER     NOT NULL REFERENCES environments (id) ON DELETE CASCADE,
    teacher_id      INTEGER     REFERENCES teachers (id) ON DELETE SET NULL,
    game_id         INTEGER REFERENCES games (id) ON DELETE CASCADE
);

CREATE TABLE tournament_results
(
    id                   SERIAL PRIMARY KEY,
    result               INTEGER   NOT NULL,
    sub1_stdout         VARCHAR(200) DEFAULT 'UNDEFINED',
    sub2_stdout         VARCHAR(200) DEFAULT 'UNDEFINED',
    execution_time       TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    first_submission_id  INTEGER   REFERENCES team_submissions (id) ON DELETE SET NULL,
    second_submission_id INTEGER   REFERENCES team_submissions (id) ON DELETE SET NULL
);

CREATE TABLE ref_results
(
    id             SERIAL PRIMARY KEY,
    result         VARCHAR(50) DEFAULT 'UNDEFINED',
    sub_stdout         VARCHAR(200) DEFAULT 'UNDEFINED',
    ref_stdout         VARCHAR(200) DEFAULT 'UNDEFINED',
    execution_time TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    submission_id  INTEGER   REFERENCES team_submissions (id) ON DELETE SET NULL,
    reference_id   INTEGER   REFERENCES ref_submissions (id) ON DELETE SET NULL
);

DROP PROCEDURE IF EXISTS create_student_team(student_id INT, student_nickname VARCHAR);

CREATE OR REPLACE PROCEDURE create_student_team(student_id INT, student_nickname VARCHAR)
AS
$BODY$
DECLARE
    new_team_id student_id%TYPE;
    team_idx    INTEGER := 1;
BEGIN
    WHILE ((SELECT id FROM teams WHERE name LIKE student_nickname::TEXT || ' team ' || team_idx::TEXT) IS NOT NULL)
        LOOP
            team_idx := team_idx + 1;
        END LOOP;
    INSERT INTO teams(name, leader_id)
    VALUES (student_nickname::TEXT || ' team ' || team_idx::TEXT, student_id)
    RETURNING id INTO new_team_id;

    INSERT INTO team_members(team_id, student_id)
    VALUES (new_team_id, student_id);
END;
$BODY$
    LANGUAGE plpgsql;

DROP FUNCTION IF EXISTS create_initial_team;
CREATE FUNCTION create_initial_team() RETURNS TRIGGER AS
$BODY$
BEGIN
    CALL create_student_team(new.id, new.login);
    RETURN NULL;
END;
$BODY$
    LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trig_student_added ON students;

CREATE TRIGGER trig_student_added
    AFTER INSERT
    ON students
    FOR EACH ROW
EXECUTE PROCEDURE create_initial_team();

DROP FUNCTION IF EXISTS manage_team_leaders;
CREATE FUNCTION manage_team_leaders() RETURNS TRIGGER AS
$BODY$
DECLARE
    new_leader_id         students.id%TYPE;
    student_team_nickname students.nickname%TYPE;
BEGIN
    IF (SELECT id FROM teams WHERE leader_id = old.student_id) IS NOT NULL THEN
        SELECT student_id
        FROM team_members
        WHERE team_id = old.team_id FETCH FIRST ROW ONLY
        INTO new_leader_id;

        IF new_leader_id IS NULL THEN
            DELETE
            FROM teams
            WHERE id = old.team_id;
        ELSE
            UPDATE teams
            SET leader_id=new_leader_id
            WHERE id = old.team_id;
        END IF;
    END IF;
    IF (tg_op = 'DELETE') THEN
        SELECT nickname FROM students WHERE id = old.student_id INTO student_team_nickname;
        IF student_team_nickname IS NOT NULL THEN
            CALL create_student_team(old.student_id, student_team_nickname);
        END IF;
    END IF;
    RETURN NULL;
END;
$BODY$
    LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trig_team_changed ON team_members;

CREATE TRIGGER trig_team_changed
    AFTER UPDATE
        OF team_id OR DELETE
    ON team_members
    FOR EACH ROW
EXECUTE PROCEDURE manage_team_leaders();

DROP FUNCTION IF EXISTS set_primary_submission;
CREATE FUNCTION set_primary_submission() RETURNS TRIGGER AS
$BODY$
DECLARE
BEGIN
    IF (SELECT id FROM team_submissions WHERE team_id = new.team_id AND is_primary = TRUE) IS NULL THEN
        UPDATE team_submissions
        SET is_primary= TRUE
        WHERE id = new.id;
    END IF;
    RETURN NULL;
END;
$BODY$
    LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trig_submission_created ON team_submissions;

CREATE TRIGGER trig_submission_created
    AFTER INSERT
    ON team_submissions
    FOR EACH ROW
EXECUTE PROCEDURE set_primary_submission();
