-- GeToDo
-- Vygenerovan ERTOSem: 2.5.2008 17:44

-- Odstraneni tabulek --

--DROP TABLE Tagged;
--DROP TABLE Subtask;
--DROP TABLE Tag;
--DROP TABLE Task;

-- Vytvoreni tabulek --

CREATE TABLE Task (
taskId      INTEGER      NOT NULL,
description      STRING      NOT NULL,
longDescription      STRING      NOT NULL,
dateCreated      STRING      NOT NULL,
dateStarted      STRING,
dateDeadline      STRING,
dateCompleted      STRING,
estDuration      STRING,
recurrence      STRING,
priority      STRING      NOT NULL,
completedPercentage      INTEGER      DEFAULT '0'  NOT NULL,
CONSTRAINT pk_Task PRIMARY KEY (taskId));

CREATE TABLE Tag (
tagId      INTEGER      NOT NULL,
tagName      STRING      NOT NULL  UNIQUE,
CONSTRAINT pk_Tag PRIMARY KEY (tagId));

CREATE TABLE Subtask (
sub_taskId      INTEGER      NOT NULL,
super_taskId      INTEGER      NOT NULL,
CONSTRAINT pk_Subtask PRIMARY KEY (sub_taskId, super_taskId),
CONSTRAINT fk_Subtask_Task FOREIGN KEY (sub_taskId, super_taskId) REFERENCES Task(taskId,taskId));

CREATE TABLE Tagged (
taskId      INTEGER      NOT NULL,
tagId      INTEGER      NOT NULL,
CONSTRAINT pk_Tagged PRIMARY KEY (taskId, tagId),
CONSTRAINT fk_Tagged_Task FOREIGN KEY (taskId) REFERENCES Task(taskId),
CONSTRAINT fk_Tagged_Tag FOREIGN KEY (tagId) REFERENCES Tag(tagId));

-- Konec skriptu --
