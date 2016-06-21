# aprmd5_udf

### Introduction
For some reason i need to generate and validate apache's MD5 password and eventually store them in mysql. 
This can be useful to use php+mysql instead of .htaccess :)

### Building

- Connect to mysql and give this query, take note of Value result:
```sql
mysql> show variables like 'plugin_dir';
+---------------+------------------------+
| Variable_name | Value                  |
+---------------+------------------------+
| plugin_dir    | /usr/lib/mysql/plugin/ |
+---------------+------------------------+
1 row in set (0.00 sec)
```
- Got to your bash, login as root and export the following variables:

```sh
# export APU_LIBS="`apu-1-config --includes --ldflags --link-ld --libs`"
# export APR_LIBS="`apr-1-config --cflags --cppflags --includes --ldflags --link-ld --libs`"
# export MYSQL_PLUGIN_DIR='/usr/lib/mysql/plugin/' # replace here with the variable value
# gcc $(mysql_config --cflags) -shared -fPIC -o $MYSQL_PLUGIN_DIR/aprmd5_validate_udf.so aprmd5_validate_udf.c $APR_LIBS $APU_LIBS 
# gcc $(mysql_config --cflags) -shared -fPIC -o $MYSQL_PLUGIN_DIR/aprmd5_udf.so aprmd5_udf.c $APR_LIBS $APU_LIBS
# chmod -x $MYSQL_PLUGIN_DIR/aprmd5_validate_udf.so
# chmod -x $MYSQL_PLUGIN_DIR/aprmd5_udf.so
```

### Installation
Connect to your mysql and send these two commands:
```sql
mysql> CREATE FUNCTION aprmd5_udf RETURNS STRING SONAME 'aprmd5_udf.so';
Query OK, 0 rows affected (0.00 sec)
mysql> CREATE FUNCTION aprmd5_validate_udf RETURNS INTEGER SONAME 'aprmd5_validate_udf.so';
Query OK, 0 rows affected (0.00 sec)
```

### Test
From inside your mysql, test the newly created functions:
```sql
mysql> SELECT aprmd5_udf("testing");
+---------------------------------------+
| aprmd5_udf("testing")                 |
+---------------------------------------+
| $apr1$QB0PVaVm$bsqSqNTLXMEgCl6nGfxhF. |
+---------------------------------------+
1 row in set (0.00 sec)

mysql> SELECT aprmd5_validate_udf("testing","$apr1$QB0PVaVm$bsqSqNTLXMEgCl6nGfxhF.") AS `check`;
+-------+
| check |
+-------+
|     1 |
+-------+
1 row in set (0.00 sec)

mysql> SELECT aprmd5_validate_udf("testin","$apr1$QB0PVaVm$bsqSqNTLXMEgCl6nGfxhF.") AS `check`;
+-------+
| check |
+-------+
|     0 |
+-------+
1 row in set (0.00 sec)
```

### How to remove
Connect to your mysql and send these two commands:
```sql
mysql> DROP FUNCTION IF EXISTS aprmd5_validate_udf;
Query OK, 0 rows affected (0.00 sec)

mysql> DROP FUNCTION IF EXISTS aprmd5_udf;
Query OK, 0 rows affected (0.00 sec)
```

From your bash, login as root and remove the two shared object files:
```sh
# export MYSQL_PLUGIN_DIR='/usr/lib/mysql/plugin/' # replace here with the variable value
# rm $MYSQL_PLUGIN_DIR/aprmd5_validate_udf.so
# rm $MYSQL_PLUGIN_DIR/aprmd5_udf.so
```

### Reference
Here some links where i took informations and some of the code:

- http://blog.loftdigital.com/blog/how-to-write-mysql-functions-in-c
- http://datapile.coffeecrew.org/blog/2010/11/02/programming-with-the-apr-using-md5/
- http://dev.mysql.com/doc/refman/5.7/en/adding-functions.html
- https://apr.apache.org/docs/apr/2.0/group___a_p_r___m_d5.html

### TODO
- Testing :)
- A Makefile can be a good idea !!!

