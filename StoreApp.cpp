#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <stdexcept>
#include <random>
#include <string>
#include <chrono>


using namespace std;

enum class PaymentMethod { Cash, Card, Cancel };


// Define a struct to represent an item
struct Item {
    string name;
    double price;
    int quantity;
    string category;
};

// Define a struct to represent a user
struct User {
    string username;
    string passwordHash; // Store the hashed password
    string salt;
    vector<Item> purchaseHistory;
};

// Forward declarations
void viewCart(const vector<Item>& cart);
double calculateTotalPrice(const vector<Item>& cart);
PaymentMethod selectPaymentMethod();
void processPayment(PaymentMethod method);
void registerUser(map<string, User>& users);

std::string generateSalt(size_t length = 16) {
    const std::string chars =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string salt;
    for (size_t i = 0; i < length; ++i) {
        salt += chars[dist(generator)];
    }
    return salt;
}
string getCurrentTime() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

void checkout(vector<Item>& inventory, vector<Item>& cart, map<string, User>& users, const string& username) {
    if (cart.empty()) {
        cout << "It looks like your cart is empty. Let's add some items before checking out.\n";
        return;
    }

    cout << "Alright, let's review your cart before finalizing the purchase:\n";
    viewCart(cart);

    double total = calculateTotalPrice(cart);
    cout << "The total cost of your items is: $" << fixed << setprecision(2) << total << endl;

    // Apply discounts and taxes if necessary (for future implementation)

    char confirm;
    cout << "Are you ready to proceed with the checkout? (Y/N): ";
    cin >> confirm;

    if (tolower(confirm) == 'y') {
        cout << "Great! We're now processing your payment...\n";

        // Update inventory
        for (const auto& itemInCart : cart) {
            for (auto& itemInInventory : inventory) {
                if (itemInCart.name == itemInInventory.name) {
                    itemInInventory.quantity -= itemInCart.quantity;
                    break;
                }
            }
        }

        PaymentMethod method = selectPaymentMethod();
        if (method == PaymentMethod::Cancel) {
            cout << "Checkout cancelled.\n";
            return;
        }

        processPayment(method);


        // Update user's purchase history
        auto& user = users[username];
        user.purchaseHistory.insert(user.purchaseHistory.end(), cart.begin(), cart.end());

        // Clear the cart
        cart.clear();

        cout << "Your purchase was successful! Thank you for shopping with us.\n";
    }
    else {
        cout << "No problem, take your time. Let us know when you're ready to check out.\n";
    }
}

void logMessage(const string& message) {
    ofstream logFile("log.txt", ios::app);
    if (logFile) {
        logFile << getCurrentTime() << " - " << message << endl;
        logFile.close();
    }
    else {
        cerr << "Unable to open log file." << endl;
    }
}
void removeItemFromStore(vector<Item>& inventory) {
    if (inventory.empty()) {
        cout << "The store inventory is currently empty.\n";
        return;
    }

    cout << "Items in the store:\n";
    for (size_t i = 0; i < inventory.size(); ++i) {
        cout << i + 1 << ": " << inventory[i].name << " - Price: $" << inventory[i].price
            << " - Quantity: " << inventory[i].quantity << " - Category: " << inventory[i].category << endl;
    }

    cout << "Enter the number of the item to remove, or 0 to cancel: ";
    int choice;
    cin >> choice;

    if (choice == 0) {
        cout << "Item removal cancelled.\n";
        return;
    }

    if (choice < 1 || choice > static_cast<int>(inventory.size())) {
        cout << "Invalid selection. Please try again.\n";
        return;
    }

    inventory.erase(inventory.begin() + (choice - 1));
    cout << "Item removed successfully.\n";
}



// Function to calculate the SHA-256 hash of a string
string sha256(const string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input.c_str(), input.size());
    SHA256_Final(hash, &sha256);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

// Function to validate user credentials
bool validateCredentials(const string& username, const string& password, const map<string, User>& users) {
    auto it = users.find(username);
    if (it != users.end()) {
        string hashedInputPassword = sha256(it->second.salt+password);
        return it->second.passwordHash == hashedInputPassword;
    }
    return false;
}

