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

vector<string> get_tokens(string line) {
    size_t tokens_n = count(line.begin(), line.end(), '&') + 1;
    vector<string> lines_buffer;
    string buffer = line;
    for (size_t i = 0; i < tokens_n; ++i) {
        lines_buffer.push_back(get_token(buffer));
        buffer = buffer.substr(buffer.find('&') + 1);
    }
    return lines_buffer;
}

string format_email(string line) {
    line = regex_replace(line, regex("%40"), "@");
    return line;
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

    } else if (data == "sign-up") { // Sign Up Form
        cout << "<form action=tarea1_seguridad?sign-up-conn method='post'>" << endl;
        cout << "Name <input type='text' name='name' size=10><br>" << endl;
        cout << "Username <input type='text' name='username' size=10><br>" << endl;
        cout << "Email <input type='text' name='email' size=20><br>" << endl;
        cout << "<input type='submit' value='Submit'>" << endl;
        cout << "</form>" << endl;
    } else if (data == "sign-up-conn") { // Sign Up Database Connection
        string line;
        getline(cin, line);
        vector<string> lines_buffer = get_tokens(line);
        lines_buffer[2] = format_email(lines_buffer[2]);

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
            stmt->execute("USE MYSQL");
            stmt->execute("INSERT INTO users(name, username, email, rol) VALUES ('" +
                          name + "','" + username + "','" + email + "','" + rol + "')");

            delete stmt;
            delete con;
        } catch (sql::SQLException &e) {
            cout << e.what() << endl;
        }


        for (const auto &i : lines_buffer) {
            cout << i << "<br>" << endl;
        }

    } else if (data == "search") {

    } else {
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

    sql_db_url = "jdbc:mariadb://localhost:3306";
    sql_db_username = "vlad";
    sql_db_password = "";

    if (!registered) {
        unregistered_template(data);
    }
    return 0;
}