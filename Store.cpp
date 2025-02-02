//
// Created by vladimir on 10/5/19.
//

#include "Store.h"

string
Store::get_product_id_from_URL(const string &line) {
    return line.substr(line.find('=') + 1);
}

string
Store::format_email(const string &line) {
    return regex_replace(line, regex("%40"), "@");
}

string
Store::get_token(const string &line) {
    size_t init = line.find('=') + 1;
    size_t end = line.find('&');
    size_t npos = end - init;
    return line.substr(init, npos);
}

vector<string> Store::get_tokens(const string &line) {
    size_t
            tokens_n = count(line.begin(), line.end(), '&') + 1;
    vector<string> lines_buffer;
    string
            buffer = line;
    string
            token;
    for (size_t i = 0; i < tokens_n; ++i) {
        token = get_token(buffer);
        if (std::regex_match(token, std::regex("(.*)(%40)(.*)")))    // Format a token with '@'
            token = format_email(token);
        lines_buffer.push_back(token);
        buffer = buffer.substr(buffer.find('&') + 1);
    }
    return lines_buffer;
}

string
Store::get_cookie_token(const string &line) {
    size_t init = line.find('=') + 1;
    size_t end = line.find(';');
    size_t npos = end - init;
    return line.substr(init, npos);
}

vector<string> Store::get_cookie_tokens(const string &line) {
    size_t tokens_n = count(line.begin(), line.end(), ';') + 1;
    vector<string> lines_buffer;
    string buffer = line;
    string token;

    for (size_t i = 0; i < tokens_n; ++i) {
        token = get_cookie_token(buffer);
        lines_buffer.push_back(token);
        buffer = buffer.substr(buffer.find(';') + 1);
    }

    bool verified_cookies = false;

    /*
     * Login database connection
     */

    /*
     * INPUTS:
     * cookie_tokens[0]: UserID
     * cookie_tokens[1]: UserName
     */
    if (lines_buffer.size() >= 2) {
        try {
            sql::Driver *driver;
            sql::Connection *con;
            sql::Statement *stmt;
            sql::ResultSet *res;

            driver = get_driver_instance();
            con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

            string query =
                    "SELECT * FROM users WHERE username = '" + lines_buffer[1] +
                    "' AND id = " + lines_buffer[0] + ";";

            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            res = stmt->executeQuery(query);

            if (res->next()) {
                verified_cookies = true;
            }

            delete res;
            delete stmt;
            delete con;
        }
        catch (sql::SQLException &e) {}
    }

    if (!verified_cookies) {
        lines_buffer.clear();
    }

    return lines_buffer;
}

bool
Store::check_product_URL(const string &line) {
    return std::regex_match(line, std::regex("(product=)([0-9]+)"));
}

bool
Store::check_product_conn_URL(const string &line) {
    return std::regex_match(line, std::regex("(product-conn=)([0-9]+)"));
}

void
Store::general_search_template() {
    // ------------------------------------------------------------------
    // Search Form
    // ------------------------------------------------------------------
    cout << "<h2>Search</h2>" << endl;
    cout
            << "<div class='alert alert-secondary' role='alert'>Leave the search space empty and click on the button to see all the products.</div>"
            << endl;
    cout << "<form action=store.cgi?search-conn method='post'>" << endl;
    cout << "<input type='text' name='name' size=30 maxlength=30><br>" << endl;
    cout << "<input type='submit' value='Search'>" << endl;
    cout << "</form>" << endl;
    cout <<
         "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>" <<
         endl;
}

void
Store::general_search_conn_template() {
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
    string product_name = remove_string_sum_signs(lines_buffer[0].substr(0, 30));
    if (!string_only_letters_and_numbers(product_name)) product_name = "";

    // cout << product_name << endl;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

        string query =
                "SELECT id, name, quantity, price FROM products WHERE name LIKE '%" +
                product_name + "%' ORDER BY name;";

        stmt = con->createStatement();
        stmt->execute("USE seguridad1");
        res = stmt->executeQuery(query);

        cout
                << "<table class='table'><tr> <th scope='col'>Name</th> <th scope='col'>Quantity</th> <th scope='col'>Price</th> </tr>"
                << endl;
        while (res->next()) {
            cout << "<tr>" << endl;
            cout << "<th scope='col'><a href='../cgi-bin/store.cgi?product=" +
                    res->getString("id") +
                    "'>" << res->getString("name") << "</a></th>" << endl;
            cout << "<th scope='col'>" << res->getString("Quantity") << "</th>" << endl;
            cout << "<th scope='col'>" << "$" << res->getString("Price") << "</th>" << endl;
            cout << "</tr>" << endl;
        }
        cout << "</table><br>" << endl;

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException &e) {
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
    }
    cout <<
         "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>" <<
         endl;
}

