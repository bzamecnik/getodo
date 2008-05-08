-- GeToDo
-- Vygenerovan ERTOSem: 2.5.2008 17:44

-- Odstraneni tabulek --

--DROP TABLE Tagged;
--DROP TABLE Subtask;
--DROP TABLE Tag;
--DROP TABLE Task;

-- Vytvoreni tabulek --

CREATE TABLE Task (
taskID      INTEGER      NOT NULL,
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
CONSTRAINT pk_Task PRIMARY KEY (taskID));

CREATE TABLE Tag (
tagID      INTEGER      NOT NULL,
tagName      STRING      NOT NULL  UNIQUE,
CONSTRAINT pk_Tag PRIMARY KEY (tagID));

CREATE TABLE Subtask (
sub_taskID      INTEGER      NOT NULL,
super_taskID      INTEGER      NOT NULL,
CONSTRAINT pk_Subtask PRIMARY KEY (sub_taskID, super_taskID),
CONSTRAINT fk_Subtask_Task FOREIGN KEY (sub_taskID, super_taskID) REFERENCES Task(taskID,taskID));

CREATE TABLE Tagged (
taskID      INTEGER      NOT NULL,
tagID      INTEGER      NOT NULL,
CONSTRAINT pk_Tagged PRIMARY KEY (taskID, tagID),
CONSTRAINT fk_Tagged_Task FOREIGN KEY (taskID) REFERENCES Task(taskID),
CONSTRAINT fk_Tagged_Tag FOREIGN KEY (tagID) REFERENCES Tag(tagID));

-- Konec skriptu --
