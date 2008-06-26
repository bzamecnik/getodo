-- Skript pro vytvoreni schematu databaze --
-- Vygenerovan: 2.5.2008 17:44

-- Odstraneni tabulek --

DROP TABLE Tagged;
DROP TABLE Subtask;
DROP TABLE Tag;
DROP TABLE Task;

-- Vytvoreni tabulek --

CREATE TABLE Task (
TaskID      Integer      NOT NULL,
description      Varchar(255)      NOT NULL,
dateCreated      Date      NOT NULL,
dateStarted      Date,
dateDeadline      Date,
dateCompleted      Date,
estDuration      Integer,
recurrence      Integer      NOT NULL,
priority      Integer      NOT NULL,
completedPercentage      Integer      DEFAULT '0'  NOT NULL,
CONSTRAINT pk_Task PRIMARY KEY (TaskID));

CREATE TABLE Tag (
tagID      Integer      NOT NULL,
tagName      Varchar(255)      NOT NULL  UNIQUE,
CONSTRAINT pk_Tag PRIMARY KEY (tagID));

CREATE TABLE Subtask (
subtask_TaskID      Integer      NOT NULL,
supertask_TaskID      Integer      NOT NULL,
CONSTRAINT pk_Subtask PRIMARY KEY (subtask_TaskID),
CONSTRAINT fk_Subtask_Task FOREIGN KEY (subtask_TaskID, supertask_TaskID) REFERENCES Task(TaskID,TaskID));

CREATE TABLE Tagged (
Task_TaskID      Integer      NOT NULL,
Tag_tagID      Integer      NOT NULL,
CONSTRAINT pk_Tagged PRIMARY KEY (Task_TaskID, Tag_tagID),
CONSTRAINT fk_Tagged_Task FOREIGN KEY (Task_TaskID) REFERENCES Task(TaskID),
CONSTRAINT fk_Tagged_Tag FOREIGN KEY (Tag_tagID) REFERENCES Tag(tagID));

-- Konec skriptu --
