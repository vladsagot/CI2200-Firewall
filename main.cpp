#include <algorithm>
#include <iostream>
#include <string>
#include <regex>

#include <mysql-cppconn-8/mysql/jdbc.h>

using namespace std;

// Database Info
string sql_db_url, sql_db_username, sql_db_password;

// Login Info
bool user_login;
string user_id;

string format_email(const string &line) {
    return regex_replace(line, regex("%40"), "@");
}

string get_token(const string &line) {
    size_t init = line.find('=') + 1;
    size_t end = line.find('&');
    size_t npos = end - init;
    return line.substr(init, npos);
}

vector<string> get_tokens(const string &line) {
    size_t tokens_n = count(line.begin(), line.end(), '&') + 1;
    vector<string> lines_buffer;
    string buffer = line;
    string token;
    for (size_t i = 0; i < tokens_n; ++i) {
        token = get_token(buffer);
        if (std::regex_match(token, std::regex("(.*)(%40)(.*)"))) // Format a token with '@'
            token = format_email(token);
        lines_buffer.push_back(token);
        buffer = buffer.substr(buffer.find('&') + 1);
    }
    return lines_buffer;
}

string get_cookie_token(const string &line) {
    size_t init = line.find('=') + 1;
    size_t end = line.find(';');
    size_t npos = end - init;
    return line.substr(init, npos);
}

vector<string> get_cookie_tokens(const string &line) {
    size_t tokens_n = count(line.begin(), line.end(), ';') + 1;
    vector<string> lines_buffer;
    string buffer = line;
    string token;
    for (size_t i = 0; i < tokens_n; ++i) {
        token = get_cookie_token(buffer);
        lines_buffer.push_back(token);
        buffer = buffer.substr(buffer.find(';') + 1);
    }
    return lines_buffer;
}

void general_search_template() {
    // ------------------------------------------------------------------
    // Search Form
    // ------------------------------------------------------------------
    cout << "<h2>Search</h2>" << endl;
    cout << "<form action=tarea1_seguridad?search-conn method='post'>" << endl;
    cout << "<input type='text' name='name' size=10><br>" << endl;
    cout << "<input type='submit' value='Search'>" << endl;
    cout << "</form>" << endl;
    cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
}

void general_search_conn_template() {
    // ------------------------------------------------------------------
    // Search Database Connection
    // ------------------------------------------------------------------
    string line;
    getline(cin, line);
    vector<string> lines_buffer = get_tokens(line);

    /*
     * INPUTS:
     * lines_buffer[0] : product_name
     * */
    string product_name = lines_buffer[0];

    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;

    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

        string query = "SELECT name, quantity, price FROM products WHERE name LIKE '%" + product_name +
                       "%' ORDER BY name;";

        stmt = con->createStatement();
        stmt->execute("USE seguridad1");
        res = stmt->executeQuery(query);

        cout << "<table><tr> <th>Name</th> <th>Quantity</th> <th>Price</th> </tr>" << endl;
        while (res->next()) {
            cout << "<tr>" << endl;
            cout << "<th>" << res->getString("Name") << "</th>" << endl;
            cout << "<th>" << res->getString("Quantity") << "</th>" << endl;
            cout << "<th>" << "$" << res->getString("Price") << "</th>" << endl;
            cout << "</tr>" << endl;
        }
        cout << "</table><br>" << endl;

        delete res;
        delete stmt;
        delete con;
    } catch (sql::SQLException &e) {
        cout << "SQLException: " << e.what() << "<br>" << endl;
    }
    cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
}

void contact_template() {
    cout << "<h2>Contact</h2>" << endl;
    cout << "<form action=tarea1_seguridad?contact-conn method='post'>" << endl;
    cout << "Name <input type='text' name='name' size=10><br>" << endl;
    cout << "Email <input type='text' name='email' size=10><br>" << endl;
    cout << "Message <input type='text' name='message' size=40><br>" << endl;
    cout << "<input type='submit' value='Send'>" << endl;
    cout << "</form>" << endl;
    cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
}

void contact_conn_template() {
    cout << "<h2>Contact</h2>" << endl;
    
}

