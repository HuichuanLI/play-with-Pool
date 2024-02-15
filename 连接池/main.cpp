#include <mysql.h>
#include <iostream>

#include "Connection.h"
#include "CommonConnectionPool.h"

int main() {

//    Connection conn;
//    char sql[1024] = {0};
//
//    std::string sql = "insert into user(id,name) values(2,'lihuichuan')";
//
//    conn.connect("127.0.0.1", 3306, "root", "", "user1");
//
//    ConnectionPool *cp = ConnectionPool::getConnectionPool();
//

    clock_t begin = clock();

    thread t1([]() {
        ConnectionPool *cp = ConnectionPool::getConnectionPool();
        for (int i = 0; i < 2500; ++i) {
            char sql[1024] = { 0 };
            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                "zhang san", 20, "male");
            shared_ptr<Connection> sp = cp->getConnection();
            sp->update(sql);
//            Connection conn;
//            char sql[1024] = {0};
//            sprintf(sql, "insert into user(name,id) values('%s',%d)",
//                    "zhang san", 20);
//            conn.connect("127.0.0.1", 3306, "root", "", "user1");
//            conn.update(sql);
        }
    });

//    thread t2([]() {
//        //ConnectionPool *cp = ConnectionPool::getConnectionPool();
//        for (int i = 0; i < 2500; ++i) {
//            /*char sql[1024] = { 0 };
//            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
//                "zhang san", 20, "male");
//            shared_ptr<Connection> sp = cp->getConnection();
//            sp->update(sql);*/
//            Connection conn;
//            char sql[1024] = {0};
//            sprintf(sql, "insert into user(name,id) values('%s',%d)",
//                    "zhang san", 20);
//            conn.connect("127.0.0.1", 3306, "root", "", "user1");
//            conn.update(sql);
//        }
//    });
//
//
//    thread t3([]() {
//        //ConnectionPool *cp = ConnectionPool::getConnectionPool();
//        for (int i = 0; i < 2500; ++i)
//        {
//            /*char sql[1024] = { 0 };
//            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
//                "zhang san", 20, "male");
//            shared_ptr<Connection> sp = cp->getConnection();
//            sp->update(sql);*/
//            Connection conn;
//            char sql[1024] = { 0 };
//            sprintf(sql, "insert into user(name,id) values('%s',%d)",
//                    "zhang san", 20);
//            conn.connect("127.0.0.1", 3306, "root", "", "user1");
//            conn.update(sql);
//        }
//    });
//
//
//    thread t4([]() {
//        //ConnectionPool *cp = ConnectionPool::getConnectionPool();
//        for (int i = 0; i < 2500; ++i)
//        {
//            /*char sql[1024] = { 0 };
//            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
//                "zhang san", 20, "male");
//            shared_ptr<Connection> sp = cp->getConnection();
//            sp->update(sql);*/
//            Connection conn;
//            char sql[1024] = { 0 };
//            sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
//                    "zhang san", 20, "male");
//            conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
//            conn.update(sql);
//        }
//    });

    t1.join();
//    t2.join();
//    t3.join();
//    t4.join();

    clock_t end = clock();
    cout << (end - begin) << "ms" << endl;

//    conn.update(sql);
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