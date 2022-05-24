#include "usermodel.h"
#include <stdio.h>
// 在数据库中新增一个用户信息
bool UserModel::insert(User &usr)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into user(username, password) values('%s', '%s')", usr.getName().c_str(), usr.getPwd().c_str());
    MySQL mysql;
    if (mysql.connect())
    {
        return mysql.update(sql);
    }
    return false;
}
// 在数据库中查找用户信息
User UserModel::query(std::string &name)
{
    // 1. 组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select * from user where username = '%s'",
            name.c_str());
    MySQL mysql;
    User usr;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr)
            {
                std::string name(row[0]);
                std::string pwd(row[1]);
                usr.setName(name);
                usr.setPwd(pwd);
            }
        }
        mysql_free_result(res);
    }
    return usr;
}