//
// Created by vladimir on 10/5/19.
//

#ifndef TAREA1_SEGURIDAD_STORE_H
#define TAREA1_SEGURIDAD_STORE_H

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <string>
#include <regex>

#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;

class Store {
private:
    /*
     * Variable initialization
     */

    // Database information
    string sql_db_url, sql_db_username, sql_db_password;

    // Determines if user is login
    bool user_login, user_login_fail;

    // Stores logged user information
    string user_id, user_name;

    // Boolean values for dynamic product templates
    bool product_template, product_template_conn;

    /*
     * Methods
     */

    static bool check_product_conn_URL(const string &line);

    static bool check_product_URL(const string &line);

    static string format_email(const string &line);

    static string get_cookie_token(const string &line);

    static string get_product_id_from_URL(const string &line);

    static string get_token(const string &line);

    static vector<string> get_cookie_tokens(const string &line);

    static vector<string> get_tokens(const string &line);

    static void contact_template();

    static void general_search_template();

    static void print_header();

    static void print_footer();

    string get_user_active_cart_id();

    void buy_template();

    void cart_template();

    void contact_conn_template();

    void create_new_cart(const string &username);

    void general_search_conn_template();

    void login_template(const string &data);

    void no_login_template(const string &data);

public:
    Store();

    void run();
};


#endif //TAREA1_SEGURIDAD_STORE_H
