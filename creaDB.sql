-- create an empty database. Name of the database:
CREATE DATABASE IF NOT EXISTS editor;

USE editor;


-- drop tables if they already exist
DROP TABLE IF EXISTS FILE_USER;
DROP TABLE IF EXISTS FILE;
DROP TABLE IF EXISTS USER;




-- create tables

CREATE TABLE USER (
	Username CHAR(255) ,
	Nickname CHAR(255) ,
	Password CHAR(255) NOT NULL,
	PRIMARY KEY (Username)
);

CREATE TABLE FILE (
	Link	CHAR(255),
	Name  CHAR(255),
	Owner CHAR(255),
	Public BOOLEAN,
	FOREIGN KEY(Owner) REFERENCES USER(Username),
	PRIMARY KEY(Link)
);

-- User is not foreign key because the user might be not signed up yet.
CREATE TABLE FILE_USER (
	Link CHAR(255),
	User CHAR(255),
	First_access BOOLEAN,
	FOREIGN KEY(Link) REFERENCES FILE(Link),
	PRIMARY KEY(Link,User)
);
