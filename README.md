# Store Management Application

A command-line store management system built in C++ that supports user registration with password hashing, inventory management, shopping cart functionality, checkout with payment processing, and role-based access for store owners.

## Overview

This application simulates a retail store environment where customers can browse inventory, add items to a cart, and check out, while store owners have administrative access to manage products and pricing. The system includes user authentication with SHA-256 password hashing and salting for security.

## Purpose

Developed as a personal project to practice object-oriented design patterns in C++, file I/O for data persistence, and security fundamentals like password hashing. The project demonstrates how to build a complete, interactive application with multiple user roles and data management.

## Technologies Used

- **C++** - Core programming language
- **OpenSSL** - SHA-256 password hashing
- **STL (Standard Template Library)** - Vectors, maps, and string streams for data management
- **File I/O** - Persistent storage for user data and activity logging

## Features

- **User registration and login** - Account creation with SHA-256 hashed and salted passwords
- **Inventory management** - Add, remove, and update products in the store
- **Shopping cart** - Browse items, add to cart, and view cart contents
- **Checkout system** - Calculate totals and process payments (cash or card)
- **Owner menu** - Role-based access for store administrators to manage inventory
- **Purchase history** - Track and display past purchases per user
- **Data persistence** - User data saved to and loaded from files
- **Input validation** - Error handling for invalid inputs throughout the application
- **Activity logging** - Actions logged to file with timestamps

## How to Run

### Prerequisites
- C++ compiler with C++11 support (g++, clang++, or MSVC)
- OpenSSL development libraries

### Compile and Run

With the included Makefile (Linux / macOS):

```bash
make           # builds ./StoreApp
make run       # builds and launches it
make clean     # removes the binary and generated data files
```

Or directly with g++:

```bash
g++ -o StoreApp StoreApp.cpp -lssl -lcrypto -std=c++11
./StoreApp
```

On macOS with Homebrew OpenSSL:

```bash
g++ -std=c++11 -I$(brew --prefix openssl)/include -L$(brew --prefix openssl)/lib \
    -o StoreApp StoreApp.cpp -lssl -lcrypto
```

### Menu Options
```
1. Add an item to the store
2. Display items in the store
3. Add an item to the cart
4. Display items in the cart
5. Calculate total price of items in the cart
6. Display purchase history
7. Log in as a user
8. Register an account
9. Log in as the owner
10. Quit
```

## Example Usage

```
Enter your choice: 8
Enter a username: john
Enter a password: mypassword
Registration successful. You can now log in.

Enter your choice: 1
Enter item name: Laptop
Enter item price: 999.99
Enter item quantity: 5
Enter item category: Electronics
Item successfully added to the store.
```

## My Role

I designed and built this entire application as a personal project. I implemented the authentication system with salted SHA-256 hashing, designed the inventory and cart data structures, built the role-based menu system, and added file-based persistence for user data.

## Lessons Learned

- Practiced implementing secure password storage with salting and hashing in C++
- Learned to design multi-role CLI applications with structured menu navigation
- Gained experience with file I/O for data persistence and logging
- Improved error handling and input validation techniques in C++
