CREATE DATABASE IF NOT EXISTS seguridad1;

USE seguridad1;

DROP TABLE IF EXISTS carts_products;
DROP TABLE IF EXISTS products;
DROP TABLE IF EXISTS carts;
DROP TABLE IF EXISTS mailbox;
DROP TABLE IF EXISTS cards_info;
DROP TABLE IF EXISTS users;

CREATE TABLE IF NOT EXISTS users
(
    id         INT AUTO_INCREMENT PRIMARY KEY,
    name       VARCHAR(30)  NOT NULL,
    username   VARCHAR(10)  NOT NULL,
    email      VARCHAR(50)  NOT NULL,
    password   VARCHAR(255) NOT NULL,
    rol        VARCHAR(30)  NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT usernames_unique UNIQUE (username),
    CONSTRAINT emails_unique UNIQUE (email)
);

CREATE TABLE IF NOT EXISTS cards_info
(
    card_number   VARCHAR(30) NOT NULL PRIMARY KEY,
    expire        VARCHAR(10) NOT NULL,
    security_code VARCHAR(5)  NOT NULL,
    id_user       INT,
    FOREIGN KEY (id_user)
        REFERENCES users (id)
        ON UPDATE RESTRICT
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS carts
(
    id         INT AUTO_INCREMENT,
    id_user    INT,
    status     VARCHAR(30) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id, id_user),
    FOREIGN KEY (id_user)
        REFERENCES users (id)
        ON UPDATE RESTRICT
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS products
(
    id          INT AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(60)  NOT NULL,
    description VARCHAR(500),
    quantity    INT UNSIGNED NOT NULL, #max 999 items
    price       INT UNSIGNED NOT NULL, #max $999
    id_user     INT          NOT NULL,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (id_user)
        REFERENCES users (id)
        ON UPDATE RESTRICT
        ON DELETE CASCADE
);

UPDATE products
SET quantity = quantity - 1
WHERE id = 3;

CREATE TABLE IF NOT EXISTS carts_products
(
    id_cart    INT NOT NULL,
    id_product INT NOT NULL,
    quantity   INT NOT NULL,
    PRIMARY KEY (id_cart, id_product),
    FOREIGN KEY (id_cart)
        REFERENCES carts (id)
        ON UPDATE RESTRICT
        ON DELETE CASCADE,
    FOREIGN KEY (id_product)
        REFERENCES products (id)
        ON UPDATE RESTRICT
        ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS mailbox
(
    id         INT AUTO_INCREMENT PRIMARY KEY,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    name       VARCHAR(30)  NOT NULL,
    email      VARCHAR(30)  NOT NULL,
    message    VARCHAR(500) NOT NULL
);

INSERT INTO users(name, username, email, password, rol)
VALUES ('Vladimir', 'vladsagot', 'vladsagot@gmail.com', MD5('123'), 'admin');

INSERT INTO users(name, username, email, password, rol)
VALUES ('Vladimir', '', 'vladsagot@gmail.com', MD5('123'), 'admin');

INSERT INTO carts(id_user, status)
VALUES (1, 'active');

INSERT INTO products(name, description, quantity, price, id_user)
VALUES ('Orange', 'A deliciuos fruit.', 10, 1, 1);

INSERT INTO products(name, description, quantity, price, id_user)
VALUES ('Banana', 'A deliciuos fruit.', 10, 1, 1);

INSERT INTO products(name, description, quantity, price, id_user)
VALUES ('Strawberry', 'A deliciuos fruit.', 10, 2, 1);

INSERT INTO products(name, description, quantity, price, id_user)
VALUES ('Pineapple', 'A deliciuos fruit.', 10, 1, 1);

INSERT INTO products(name, description, quantity, price, id_user)
VALUES ('Apple', 'A deliciuos fruit.', 10, 1.5, 1);

INSERT INTO products(name, description, quantity, price, id_user)
VALUES ('Green apple', 'A deliciuos fruit.', 10, 1.5, 1);

