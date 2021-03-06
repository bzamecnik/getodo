-- GeToDo - initial SQL database structure

-- Drop tables --

--DROP TABLE Tagged;
--DROP TABLE Subtask;
--DROP TABLE Tag;
--DROP TABLE Task;

-- Create tables --

CREATE TABLE Task (
taskId      INTEGER      NOT NULL,
parentId      INTEGER DEFAULT '-1' NOT NULL, -- -1 -> Task::INVALID_ID
description      STRING      NOT NULL,
longDescription      STRING,
dateCreated      STRING      NOT NULL,
dateLastModified      STRING      NOT NULL,
dateStarted      STRING,
dateDeadline      STRING,
dateCompleted      STRING,
estDuration      STRING,
recurrence      STRING,
priority      STRING      NOT NULL,
completedPercentage      INTEGER      DEFAULT '0'  NOT NULL,
done      STRING      DEFAULT '0'  NOT NULL,
CONSTRAINT pk_Task PRIMARY KEY (taskId));

CREATE TABLE Tag (
tagId      INTEGER      NOT NULL,
tagName      STRING      NOT NULL, -- +UNIQUE
CONSTRAINT pk_Tag PRIMARY KEY (tagId));

CREATE TABLE Tagged (
taskId      INTEGER      NOT NULL,
tagId      INTEGER      NOT NULL,
CONSTRAINT pk_Tagged PRIMARY KEY (taskId, tagId),
CONSTRAINT fk_Tagged_Task FOREIGN KEY (taskId) REFERENCES Task(taskId),
CONSTRAINT fk_Tagged_Tag FOREIGN KEY (tagId) REFERENCES Tag(tagId));

CREATE TABLE FilterRule (
filterRuleId      INTEGER      NOT NULL,
name      STRING      NOT NULL, -- +UNIQUE (?)
rule      STRING      NOT NULL,
CONSTRAINT pk_FilterRule PRIMARY KEY (filterRuleId));
