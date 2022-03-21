CREATE TABLE Users (
    u_id int NOT NULL AUTO_INCREMENT, 
    login VARCHAR(30) NOT NULL UNIQUE, 
    first_name VARCHAR(30), 
    last_name VARCHAR(30), 
    age int,
    PRIMARY KEY (u_id)
);


INSERT INTO Users 
    (login, first_name, last_name, age) 
VALUES
    ('Dad. Fedor', 'Fedor', 'Penin', 23);