// Function to calculate the total price of items in the cart
double calculateTotalPrice(const vector<Item>& cart) {
    double total = 0.0;
    for (const Item& item : cart) {
        total += item.price * item.quantity;
    }
    return total;
}

// Function to display items in a user's purchase history
void displayPurchaseHistory(const User& user) {
    cout << "Purchase History for User: " << user.username << endl;
    for (const Item& item : user.purchaseHistory) {
        cout << "Item: " << item.name << " - Price: $" << item.price << " - Quantity: " << item.quantity << " - Category: " << item.category << endl;
    }
    cout << "Total Purchase History Price: $" << calculateTotalPrice(user.purchaseHistory) << endl;
}

// Function to save user data to a file
void saveUserData(const map<string, User>& users) {
    ofstream outFile("userdata.txt");
    if (outFile.is_open()) {
        for (const auto& entry : users) {
            const User& user = entry.second;
            outFile << user.username << " " << user.passwordHash << "\n";
            for (const Item& item : user.purchaseHistory) {
                outFile << item.name << " " << item.price << " " << item.quantity << " " << item.category << "\n";
            }
        }
        outFile.close();
    } else {
        cerr << "Unable to save user data." << endl;
    }
}

// Function to load user data from a file
void loadUserData(map<string, User>& users) {
    ifstream inFile("userdata.txt");
    if (inFile.is_open()) {
        string line;
        while (getline(inFile, line)) {
            User user;
            user.username = line;
            getline(inFile, user.passwordHash);
            while (getline(inFile, line) && !line.empty()) {
                Item item;
                istringstream iss(line);
                iss >> item.name >> item.price >> item.quantity >> item.category;
                user.purchaseHistory.push_back(item);
            }
            users[user.username] = user;
        }
        inFile.close();
    } else {
        cerr << "Unable to load user data." << endl;
    }
}
void logError(const string& error) {
    ofstream logFile("error_log.txt", ios::app);
    if (logFile.is_open()) {
        logFile << error << "\n";
        logFile.close();
    }
    else {
        cerr << "Unable to open log file." << endl;
    }
}


void displayMainMenu() {
    cout << "\nMain Menu:\n";
    cout << "1. Add an item to the store\n";
    cout << "2. View cart\n";
    cout << "3. Register\n";
    cout << "4. Exit\n";
    cout << "Enter your choice: ";
}
int getValidatedInput(int minOption, int maxOption) {
    int choice;
    while (true) {
        cin >> choice;
        if (cin.fail() || choice < minOption || choice > maxOption) {
            cin.clear(); // Clear error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Skip to the next newline
            cout << "Invalid input, please enter a number between " << minOption << " and " << maxOption << ": ";
        }
        else {
            return choice;
        }
    }
}
void updateItemPrice(vector<Item>& inventory) {
    if (inventory.empty()) {
        cout << "The inventory is currently empty.\n";
        return;
    }

    cout << "Select an item to update its price:\n";
    for (size_t i = 0; i < inventory.size(); ++i) {
        cout << i + 1 << ": " << inventory[i].name << " (Current Price: $" << inventory[i].price << ")\n";
    }

    int choice;
    cout << "Enter item number: ";
    cin >> choice;

    if (choice < 1 || choice > static_cast<int>(inventory.size())) {
        cout << "Invalid selection.\n";
        return;
    }

    double newPrice;
    cout << "Enter new price for " << inventory[choice - 1].name << ": ";
    cin >> newPrice;

    if (newPrice < 0) {
        cout << "Price cannot be negative.\n";
        return;
    }

    inventory[choice - 1].price = newPrice;
    cout << "Price updated successfully.\n";
}