void
Store::contact_template() {
    // ------------------------------------------------------------------
    // Contact Form
    // ------------------------------------------------------------------
    cout << "<h2>Contact</h2>" << endl;
    cout << "<form action=store.cgi?contact-conn method='post'>" << endl;
    cout << "Name <input type='text' name='name' size=30 maxlength=30><br>" << endl;
    cout << "Email <input type='text' name='email' size=30 maxlength=30><br>" << endl;
    cout << "Message <input type='text' name='message' size=50 maxlength=500><br>" << endl;
    cout << "<input type='submit' value='Send'>" << endl;
    cout << "</form>" << endl;
    cout <<
         "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>" <<
         endl;
}

void
Store::contact_conn_template() {
    // ------------------------------------------------------------------
    // Contact Database Connection
    // ------------------------------------------------------------------
    cout << "<h2>Contact</h2>" << endl;

    string line;
    getline(cin, line);
    vector<string> lines_buffer = get_tokens(line);

    /*
     * INPUTS:
     * lines_buffer[0]: name
     * lines_buffer[1]: email
     * lines_buffer[2]: message
     * */
    string name = lines_buffer[0].substr(0, 30);
    string email = lines_buffer[1].substr(0, 30);
    string message = lines_buffer[2].substr(0, 500);

    if (string_only_letters_and_numbers(name)) name = "";
    if (string_only_letters_and_numbers(message)) message = "";

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;

        driver = get_driver_instance();
        con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

        stmt = con->createStatement();
        stmt->execute("USE seguridad1");
        stmt->execute("INSERT INTO mailbox(name, email, message) VALUES ('" +
                      name + "','" + email + "','" + message + "')");

        delete stmt;
        delete con;

        cout << "<div class='alert alert-primary' role='alert'>Thanks for your message!</div>" << endl;
    } catch (sql::SQLException &e) {
        // cout << "SQLException: " << e.what() << "<br>" << endl;
        cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
    }
    cout <<
         "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>" <<
         endl;
}

void
Store::create_new_cart(const string &username) {

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

        string query =
                "SELECT id, username FROM users WHERE username = '" + username + "';";

        stmt = con->createStatement();
        stmt->execute("USE seguridad1");
        res = stmt->executeQuery(query);

        /*
         * If a cart exists, then set the status to 'inactive' value and creates a new one,
         * when this happens is because an user bought the items in a cart
         */
        while (res->next()) {
            stmt->
                    execute("UPDATE carts SET status = 'inactive' WHERE id_user = " +
                            res->getString("id") + " AND status = 'active'");
            stmt->execute("INSERT INTO carts(id_user,status) VALUES (" +
                          res->getString("id") + ",'active')");
        }

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException &e) {
        cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
    }
}

string
Store::get_user_active_cart_id() {

    string result;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        driver = get_driver_instance();
        con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

        string query =
                "SELECT id FROM carts WHERE status = 'active' AND id_user = '" +
                user_id + "'";

        stmt = con->createStatement();
        stmt->execute("USE seguridad1");
        res = stmt->executeQuery(query);

        if (res->next())
            result = res->getString("id");

        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException &e) {
        cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
    }

    return result;
}

