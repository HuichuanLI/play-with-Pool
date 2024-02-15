#include <mysql.h>
#include <iostream>

#include "Connection.h"

int main() {

    Connection conn;
//    char sql[1024] = {0};

    std::string sql = "insert into user(id,name) values(2,'lihuichuan')";

    conn.connect("127.0.0.1", 3306, "root", "", "user1");


    conn.update(sql);
}

/*Connection conn;
char sql[1024] = { 0 };
sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
    "zhang san", 20, "male");
conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
conn.update(sql);

//    // 把下面配置信息修改为自己数据库的
//    const std::string host = "127.0.0.1";
//    const std::string user = "root";
//    const std::string pwd = "";
//    const std::string database = "user1";
//    unsigned short port = 0;
//    MySQL conn;
//    conn.connect(host, port, user, pwd, database);
//    conn.update("insert into user(id,name) values(1,'zhang san')");
//    MYSQL_RES *res = conn.query("SELECT * FROM user");
//    std::cout << mysql_num_rows(res) << std::endl;
//    unsigned int num_fields = mysql_num_fields(res);
//    MYSQL_ROW row ;
//    row = mysql_fetch_row(res);
//    while (row){
//        for (int i = 0; i < num_fields; i++)
//        {
//            char *filed = row[i];
//            std::cout << filed << " ";
//        }
//        std::cout << std::endl;
//        row = mysql_fetch_row(res);
//    }
//    return 0;
}
*/