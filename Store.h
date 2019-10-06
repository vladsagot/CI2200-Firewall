//
// Created by vladimir on 10/5/19.
//

#ifndef TAREA1_SEGURIDAD_STORE_H
#define TAREA1_SEGURIDAD_STORE_H

#include <algorithm>
#include <iostream>
#include <string>
#include <regex>

#include <mysql-cppconn-8/mysql/jdbc.h>

using namespace std;

class Store {
private:
    /*
     * Variable initialization
     */

    // Database information
    string sql_db_url, sql_db_username, sql_db_password;

    // Determines if user is login
    bool user_login;

    // Stores logged user information
    string user_id;
    string user_name;

    // Boolean values for dynamic product templates
    bool product_template;
    bool product_template_conn;

    /*
     * Methods
     */

    static string get_product_id_from_URL(const string &line);

    void login_template(const string &data);

    void no_login_template(const string &data);

    void buy_template();

    static string format_email(const string &line);

    static string get_token(const string &line);

    static vector<string> get_tokens(const string &line);

    static string get_cookie_token(const string &line);

    static vector<string> get_cookie_tokens(const string &line);

    static bool check_product_conn_URL(const string &line);

    static void general_search_template();

    void general_search_conn_template();

    static void contact_template();

    void contact_conn_template();

    void create_new_cart(const string &username);

    string get_user_active_cart_id();

    void cart_template();

    static bool check_product_URL(const string &line);

public:
    void run();
};


#endif //TAREA1_SEGURIDAD_STORE_H