void
Store::cart_template() {
    cout << "<h2>Cart</h2>" << endl;

    string active_cart = get_user_active_cart_id();
    int cart_size = 0;

    try {
        sql::Driver *driver;
        sql::Connection *con;
        sql::Statement *stmt;
        sql::ResultSet *res;

        double total = 0;

        driver = get_driver_instance();
        con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

        string query =
                "SELECT products.name, products.price, carts_products.quantity, products.price * carts_products.quantity AS total FROM carts_products, products, carts WHERE carts.id = "
                + active_cart +
                " AND carts_products.id_cart = carts.id AND carts_products.id_product = products.id;";

        stmt = con->createStatement();
        stmt->execute("USE seguridad1");
        res = stmt->executeQuery(query);

        cout <<
             "<table class='table'><tr> <th scope='col'>Name</th> <th scope='col'>Price</th> <th scope='col'>Quantity</th> <th scope='col'>Subtotal</th> </tr>"
             << endl;

        while (res->next()) {
            cart_size++;
            cout << "<tr>" << endl;
            cout << "<th scope='col'>" << res->getString("name") << "</th>" << endl;
            cout << "<th scope='col'> $" << res->getString("price") << "</th>" << endl;
            cout << "<th scope='col'>" << res->getString("quantity") << "</th>" << endl;
            cout << "<th scope='col'> $" << res->getString("total") << "</th>" << endl;
            cout << "</tr>" << endl;

            total += stod(res->getString("total"));
        }
        cout
                << "<tr style='height: 50px;'><th scope='col'>Total</th><th scope='col'></th><th scope='col'></th><th scope='col'>$"
                << total << "</th></tr>" << endl;
        cout << "</table>" << endl;
        delete res;
        delete stmt;
        delete con;
    }
    catch (sql::SQLException &e) {
        // cout << "SQLException: " << e.what() << "<br>" << endl;
        cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
    }

    cout << "<form action=store.cgi?buy method='post'>" << endl;
    cout << "Card number <input type='text' name='card' size=30 minlength=16 maxlength=16 required><br>" << endl;
    cout << "Expires <input type='text' name='expires' size=10 minlength=5 maxlength=5 required><br>" << endl;
    cout << "Security code <input type='text' name='code' size=10 minlength=3 maxlength=3 required><br>" << endl;
    if (cart_size > 0) { cout << "<input type='submit' value='Buy'>" << endl; }
    else { cout << "<input type='submit' value='Buy' disabled>" << endl; }
    cout << "</form>" << endl;

    cout <<
         "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>" <<
         endl;
}

void
Store::buy_template() {
    cout << "<h2>Buy</h2>" << endl;

    string line;
    getline(cin, line);
    vector<string> lines_buffer = get_tokens(line);

    /*
     * INPUTS:
     * lines_buffer[0]: card
     * lines_buffer[1]: expires
     * lines_buffer[2]: code
     * */
    string card = lines_buffer[0].substr(0, 16);
    string expires = lines_buffer[1].substr(0, 5);
    string code = lines_buffer[2].substr(0, 3);

    bool invalid = false;

    if (!string_only_letters_and_numbers(card) || !string_only_letters_and_numbers(card) ||
        !string_only_letters_and_numbers(card))
        invalid = true;

    string active_cart = get_user_active_cart_id();

    if (!invalid) {
        try {
            sql::Driver *driver;
            sql::Connection *con;
            sql::Statement *stmt;
            sql::ResultSet *res;

            driver = get_driver_instance();
            con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

            // -------------------------
            // CARD REGISTRATION
            // -------------------------
            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            stmt->
                    execute
                    ("INSERT INTO cards_info(card_number, expire, security_code, id_user) VALUES ('"
                     + card + "','" + expires + "', MD5('" + code + "')," + user_id + ");");

            // -------------------------
            // TRANSACTION
            // -------------------------
            string query =
                    "SELECT products.id, products.price, carts_products.quantity, products.price * carts_products.quantity AS total FROM carts_products, products, carts WHERE carts.id = "
                    + active_cart +
                    " AND carts_products.id_cart = carts.id AND carts_products.id_product = products.id;";

            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            res = stmt->executeQuery(query);

            while (res->next()) {
                stmt->execute("UPDATE products SET quantity = quantity - " +
                              res->getString("quantity") + " WHERE id = " +
                              res->getString("id") + ";");
            }

            // -------------------------
            // CREATE A NEW CART FROM USER, AFTER TRANSACTION
            // -------------------------

            delete stmt;
            delete con;

            create_new_cart(user_name);

            cout << "<div class='alert alert-primary' role='alert'>Thanks! Hope to enjoy our products!</div>" << endl;
        }
        catch (sql::SQLException &e) {
            // cout << "SQLException: " << e.what() << "<br>" << endl;
            cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
        }
    }
    cout <<
         "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>" <<
         endl;
}

