#include <memory>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

template <typename T>
class IDatabase {
public:
    virtual ~IDatabase() = default;
    virtual void addData(shared_ptr<T>& users) = 0;
    virtual T getDataByID(int id) = 0;
    virtual T getDataByUsername(string username) = 0;
    virtual vector<T> getAll() = 0;
};

class Users {
private:
    int id;
    string name;
    string username;
    string password;
    int age;
public:
    Users() {}
    Users(int id, string name, string username, string password, int age) {
        this->id = id;
        this->name = name;
        this->username = username;
        this->password = password;
        this->age = age;
    }
    ~Users() {
        cout << "LOG: User object is destroyed" << endl;
    }
    int getId() {
        return id;
    }
    void setId(int id) {
        this->id = id;
    }
    string getName() {
        return name;
    }
    void setName(string name) {
        this->name = name;
    }
    string getUsername() {
        return username;
    }
    void setUsername(string username) {
        this->username = username;
    }
    string getPassword() {
        return password;
    }
    void setPassword(string password) {
        this->password = password;
    }
    int getAge() {
        return age;
    }
    void setAge(int age) {
        this->age = age;
    }

    friend ostream& operator<<(ostream& COUT, const Users& user) {
        COUT << "Id: " << user.id << endl;
        COUT << "Name: "<< user.name << endl;
        COUT << "Username: " << user.username << endl;
        COUT << "Password: " << user.password << endl;
        COUT << "Age: "<< user.age << endl;

        return COUT;
    }

    friend ostream& operator<<(ostream& COUT, const vector<Users>& users) {
        for (const auto& user : users) {
            COUT << "Id: " << user.id << endl;
            COUT << "Name: "<< user.name << endl;
            COUT << "Username: " << user.username << endl;
            COUT << "Password: " << user.password << endl;
            COUT << "Age: "<< user.age << endl;
        }
        return COUT;
    }
};

template <typename T>
class Database : public IDatabase<T> {
private:
    string db_name = "data.db"; //default db name
    vector<Users> users;
public:
    Database() {
    }
    ~Database() {
        cout << "LOG: Database object is destroyed" << endl;
    }
    void addData(shared_ptr<T>& user) override {
        ofstream outdb_file(db_name, ios::binary | ios::app);
        if (outdb_file.is_open()) {
            // 1. Write ID
            int id = user->getId();
            if (id < 0) {
                throw "invalid id";
            }
            outdb_file.write(reinterpret_cast<const char*>(&id), sizeof(id));

            // 2. Write Name
            string name = user->getName();
            if (name.empty()) {
                throw "name is empty";
            }
            size_t name_len = name.size();
            //Write string size
            outdb_file.write(reinterpret_cast<const char*>(&name_len), sizeof(name_len));
            outdb_file.write(name.data(),name_len);

            // 3. Write Username
            string username = user->getUsername();
            if (username.empty()) {
                throw "username is empty";
            }
            size_t username_len = username.size();

            //Write string size
            outdb_file.write(reinterpret_cast<const char*>(&username_len), sizeof(username_len));
            outdb_file.write(username.data(),username_len);

            //4. Write Password
            string password = user->getPassword();
            if (password.empty()) {
                throw "password is empty";
            }
            size_t password_len = password.size();
            //Write string size
            outdb_file.write(reinterpret_cast<const char*>(&password_len), sizeof(password_len));
            outdb_file.write(password.data(),password_len);

            //5. Write Age
            int age = user->getAge();
            if (age < 0) {
                throw "invalid age";
            }
            outdb_file.write(reinterpret_cast<const char*>(&age), sizeof(age));

            cout << "Data saved to database." << endl;
        }
        else {
            throw "Cannot open database file";
        }
        outdb_file.close();
    }


    T getDataByID(int id) override {
        Users u = extractData('i',id,"");
        if (u.getName().empty()) {
            string str_error = "Data not found with id :";
            str_error.append(to_string(id));
            throw str_error;
        }
        return u;
    }

    T getDataByUsername(string my_username) override {
         Users u = extractData('u', -1, my_username);
        if (u.getName().empty()) {
            string str_error = "Data not found with username :";
            str_error.append(my_username);
            throw str_error;
        }
        return u;
    }

    vector<T> getAll() override {
        extractData('a',-1,"");
        return users;
    }
    T extractData(char operation,int id, string my_username) {
        Users user;
        ifstream indb_file(db_name, ios::binary);
        if (!indb_file.is_open()) {
            throw "Cannot open database file";
        }
        //READ ID
        int my_id = 0;
        while (indb_file.read(reinterpret_cast<char*>(&my_id), sizeof(my_id))) {

            //1. READ NAME
            //READ STRING SIZE
            size_t name_len = 0;
            indb_file.read(reinterpret_cast<char*>(&name_len), sizeof(name_len));
            string name(name_len,'\0');
            indb_file.read(&name[0], name_len);

            //2. READ USERNAME
            //READ STRING SIZE
            size_t username_len = 0;
            indb_file.read(reinterpret_cast<char*>(&username_len), sizeof(username_len));
            string username(username_len,'\0');
            indb_file.read(&username[0], username_len);

            //3. READ PASSWORD
            //READ STRING SIZE
            size_t password_len = 0;
            indb_file.read(reinterpret_cast<char*>(&password_len), sizeof(password_len));
            string password(password_len,'\0');
            indb_file.read(&password[0], password_len);

            //4. READ AGE
            int age = 0;
            indb_file.read(reinterpret_cast<char*>(&age), sizeof(age));


            user.setId(my_id);
            user.setName(name);
            user.setUsername(username);
            user.setPassword(password);
            user.setAge(age);

            if (operation == 'i') {
                if (my_id == id) {
                    return user;
                }
            }
            else if (operation == 'u') {
                if (my_username == username) {
                    return user;
                }
            }
            else if (operation == 'a') {
                users.push_back(user);
            }
        }
        return Users{};
    }
};
int main() {
    try {
        auto user = make_shared<Users>(
            2, "Trace Millie", "tracey", "tracey7@1", 15);
        auto database = make_unique<Database<Users>>();

        //database->addData(user); // save user details to database

        Users getUser = database->getDataByID(1);
        cout << getUser << endl;

        //vector<Users> users = database->getAll();
        //cout << users << endl;
    }
    catch (const char* exp) {
        cerr << "error: " << exp << endl;
    }
    catch (const string& exp) {
        cerr << "error: " << exp << endl;
    }
    catch (...) {
        cerr << "an error occurred" << endl;
    }


    return 0;
}