void addItemToStore(vector<Item>& inventory) {
    Item newItem;
    cout << "Enter item name: ";
    cin.ignore(); // Clear the input buffer
    getline(cin, newItem.name);
    cout << "Enter item price: ";
    cin >> newItem.price;
    cout << "Enter item quantity: ";
    cin >> newItem.quantity;
    cout << "Enter item category: ";
    cin.ignore();
    getline(cin, newItem.category);

    inventory.push_back(newItem);
    cout << "Item successfully added to the store.\n";
}
void viewCart(const vector<Item>& cart) {
    if (cart.empty()) {
        cout << "Your cart is empty.\n";
        return;
    }

    cout << "Items in your cart:\n";
    for (const auto& item : cart) {
        cout << "Item: " << item.name << ", Price: $" << item.price
            << ", Quantity: " << item.quantity << ", Category: " << item.category << endl;
    }
}
void mainMenu() {
    vector<Item> inventory;
    vector<Item> cart;
    map<string, User> users;

    while (true) {
        displayMainMenu();
        int choice = getValidatedInput(1, 4);

        switch (choice) {
        case 1:
            addItemToStore(inventory);
            break;
        case 2:
            viewCart(cart);
            break;
        case 3:
            registerUser(users);
            break;
        case 4:
            cout << "Exiting the application.\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

// Function to register a new user
void registerUser(map<string, User>& users) {
    string username, password;
    cout << "Enter a username: ";
    cin.ignore();
    getline(cin, username);

    // Check if the username already exists
    if (users.find(username) != users.end()) {
        cout << "Username already exists. Please choose a different username.\n";
        return;
    }

    cout << "Enter a password: ";
    getline(cin, password);
    string salt = generateSalt();
    // Hash the password before storing it
    string passwordHash = sha256(salt + password);

    // Create a new user and add them to the map
    User newUser;
    newUser.username = username;
    newUser.passwordHash = passwordHash;
    newUser.salt = salt;

    users[username] = newUser;

    cout << "Registration successful. You can now log in.\n";
}
void processPayment(PaymentMethod method) {
    switch (method) {
    case PaymentMethod::Cash:
        cout << "Processing cash payment...\n";
        // Simulate cash payment
        break;
    case PaymentMethod::Card:
        cout << "Processing card payment...\n";
        // Simulate card payment
        break;
    case PaymentMethod::Cancel:
        cout << "Payment cancelled.\n";
        break;
    }
}

// Function for the owner menu
void ownerMenu(vector<Item>& inventory) {
    char choice;
    do {
        cout << "Owner Menu:\n";
        cout << "1. Add a product to the store\n";
        cout << "2. Remove a product from the store\n";
        cout << "3. Display items in the store\n";
        cout << "4.update item price\n";
        cout << "5.Display items in the store\n";
        cout << "6.Quit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case '1': {
                // Allow the owner to add products to the store
                Item newItem;
                cout << "Enter item name: ";
                cin.ignore();
                getline(cin, newItem.name);
                cout << "Enter item price: ";
                cin >> newItem.price;
                cout << "Enter item quantity: ";
                cin >> newItem.quantity;
                cout << "Enter item category: ";
                cin.ignore();
                getline(cin, newItem.category);
                inventory.push_back(newItem);
                cout << "Product added to the store.\n";
                break;
            }
            // Add more owner-specific actions as needed
            case '2': {
                removeItemFromStore(inventory);
                break;
            }
            case '3': {
                cout << "Items in the store:\n";
                for (const Item& item : inventory) {
                    cout << "Item: " << item.name << " - Price: $" << item.price << " - Quantity: " << item.quantity << " - Category: " << item.category << endl;
                }
                break;
            }
            case '4': {
                updateItemPrice(inventory);
                break;
            }
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != '6');

}
PaymentMethod selectPaymentMethod() {
    cout << "\nSelect a payment method:\n";
    cout << "1. Cash\n";
    cout << "2. Card\n";
    cout << "3. Cancel Transaction\n";
    cout << "Enter your choice: ";

    int choice;
    cin >> choice;

    switch (choice) {
    case 1: return PaymentMethod::Cash;
    case 2: return PaymentMethod::Card;
    default: return PaymentMethod::Cancel;
    }
}

int main() {
    map<string, User> users; // Map to store user data (username -> User)
    vector<Item> inventory; // Vector to store items in the store
    vector<Item> cart;      // Vector to store items in the user's cart

    // Load user data from a file
    loadUserData(users);

    // Demo owner credentials. Replace with a per-deployment secret in production.
    const string ownerUsername = "owner";
    const string ownerPassword = sha256("admin");

    char choice;

    // Main program loop
    do {
        cout << "Options:\n";
        cout << "1. Add an item to the store\n";
        cout << "2. Display items in the store\n";
        cout << "3. Add an item to the cart\n";
        cout << "4. Display items in the cart\n";
        cout << "5. Calculate total price of items in the cart\n";
        cout << "6. Display purchase history\n";
        cout << "7. Log in as a user\n";
        cout << "8. Register an account\n";
        cout << "9. Log in as the owner\n"; // Added owner login option
        cout << "0. Quit\n";
        cout << "Enter your choice: ";

        try {
            cin >> choice;
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("invalid imput please enter a valid option.");
            }
            switch (choice) {
            case '1': {
                Item newItem;
                cout << "Enter item name: ";
                cin.ignore();
                getline(cin, newItem.name);
                cout << "Enter item price: ";
                cin >> newItem.price;
                cout << "Enter item quantity: ";
                cin >> newItem.quantity;
                cout << "Enter item category: ";
                cin.ignore();
                getline(cin, newItem.category);
                inventory.push_back(newItem);
                break;
            }
            case '2': {
                cout << "Items in the store:\n";
                for (const Item& item : inventory) {
                    cout << "Item: " << item.name << " - Price: $" << item.price << " - Quantity: " << item.quantity << " - Category: " << item.category << endl;
                }
                break;
            }
            case '3': {
                cout << "Items in the store:\n";
                for (int i = 0; i < inventory.size(); ++i) {
                    cout << i + 1 << ". " << inventory[i].name << " - Price: $" << inventory[i].price << " - Quantity: " << inventory[i].quantity << " - Category: " << inventory[i].category << endl;
                }
                int selection;
                cout << "Enter the number of the item to add to the cart: ";
                cin >> selection;
                if (selection >= 1 && selection <= inventory.size()) {
                    Item selectedItem = inventory[selection - 1];
                    int quantityToAdd;
                    cout << "Enter the quantity to add to the cart: ";
                    cin >> quantityToAdd;
                    if (quantityToAdd > 0 && quantityToAdd <= selectedItem.quantity) {
                        selectedItem.quantity = quantityToAdd;
                        cart.push_back(selectedItem);
                        cout << "Item added to the cart.\n";
                    }
                    else {
                        cout << "Invalid quantity. Please try again.\n";
                    }
                }
                else {
                    cout << "Invalid selection. Please try again.\n";
                }
                break;
            }
            case '4': {
                cout << "Items in the cart:\n";
                for (const Item& item : cart) {
                    cout << "Item: " << item.name << " - Price: $" << item.price << " - Quantity: " << item.quantity << " - Category: " << item.category << endl;
                }
                break;
            }
            case '5': {
                double total = calculateTotalPrice(cart);
                cout << "Total price of items in the cart: $" << total << endl;
                break;
            }
            case '6': {
                string username;
                cout << "Enter your username: ";
                cin.ignore();
                getline(cin, username);
                if (users.find(username) != users.end()) {
                    displayPurchaseHistory(users[username]);
                }
                else {
                    cout << "User not found.\n";
                }
                break;
            }
            case '7': {
                string username, password;
                cout << "Enter your username: ";
                cin.ignore();
                getline(cin, username);
                cout << "Enter your password: ";
                getline(cin, password);
                if (validateCredentials(username, password, users)) {
                    cout << "Login successful.\n";
                }
                else {
                    cout << "Invalid username or password. Please try again.\n";
                }
                break;
            }
            case '8': {
                registerUser(users);
                break;
            }
            case '9': {
                string username, password;
                cout << "Enter the owner username: ";
                cin.ignore();
                getline(cin, username);
                cout << "Enter the owner password: ";
                getline(cin, password);
                if (username == ownerUsername && sha256(password) == ownerPassword) {
                    ownerMenu(inventory); // Call the owner menu function
                }
                else {
                    cout << "Invalid owner credentials. Please try again.\n";
                }
                break;
            }
            case '0': {
                saveUserData(users);
                cout << "Exiting the program.\n";
                break;
            }
            default:
                cout << "Invalid choice. Please try again.\n";
            }
        }
        catch (const std::exception& e) {
            cerr << "Error: " << e.what() << endl;
        }

    } while (choice != '0');

    return 0;
}