void
Store::no_login_template(const string &data) {

    if (user_login_fail)
        cout << "<div class='alert alert-danger' role='alert'>Invalid user or password. Try again.</div>" << endl;

    if (data == "login") {
        // ------------------------------------------------------------------
        // Login Form
        // ------------------------------------------------------------------
        cout << "<h2>Login</h2>" << endl;
        cout << "<form action=store.cgi?login-check-in method='post'>" <<
             endl;
        cout << "Username <input type='text' name='username' size=15 maxlength=10 required><br>" <<
             endl;
        cout << "Password <input type='password' name='password' size=15 maxlength=10 required><br>" << endl;
        cout << "<input type='submit' value='Login'>" << endl;
        cout << "</form><br>" << endl;
        cout <<
             "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>"
             << endl;
    } else if (data == "sign-up") {
        // ------------------------------------------------------------------
        // Sign Up Form
        // ------------------------------------------------------------------
        cout << "<h2>Sign up</h2>" << endl;
        cout << "<form action=store.cgi?sign-up-conn method='post'>" <<
             endl;
        cout << "Name <input type='text' name='name' size=15 minlength=5 maxlength=30 required><br>" << endl;
        cout << "Username <input type='text' name='username' size=15 minlength=5 maxlength=10 required><br>" <<
             endl;
        cout << "Email <input type='text' name='email' size=30 minlength=5 maxlength=50 required><br>" << endl;
        cout << "Password <input type='password' name='password' size=15 minlength=5 maxlength=10 required><br>"
             << endl;
        cout << "<p>Password size up to 10 characters.</p>" << endl;
        cout << "<input type='submit' value='Submit'>" << endl;
        cout << "</form><br>" << endl;
        cout <<
             "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>"
             << endl;
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
         * lines_buffer[3] : password
         */
        string name = lines_buffer[0].substr(0, 30);
        string username = lines_buffer[1].substr(0, 10);
        string email = lines_buffer[2].substr(0, 50);
        string password = lines_buffer[3].substr(0, 10);
        string rol = "buyer";

        bool invalid = false;

        if (!string_only_letters_and_numbers(name) || !string_only_letters_and_numbers(username) ||
            !string_only_letters_and_numbers(password))
            invalid = true;

        /*
        for (const auto &i : lines_buffer) {
            cout << i << endl;
        }
         */

        if (name.size() > 4 && username.size() > 4 && email.size() > 4 && password.size() > 4 && !invalid) {

            try {
                sql::Driver *driver;
                sql::Connection *con;
                sql::Statement *stmt;

                driver = get_driver_instance();
                con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

                stmt = con->createStatement();
                stmt->execute("USE seguridad1");
                stmt->
                        execute("INSERT INTO users(name, username, email, password, rol) VALUES ('" +
                                name + "','" + username + "','" + email + "', MD5('" + password + "'),'" + rol +
                                "')");

                delete stmt;
                delete con;

                cout << "<div class='alert alert-primary' role='alert'>Successful registration</div>" << endl;
            }
            catch (sql::SQLException &e) {
                // cout << "SQLException: " << e.what() << "<br>" << endl;
                cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
            }
        } else {
            cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
        }

        create_new_cart(username);
        cout <<
             "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>"
             << endl;
    } else if (data == "search") {
        general_search_template();
    } else if (data == "search-conn") {
        general_search_conn_template();
    } else if (data == "contact") {
        contact_template();
    } else if (data == "contact-conn") {
        contact_conn_template();
    } else if (product_template) {
        cout << "<div class='alert alert-secondary' role='alert'>Create a user or login to buy.</div>" << endl;
    } else {
        // ------------------------------------------------------------------
        // Main Site
        // ------------------------------------------------------------------
        cout << "<h2><a href='../cgi-bin/store.cgi?login'>Login</a></h2>"
             << endl;
        cout <<
             "<h2><a href='../cgi-bin/store.cgi?sign-up'>Sign up</a></h2>"
             << endl;
        cout <<
             "<h2><a href='../cgi-bin/store.cgi?search'>Search products</a></h2>"
             << endl;
        cout <<
             "<h2><a href='../cgi-bin/store.cgi?contact'>Contact</a></h2>"
             << endl;
    }
    print_footer();
}

