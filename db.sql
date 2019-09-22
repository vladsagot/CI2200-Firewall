CREATE DATABASE seguridad1;

USE seguridad1;

DROP TABLE order_products;
DROP TABLE products;
DROP TABLE orders;
DROP TABLE suggestions_mailbox;
DROP TABLE users;

CREATE TABLE IF NOT EXISTS users
(
    id         INT AUTO_INCREMENT PRIMARY KEY,
    name       VARCHAR(30) NOT NULL,
    username   VARCHAR(30) NOT NULL,
    email      VARCHAR(50) NOT NULL,
    rol        VARCHAR(30) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT usernames_unique UNIQUE (username),
    CONSTRAINT emails_unique UNIQUE (email)
);

CREATE TABLE IF NOT EXISTS suggestions_mailbox
(
    id         INT AUTO_INCREMENT PRIMARY KEY,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    name       VARCHAR(30) NOT NULL,
    message    VARCHAR(1000)
);

CREATE TABLE IF NOT EXISTS orders
(
    id         INT AUTO_INCREMENT,
    id_user    INT,
    status     VARCHAR(255) NOT NULL,
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
    name        VARCHAR(30)   NOT NULL,
    description VARCHAR(1000),
    quantity    INT           NOT NULL,
    price       DECIMAL(6, 2) NOT NULL,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    CONSTRAINT names_unique UNIQUE (name)
);

CREATE TABLE IF NOT EXISTS order_products
(
    id_order   INT NOT NULL,
    id_product INT NOT NULL,
    quantity   INT NOT NULL,
    PRIMARY KEY (id_order, id_product),
    FOREIGN KEY (id_order)
        REFERENCES orders (id)
        ON UPDATE RESTRICT
        ON DELETE CASCADE,
    FOREIGN KEY (id_product)
        REFERENCES products (id)
        ON UPDATE RESTRICT
        ON DELETE CASCADE
);