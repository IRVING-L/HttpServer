#ifndef USERMODEL_H
#define USERMODEL_H
#include "database.h"
#include "user.h"
#include <iostream>
#include <string>

class UserModel
{
public:
    // 在数据库中新增一个用户信息
    bool insert(User &usr);
    // 在数据库中查找用户信息
    User query(std::string &name);
};
#endif