void
Store::login_template(const string &data) {
    cout << "<div class='alert alert-primary' role='alert'>User: " + user_name + "</div>" << endl;

    if (data == "sell") {
        // ------------------------------------------------------------------
        // Sell Form
        // ------------------------------------------------------------------
        cout << "<h2>Sell product</h2>" << endl;
        cout << "<form action=store.cgi?sell-conn method='post'>" <<
             endl;
        cout << "Product name <input type='text' name='name' size=30 minlength=5 maxlength=60 required><br>" <<
             endl;
        cout << "Description <input type='text' name='description' size=40 minlength=5 maxlength=500 required><br>"
             << endl;
        cout << "Quantity <input type='number' name='quantity' size=10 minlength=1 maxlength=3 required><br>" <<
             endl; // max 999 items
        cout << "Price $<input type='number' name='price' size=10 minlength=1 maxlength=3 required><br>"
             << endl;  // max $999
        cout << "<input type='submit' value='Send'>" << endl;
        cout << "</form>" << endl;
        cout <<
             "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>"
             << endl;
    } else if (data == "sell-conn") {
        // ------------------------------------------------------------------
        // Sell Database Connection
        // ------------------------------------------------------------------
        cout << "<h2>Sell product</h2>" << endl;

        string line;
        getline(cin, line);
        vector<string> lines_buffer = get_tokens(line);

        /*
         * INPUTS:
         * lines_buffer[0]: name
         * lines_buffer[1]: description
         * lines_buffer[2]: quantity -- check if [1-999]
         * lines_buffer[3]: price -- check if [$1-$999]
         * */

        string name = lines_buffer[0].substr(0, 60);
        string description = lines_buffer[1].substr(0, 60);

        if (!string_only_letters_and_numbers(name)) name = "";
        if (!string_only_letters_and_numbers(description)) description = "";

        int buff_quantity = 0 < stoi(lines_buffer[2]) && stoi(lines_buffer[2]) < 1000 ? stoi(lines_buffer[2]) : 1;
        int buff_price = 0 < stoi(lines_buffer[3]) && stoi(lines_buffer[3]) < 1000 ? stoi(lines_buffer[3]) : 1;

        string quantity = to_string(buff_quantity);
        string price = to_string(buff_price);

        if (name.size() > 4 && description.size() > 4) {

            try {
                sql::Driver *driver;
                sql::Connection *con;
                sql::Statement *stmt;

                driver = get_driver_instance();
                con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

                stmt = con->createStatement();
                stmt->execute("USE seguridad1");
                stmt->
                        execute
                        ("INSERT INTO products(name, description, quantity, price, id_user) VALUES ('"
                         + name + "','" + description + "'," + quantity + "," + price +
                         "," + user_id + ")");

                delete stmt;
                delete con;

                cout << "<div class='alert alert-primary' role='alert'>Your product is ready to sell!</div>" << endl;
            }
            catch (sql::SQLException &e) {
                // cout << "SQLException: " << e.what() << "<br>" << endl;
                cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
            }
        } else {
            cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
        }
        cout <<
             "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>"
             << endl;
    } else if (data == "search") {
        general_search_template();
    } else if (data == "search-conn") {
        general_search_conn_template();
    } else if (data == "cart") {
        cart_template();
    } else if (data == "buy") {
        buy_template();
    } else if (product_template) {
        // ------------------------------------------------------------------
        // Individual Product Site
        // ------------------------------------------------------------------
        string product_id = get_product_id_from_URL(data);

        try {
            sql::Driver *driver;
            sql::Connection *con;
            sql::Statement *stmt;
            sql::ResultSet *res;

            driver = get_driver_instance();
            con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

            string query =
                    "SELECT * FROM products WHERE id = '" + product_id + "';";

            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            res = stmt->executeQuery(query);

            if (res->next()) {
                cout << "<h2>Product: " << res->
                        getString("name") << "</h2>" << endl;
                cout << "<h3>Price: $" << res->
                        getString("price") << "</h3>" << endl;
                cout << "<p>" << res->getString("description") << "</p>" << endl;
                cout << "<p>In stock: " << res->
                        getString("quantity") << "</p>" << endl;

                cout << "<form action=store.cgi?product-conn=" +
                        product_id + " method='post'>" << endl;
                cout <<
                     "Quantity <input type='number' name='quantity' size=10 min=1 max="
                     + res->getString("quantity") + "><br>" << endl;
                cout << "<input type='submit' value='Add to cart'>" << endl;
                cout << "</form>" << endl;
            } else {
                cout << "<h2>Product doesn't exist</h2>" << endl;
            }

            delete res;
            delete stmt;
            delete con;
        }
        catch (sql::SQLException &e) {
            cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
        }
        cout <<
             "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>"
             << endl;
    } else if (product_template_conn) {
        // ------------------------------------------------------------------
        // Buy Individual Product Site
        // ------------------------------------------------------------------
        string product_id = get_product_id_from_URL(data);
        string active_cart_id = get_user_active_cart_id();

        string line;
        getline(cin, line);
        vector<string> lines_buffer = get_tokens(line);

        /*
         * INPUTS:
         * lines_buffer[0]: quantity
         * */
        string quantity = lines_buffer[0];

        try {
            sql::Driver *driver;
            sql::Connection *con;
            sql::Statement *stmt;

            driver = get_driver_instance();
            con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

            // INSERT INTO carts_products(id_cart, id_product, quantity) VALUES ();
            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            stmt->execute("UPDATE products SET quantity = quantity - " + quantity + " WHERE id = " + product_id + ";");
            stmt->
                    execute
                    ("INSERT INTO carts_products(id_cart, id_product, quantity) VALUES ("
                     + active_cart_id + "," + product_id + "," + quantity + ")");

            delete stmt;
            delete con;

            cout << "<p>Product added to cart</p>" << endl;
        }
        catch (sql::SQLException &e) {
            // cout << "SQLException: " << e.what() << "<br>" << endl;
            cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
        }

        cout <<
             "<a class='btn btn-secondary btn-sm my-5' href='../cgi-bin/store.cgi' role='button'>Go to the main page</a>"
             << endl;
    } else {
        // ------------------------------------------------------------------
        // Main Site
        // ------------------------------------------------------------------
        cout <<
             "<h2><a href='../cgi-bin/store.cgi?sell'>Sell product</a></h2>"
             << endl;
        cout <<
             "<h2><a href='../cgi-bin/store.cgi?search'>Search products</a></h2>"
             << endl;
        cout <<
             "<h2><a href='../cgi-bin/store.cgi?cart'>Shopping cart</a></h2>"
             << endl;
    }
    cout <<
         "<p><a class='btn btn-primary' href='../cgi-bin/store.cgi?login-check-out' role='button'>Log Out</a></p>"
         << endl;

    print_footer();
}

