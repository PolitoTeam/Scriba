-- create an empty database. Name of the database:
CREATE DATABASE IF NOT EXISTS editor;

USE editor;


-- drop tables if they already exist
DROP TABLE IF EXISTS USER;

-- create tables

CREATE TABLE USER (
	Username CHAR(255) ,
	Nickname CHAR(255) ,
	Password CHAR(50) NOT NULL,
	Icon	 CHAR(50),
	PRIMARY KEY (Username)
);