void no_login_template(const string &data) {
    cout << "<!doctype html>" << endl;
    cout << "<html lang='en'>" << endl;

    cout << "<head>" << endl;
    cout << "<meta charset='utf-8'>" << endl;
    cout << "<title>Store</title>" << endl;
    cout << "<meta name='description' content='Store'>" << endl;
    cout << "<meta name='author' content='Vladimir Sagot'>" << endl;
    cout << "</head>" << endl;

    cout << "<body>" << endl;
    cout << "<h1><a href='../cgi-bin/tarea1_seguridad'>Store</a></h1>" << endl;

    if (data == "login") {
        // ------------------------------------------------------------------
        // Login Form
        // ------------------------------------------------------------------
        cout << "<h2>Login</h2>" << endl;
        cout << "<form action=tarea1_seguridad?login-check-in method='post'>" << endl;
        cout << "Username <input type='text' name='username' size=10><br>" << endl;
        cout << "<input type='submit' value='Login'>" << endl;
        cout << "</form><br>" << endl;
        cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
    } else if (data == "sign-up") {
        // ------------------------------------------------------------------
        // Sign Up Form
        // ------------------------------------------------------------------
        cout << "<h2>Sign up</h2>" << endl;
        cout << "<form action=tarea1_seguridad?sign-up-conn method='post'>" << endl;
        cout << "Name <input type='text' name='name' size=10><br>" << endl;
        cout << "Username <input type='text' name='username' size=10><br>" << endl;
        cout << "Email <input type='text' name='email' size=20><br>" << endl;
        cout << "<input type='submit' value='Submit'>" << endl;
        cout << "</form><br>" << endl;
        cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
    } else if (data == "sign-up-conn") {
        // ------------------------------------------------------------------
        // Sign Up Database Connection
        // ------------------------------------------------------------------
        cout << "<h2>Sign up</h2>" << endl;

        string line;
        getline(cin, line);
        vector<string> lines_buffer = get_tokens(line);

        /*
         * INPUTS:
         * lines_buffer[0] : name
         * lines_buffer[1] : username
         * lines_buffer[2] : email
         */
        string name = lines_buffer[0];
        string username = lines_buffer[1];
        string email = lines_buffer[2];
        string rol = "buyer";

        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;

        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            stmt->execute("INSERT INTO users(name, username, email, rol) VALUES ('" +
                          name + "','" + username + "','" + email + "','" + rol + "')");

            delete stmt;
            delete con;

            cout << "Successful registration<br>" << endl;
        } catch (sql::SQLException &e) {
            cout << "SQLException: " << e.what() << "<br>" << endl;
        }
        cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
    } else if (data == "search") {
        general_search_template();
    } else if (data == "search-conn") {
        general_search_conn_template();
    } else if (data == "contact") {
        contact_template();
    } else if (data == "contact_conn") {
        contact_conn_template();
    } else {
        // ------------------------------------------------------------------
        // Main Site
        // ------------------------------------------------------------------
        cout << "<h2><a href='../cgi-bin/tarea1_seguridad?login'>Login</a></h2>" << endl;
        cout << "<h2><a href='../cgi-bin/tarea1_seguridad?sign-up'>Sign up</a></h2>" << endl;
        cout << "<h2><a href='../cgi-bin/tarea1_seguridad?search'>Search</a></h2>" << endl;
        cout << "<h2><a href='../cgi-bin/tarea1_seguridad?contact'>Contact</a></h2>" << endl;
    }
    cout << "</body>" << endl;
    cout << "</html>" << endl;
}

void login_template(const string &data) {
    cout << "<!doctype html>" << endl;
    cout << "<html lang='en'>" << endl;

    cout << "<head>" << endl;
    cout << "<meta charset='utf-8'>" << endl;
    cout << "<title>Store</title>" << endl;
    cout << "<meta name='description' content='Store'>" << endl;
    cout << "<meta name='author' content='Vladimir Sagot'>" << endl;
    cout << "</head>" << endl;

    cout << "<body>" << endl;
    cout << "<h1><a href='../cgi-bin/tarea1_seguridad'>Store</a></h1>" << endl;
    cout << "<h2><a href='../cgi-bin/tarea1_seguridad?login-check-out'>Log Out</a></h2>" << endl;
    cout << "</body>" << endl;
    cout << "</html>" << endl;
}

int main() {
    // ------------------------------------------------------------------
    // Set initial variables
    // ------------------------------------------------------------------
    string data = getenv("QUERY_STRING") ? getenv("QUERY_STRING") : "";
    string cookie = getenv("HTTP_COOKIE") ? getenv("HTTP_COOKIE") : "";

    user_login = false;

    // Database Connection Info
    sql_db_url = "localhost";
    sql_db_username = "vlad";
    sql_db_password = "";
    // ------------------------------------------------------------------

    // Verify Login Info
    if (data == "login-check-in") {
        // ------------------------------------------------------------------
        // Login Check In
        // ------------------------------------------------------------------
        string line;
        getline(cin, line);
        vector<string> lines_buffer = get_tokens(line);

        /*
         * INPUTS:
         * lines_buffer[0] : username
         */
        string username = lines_buffer[0];

        // ------------------------------------------------------------------
        // Login Database Connection
        // ------------------------------------------------------------------
        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

            string query = "SELECT id, username FROM users WHERE username = '" + username + "';";

            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            res = stmt->executeQuery(query);

            if (res->next()) {
                cout << "Set-Cookie:UserID=" + res->getString("ID") + ";" << endl;
            } else {
                cout << "Set-Cookie:UserID=;" << endl;
                //cout << "Incorrect user or password<br>" << endl;
            }

            //cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;

            delete res;
            delete stmt;
            delete con;
        } catch (sql::SQLException &e) {
            //cout << "SQLException: " << e.what() << "<br>" << endl;
        }
    }

    if (data == "login-check-out") {
        cout << "Set-Cookie:UserID=;" << endl;
    }

    // Verify is exist a logged user
    vector<string> cookie_tokens = get_cookie_tokens(cookie);

    /*
     * INPUTS:
     * cookie_tokens[0]: UserID
     * cookie_tokens[1]: Password
     * cookie_tokens[2]: Domain
     * cookie_tokens[3]: Path
     */
    if (!cookie_tokens.empty() && !cookie_tokens[0].empty()) {
        user_login = true;
        user_id = cookie_tokens[0];
    }

    cout << "content-type: text/html" << endl << endl;

    // Refresh page if an user is log in or log out
    if (data == "login-check-in" || data == "login-check-out")
        cout << "<meta http-equiv='refresh' content='0; url=../cgi-bin/tarea1_seguridad'>" << endl;

    cout << "DATA: " << data << "<br>" << endl;
    cout << "COOKIE: " << cookie << "<br>" << endl;
    if (user_login) {
        cout << "UserID: " << user_id << "<br>" << endl;
        login_template(data);
    } else {
        no_login_template(data);
    }
    return 0;
}