void
Store::run() {
    /*
     * Set initial variables
     */
    // Environment variables
    string data = getenv("QUERY_STRING") ? getenv("QUERY_STRING") : "";
    string cookie = getenv("HTTP_COOKIE") ? getenv("HTTP_COOKIE") : "";

    // Set values for dinamic products templates
    product_template = check_product_URL(data);
    product_template_conn = check_product_conn_URL(data);

    /*
     * End to set initial variables
     */

    // Verify login information
    if (data == "login-check-in") {
        /*
         * Login check in
         */
        string line;
        getline(cin, line);
        vector<string> lines_buffer = get_tokens(line);

        /*
         * INPUTS:
         * lines_buffer[0] : username
         * lines_buffer[1] : password
         */
        string username = lines_buffer[0].substr(0, 10);
        string password = lines_buffer[1].substr(0, 10);

        /*
         * Login database connection
         */

        try {
            sql::Driver *driver;
            sql::Connection *con;
            sql::Statement *stmt;
            sql::ResultSet *res;

            driver = get_driver_instance();
            con = driver->connect(sql_db_url, sql_db_username, sql_db_password);

            string query =
                    "SELECT id, username FROM users WHERE username = '" + username +
                    "' AND password = MD5('" + password + "');";

            stmt = con->createStatement();
            stmt->execute("USE seguridad1");
            res = stmt->executeQuery(query);

            if (res->next()) {

                cout << "Set-Cookie:UserID=" + res->getString("id") +
                        ";" << endl;
                cout << "Set-Cookie:UserName=" + res->getString("username") +
                        ";" << endl;

            } else {
                user_login_fail = true;
            }

            delete res;
            delete stmt;
            delete con;
        }
        catch (sql::SQLException &e) {
            // cout << "<div class='alert alert-danger' role='alert'>Error</div>" << endl;
        }
    }

    /*
     * Logout user by erasing cookie values
     */
    if (data == "login-check-out") {
        cout << "Set-Cookie:UserID=;" << endl;
        cout << "Set-Cookie:UserName=;" << endl;
    }

    // Verify is exist a logged user
    vector<string> cookie_tokens = get_cookie_tokens(cookie);

    /*
     * INPUTS:
     * cookie_tokens[0]: UserID
     * cookie_tokens[1]: UserName
     */
    if (!cookie_tokens.empty() && !cookie_tokens[0].empty()) {
        user_login = true;
        user_id = cookie_tokens[0];
        user_name = cookie_tokens[1];
    }

    /*
     *
     *
     * HTML header starts here
     *
     *
     */
    cout << "Content-type: text/html" << endl << endl;

    // Refresh page if an user is log in or log out
    if (data == "login-check-in" || data == "login-check-out")
        cout <<
             "<meta http-equiv='refresh' content='3; url=../cgi-bin/store.cgi'>"
             << endl;


    print_header();

    /*
     * Checks if user is login to display another template
     */
    if (user_login) {
        // cout << "USER ID: " << user_id << "<br>" << endl;
        login_template(data);
    } else {
        no_login_template(data);
    }

    /*
     * HTML ends here
     */
}

