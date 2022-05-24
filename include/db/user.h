#ifndef USER_H
#define USER_H
#include <string>
class User
{
public:
    User(const std::string &name = "", const std::string &pwd = "")
        : _name(name), _pwd(pwd) {}
    void setName(std::string &name) { _name = name; }
    void setPwd(std::string &pwd) { _pwd = pwd; }
    const std::string getName() const { return _name; }
    const std::string getPwd() const { return _pwd; }

private:
    std::string _name;
    std::string _pwd;
};

#endif