#include <algorithm>
#include <iostream>
#include <string>
#include <regex>

#include <mysql-cppconn-8/mysql/jdbc.h>

using namespace std;

bool registered = false;

string sql_db_url, sql_db_username, sql_db_password;

string get_token(const string &line) {
    size_t init = line.find('=') + 1;
    size_t end = line.find('&');
    size_t npos = end - init;
    return line.substr(init, npos);
}

string format_email(const string &line) {
    return regex_replace(line, regex("%40"), "@");
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

void unregistered_template(const string &data) {
    cout << "content-type: text/html" << endl << endl;
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
        // Login Form
        cout << "<h2>Login</h2>" << endl;
        cout << "<form action=tarea1_seguridad?login-conn method='post'>" << endl;
        cout << "Username <input type='text' name='username' size=10><br>" << endl;
        cout << "<input type='submit' value='Login'>" << endl;
        cout << "</form><br>" << endl;
        cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
    } else if (data == "login-conn") {
        // Login Database Connection
        string line;
        getline(cin, line);
        vector<string> lines_buffer = get_tokens(line);

        /*
         * INPUTS:
         * lines_buffer[0] : username
         */
        string username = lines_buffer[0];

        sql::mysql::MySQL_Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

            string query = "SELECT username FROM users WHERE username = '" + username + "';";

            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            res = stmt->executeQuery(query);

            if (res->next()) {
                registered = true;
                cout << "Successful login" << endl;
            } else {
                cout << "Incorrect user or password." << endl;
            }

            cout << "<br><a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;

            delete res;
            delete stmt;
            delete con;
        } catch (sql::SQLException &e) {
            cout << "SQLException: " << e.what() << "<br>" << endl;
        }

    } else if (data == "sign-up") {
        // Sign Up Form
        cout << "<h2>Sign up</h2>" << endl;
        cout << "<form action=tarea1_seguridad?sign-up-conn method='post'>" << endl;
        cout << "Name <input type='text' name='name' size=10><br>" << endl;
        cout << "Username <input type='text' name='username' size=10><br>" << endl;
        cout << "Email <input type='text' name='email' size=20><br>" << endl;
        cout << "<input type='submit' value='Submit'>" << endl;
        cout << "</form><br>" << endl;
        cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
    } else if (data == "sign-up-conn") {
        // Sign Up Database Connection
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
        // Search Form
        cout << "<h2>Search</h2>" << endl;
        cout << "<form action=tarea1_seguridad?search-conn method='post'>" << endl;
        cout << "<input type='text' name='name' size=10><br>" << endl;
        cout << "<input type='submit' value='Search'>" << endl;
        cout << "</form>" << endl;
        cout << "<a href='../cgi-bin/tarea1_seguridad'>Go to the main page</a>" << endl;
    } else if (data == "search-conn") {
        // Search Database Connection
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
    } else {
        // Main Site
        cout << "<h2><a href='../cgi-bin/tarea1_seguridad?login'>Login</a></h2>" << endl;
        cout << "<h2><a href='../cgi-bin/tarea1_seguridad?sign-up'>Sign up</a></h2>" << endl;
        cout << "<h2><a href='../cgi-bin/tarea1_seguridad?search'>Search</a></h2>" << endl;

        cout << "DATA: " << data << endl;
    }

    cout << "</body>" << endl;
    cout << "</html>" << endl;
}

int main() {
    string data = getenv("QUERY_STRING");

    sql_db_url = "localhost";
    sql_db_username = "vlad";
    sql_db_password = "";

    if (!registered) {
        unregistered_template(data);
    }
    return 0;
}