Store::Store() : sql_db_url("127.0.0.1:3306"), sql_db_username("store"), sql_db_password("$tore123#"), user_id(""),
                 user_name("") {
    // Database connection info
    // sql_db_url = "127.0.0.1:3306";
    // sql_db_username = "root";
    // sql_db_password = "mango";

    // Set initial configuration for login
    user_login = false;
    user_login_fail = false;

    // Stores logged user information
    // user_id = "";
    // user_name = "";

    // Boolean values for dynamic product templates
    product_template = false;
    product_template_conn = false;
}

void Store::print_header() {
    //cout << "DATA: " << data << "<br>" << endl;
    //cout << "COOKIE: " << cookie << "<br>" << endl;
    //cout << "PRODUCT TEMPLATE: " << product_template << "<br>" << endl;

    cout << "<!doctype html>" << endl;
    cout << "<html lang='en'>" << endl;
    cout << "<head>" << endl;
    cout << "<meta charset='utf-8'>" << endl;
    cout << "<meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'>" << endl;
    cout << "<title>Store</title>" << endl;
    cout << "<meta name='description' content='Store'>" << endl;
    cout << "<meta name='author' content='Vladimir Sagot'>" << endl;
    cout << "<link rel='stylesheet' href='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css'"
         << " integrity='sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T' crossorigin='anonymous'>"
         << endl;
    cout
            << "<style type='text/css'> body, .sticky-footer-wrapper { min-height:100vh; } .flex-fill { flex:1 1 auto; } </style>"
            << endl;
    cout
            << "<script type='text/javascript'> if (location.protocol != 'https:') { location.href = 'https:' + window.location.href.substring(window.location.protocol.length); } </script>"
            << endl;
    cout << "</head>" << endl;
    cout << "<body class='d-flex flex-column sticky-footer-wrapper'>" << endl;
    cout << "<header><nav class='navbar navbar-dark bg-primary mb-4'>"
         << "<a class='navbar-brand' href='../cgi-bin/store.cgi'>Store</a></nav></header>" << endl;
    cout << "<main role='main' class='flex-fill'>" << endl;
    cout << "<div class='container'><div class='row'><div class='col'>" << endl;
}

void Store::print_footer() {
    cout << "</div></div></div></main>" << endl;
    cout
            << "<footer class='footer mt-auto py-3 bg-dark text-white-50'><div class='container text-center'><small>Vladimir Sagot Muñoz | vladimir.sagot@ucr.ac.cr</small></div></footer>"
            << endl;
    // cout << "<script src='https://code.jquery.com/jquery-3.3.1.slim.min.js' integrity='sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo' crossorigin='anonymous'></script>" << endl;
    // cout << "<script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js' integrity='sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1' crossorigin='anonymous'></script>" << endl;
    // cout << "<script src='https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js' integrity='sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM' crossorigin='anonymous'></script>" << endl;
    cout << "</body>" << endl;
    cout << "</html>" << endl;
}

bool Store::string_only_letters_and_numbers(string s) {
    return !none_of(s.begin(), s.end(), [](unsigned char c) { return isdigit(c) || isalpha(c) || isspace(c); });
}

string Store::remove_string_sum_signs(string s) {
    string buff = std::move(s);
    replace(buff.begin(), buff.end(), '+', ' ');
    return buff;
}
