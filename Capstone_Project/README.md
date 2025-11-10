# 🧮 Mini In-Memory Columnar SQL Engine

A lightweight analytical SQL engine written in **C**, designed to demonstrate a simple **in-memory columnar data model** with **vectorized execution** for aggregate operations like 
`SUM`,
`AVG`,
`COUNT`,
`MIN`,
`MAX`.  
The project uses **Flex** and **Bison** to parse SQL-like commands for data insertion and aggregation.

---
## 📘 About

This project implements a **mini in-memory columnar store** that keeps data entirely in memory.  
Unlike traditional row-based storage, columnar databases store values column by column — allowing for faster analytics and aggregate computation over large datasets.

The SQL engine accepts simple queries such as:
- `INSERT INTO Employee VALUES (50000, 30, 5);`
- `SELECT SUM(salary) FROM Employee;`

---

## ✨ Features

- 🧱 In-memory **columnar storage model**  
- ⚡ **Vectorized execution** for efficient aggregate operations  
- 🧮 Supports `SUM()`,`AVG()`,`COUNT()`,`MIN()`,`MAX()` aggregations  
- 🗃️ Basic **SQL-like query parser** using **Flex** and **Bison**  
- 💾 **Dynamic column allocation** with memory management  
- 🧰 Extensible structure for adding new aggregates or operators

---

## 🧱 Architecture

```text
+------------------------------+
|     main.c (CLI Engine)      |
+--------------+---------------+
               |
+--------------v----------------+
|  Columnar Engine (columnar.c) |
|  - Column structure           |
|  - Table structure            |
|  - Aggregate functions        |
|   (SUM,AVG,COUNT,MIN,MAX)     |
+-------------------------------+
			   |
+--------------v---------------+
|     SQL Parser (Flex+Bison)  |
|  - lexer.l                   |
|  - parser.y                  |
+--------------+---------------+

---

## 🚀 Usage

bhargavi@DESKTOP-QSLAT5P:~/IITM_TRAINING/Capstone_Project$ bison -d mini_sql.y
bhargavi@DESKTOP-QSLAT5P:~/IITM_TRAINING/Capstone_Project$ flex mini_sql.l
bhargavi@DESKTOP-QSLAT5P:~/IITM_TRAINING/Capstone_Project$ gcc columnar.c mini_sql.tab.c lex.yy.c -o mini_sql -lfl

bhargavi@DESKTOP-QSLAT5P:~/IITM_TRAINING/Capstone_Project$
bhargavi@DESKTOP-QSLAT5P:~/IITM_TRAINING/Capstone_Project$ ./mini_sql

Mini Columnar SQL Engine (Flex+Bison)

Example commands:
  INSERT INTO Employee VALUES (50000, 30, 5);
  SELECT SUM(salary) FROM Employee;
Type Ctrl+D to quit.

INSERT INTO Employee VALUES (50000, 30, 5);
Inserted row into Employee.

INSERT INTO Employee VALUES (60000, 35, 8);
Inserted row into Employee.

INSERT INTO Employee VALUES (30000, 25, 1);
Inserted row into Employee.

INSERT INTO Employee VALUES (40000, 28, 3);
Inserted row into Employee.

SELECT SUM(salary) FROM Employee;
SUM(salary) = 180000

SELECT AVG(age) FROM Employee;
AVG(age) = 29.50

SELECT COUNT(experience) FROM Employee;
COUNT(experience) = 4

SELECT MAX(age) FROM Employee;
MAX(age) = 35

SELECT MIN(salary) FROM Employee;
MIN(salary) = 30000

Shutting down...
Memory freed. Exiting.

---