#include <bits/stdc++.h>
using namespace std;

class Member
{
private:
    string mobile;
    string password;
    string username;
    int balance;

public:
    string getUsername() const { return username; }
    void setUsername(const string name)
    {
        username = name;
    }

    string getPassword() const { return password; }
    void setPassword(const string pass)
    {
        password = pass;
    }

    string getMobile() const { return mobile; }
    void setMobile(const string mob)
    {
        mobile = mob;
    }
    int getBalance() const { return balance; }
    void setBalance(int amount) { balance = amount; }

    Member(const string user, const string pass, const string mob, int bal)
    {
        username = user;
        password = pass;
        mobile = mob;
        balance = bal;
    };
};

class History
{
private:
    static int tranxactionId;
    string description;
    int amount;
    int balance;

public:
    void SaveToFile()
    {
        ofstream history("History.txt", ios::app);
        history << tranxactionId << "," << description << "," << amount << "," << balance << "\n";
        history.close();
    }

    History(string des, int am, int bal)
    {
        tranxactionId++;
        description = des;
        amount = am;
        balance = bal;
    }
};
int History::tranxactionId = 000;

Member *newMember;
History *newHistory;

bool RegisterUser();
bool LoginUser();
void UserMenu();
bool ConfirmOTP();
bool Confirmation();
bool IsNumberSaved(string number);
void UpdateSavedInfo();

string savedUserInfo, savedHistoryInfo,
    username, savedUsername, password, savedPassword, mobile, savedMobile;
int bal;
int choice;

int main()
{
    while (1)
    {
        cout << "\n------MyCash------\n1. Register\n2. Login\n3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
            // redirects to the user menu on successfull registration
            if (RegisterUser())
            { // creates a new pointer to a newly created member object
                newMember = new Member(username, password, mobile, 0);
                cout << "Registration successful. Welcome " << username << "!!!\n";
                UserMenu();
            }

            break;

        case 2:
            // redirects to the user menu on successfull login
            if (LoginUser())
            {
                newMember = new Member(savedUsername, savedPassword, savedMobile, bal);
                cout << "Login successful. Welcome " << savedUsername << "!!!\n";
                UserMenu();
            }
            else
                cout << "Invalid login!!!" << endl;

            break;

        case 3:
            cout << "\nExiting the program...\n";
            return 0;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}

bool RegisterUser()
{
    // Prompt for mobile number and password
    cout << "Enter mobile number (11 digit): ";
    cin >> mobile;
    // if (mobile.length() < 11)
    // {
    //     cout << "Mobile number must be 11 character long!!!\n";
    //     return false;
    // }
    if (mobile[0] != '0' || mobile[1] != '1')
    {
        cout << "Invalid mobile number. It should start with '01'.\n";
        return false;
    }
    // checks if this mobile number is saved in the file
    if (IsNumberSaved(mobile))
    {
        cout << "Member already exists!!!\n";
        return false;
    }

    cout << "Enter name (no space): ";
    cin >> username;
    cout << "Enter pin (5 digit): ";
    cin >> password;
    if (password.length() < 5)
    {
        cout << "Password must be 5 character long!!!\n";
        return false;
    }

    // OTP confirmation
    if (!ConfirmOTP())
        return false;

    // Open the file in append mode
    ofstream file("SavedMembers.txt", ios::app);
    file << username << "," << password << "," << mobile << "," << 0.00 << "\n";
    file.close();

    return true;
}

bool LoginUser()
{
    bool isUserMatch = false;

    // Prompt for mobile nember and password
    cout << "Enter mobile number (11 digit): ";
    cin >> mobile;
    cout << "Enter pin (5 digit): ";
    cin >> password;

    // Open the file in append mode
    ifstream file("SavedMembers.txt", ios::app);

    // Read each line and check for matching username and password
    while (getline(file, savedUserInfo))
    {
        if (savedUserInfo.find(mobile) != std::string::npos)
        {
            stringstream ss(savedUserInfo);
            getline(ss, savedUsername, ',');
            getline(ss, savedPassword, ',');
            getline(ss, savedMobile, ',');

            if (savedMobile == mobile && savedPassword == password)
            {
                ss >> bal;
                isUserMatch = true;
                break;
            }
        }
    }
    file.close();
    return isUserMatch;
}

void UserMenu()
{
    while (1)
    {
        cout << "\n------MyCash Menu------\n1. Update Me\n2. Remove Me\n3. Send Money\n4. Cash in\n5. Cash out \n6. Pay bill\n7. Check balance\n8. History\n9. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice)
        {
        case 1:
        {
            string newPassword, newUsername;
            cout << "Enter new name: ";
            cin >> newUsername;
            cout << "Enter new pin: ";
            cin >> newPassword;

            if (!ConfirmOTP())
                break;

            newMember->setUsername(newUsername);
            newMember->setPassword(newPassword);

            UpdateSavedInfo();
            cout << "Information updated successfully...\n";
            break;
        }
        case 2:
        {
            if (!ConfirmOTP())
                break;

            ifstream original("SavedMembers.txt", ios::app);
            ofstream destination("temp.txt", ios::app);

            while (getline(original, savedUserInfo))
            {
                if (savedUserInfo.find(newMember->getMobile()) != std::string::npos)
                    cout << "Information deleted successfully...\n";
                else
                    destination << savedUserInfo << "\n";
            }
            original.close();
            destination.close();
            remove("SavedMembers.txt");
            rename("temp.txt", "SavedMembers.txt");
            return;
        }
        case 3:
        {
            string receiverMobileNumber;
            cout << "Enter receiver mobile no. (11 digit): ";
            cin >> receiverMobileNumber;
            if (receiverMobileNumber == newMember->getMobile())
            {
                cout << "You cannot send money to yourself!!!\n";
                break;
            }

            // Check if receiver exists
            if (!IsNumberSaved(receiverMobileNumber))
            {
                cout << "Receiver not found!!!\n";
                break;
            }

            int sendMoneyAmount;
            cout << "Enter amount: ";
            cin >> sendMoneyAmount;
            if (newMember->getBalance() < sendMoneyAmount)
            {
                cout << "Insufficient fund!!!\n";
                break;
            }

            cout << "Sending " << sendMoneyAmount << " to " << receiverMobileNumber << endl;
            if (!Confirmation())
                break;
            if (!ConfirmOTP())
                break;

            ifstream original("SavedMembers.txt", ios::app);
            ofstream destination("temp.txt", ios::app);
            while (getline(original, savedUserInfo))
            {
                stringstream ss(savedUserInfo);
                getline(ss, savedUsername, ',');
                getline(ss, savedPassword, ',');
                getline(ss, savedMobile, ',');
                ss >> bal;

                // If this is the receiver's account, add money
                if (savedMobile == receiverMobileNumber)
                {
                    bal += sendMoneyAmount;
                    destination << savedUsername << "," << savedPassword << "," << savedMobile << "," << bal << "\n";
                }
                // If this is the sender's account, subtract money
                else if (savedMobile == newMember->getMobile())
                {
                    bal -= sendMoneyAmount;
                    newMember->setBalance(bal);
                    destination << savedUsername << "," << savedPassword << "," << savedMobile << "," << bal << "\n";
                }
                // For all other accounts, keep as is
                else
                    destination << savedUserInfo << "\n";
            }
            original.close();
            destination.close();
            remove("SavedMembers.txt");
            rename("temp.txt", "SavedMembers.txt");

            History h("Send Money", sendMoneyAmount, newMember->getBalance());
            h.SaveToFile();

            cout << "Transaction successful. " << sendMoneyAmount << "$ sent to " << receiverMobileNumber << "\n";
            break;
        }
        case 4:
        {
            int cashInAmount;
            cout << "Enter amount: ";
            cin >> cashInAmount;
            cout << "Cash in " << cashInAmount << " $" << endl;

            if (!Confirmation())
                break;

            newMember->setBalance(newMember->getBalance() + cashInAmount);
            UpdateSavedInfo();

            History h("Cash In", cashInAmount, newMember->getBalance());
            h.SaveToFile();

            cout << "Cash in successfull...\n";
            break;
        }
        case 5:
        {
            int cashOutAmount;
            cout << "Enter amount: ";
            cin >> cashOutAmount;
            cout << "Cash out " << cashOutAmount << " $" << endl;

            if (!Confirmation())
                break;

            if (!ConfirmOTP())
                break;

            newMember->setBalance(newMember->getBalance() - cashOutAmount);
            UpdateSavedInfo();

            History h("Cash Out", cashOutAmount, newMember->getBalance());
            h.SaveToFile();

            cout << "Cash out successfull...\n";

            break;
        }
        case 6:
        {
            int billAmount, billChoice;
            cout << "Enter Bill Type (Gas/Electricity/Water/Internet-1/2/3/4): ";
            cin >> billChoice;
            switch (billChoice)
            {
            case 1:
                cout << "Your Gas Bill: ";
                break;
            case 2:
                cout << "Your Electricity Bill: ";
                break;
            case 3:
                cout << "Your Water Bill: ";
                break;
            case 4:
                cout << "Your Internet Bill: ";
                break;
            default:
                break;
            }
            cin >> billAmount;
            if (!Confirmation())
                break;
            if (!ConfirmOTP())
                break;

            newMember->setBalance(newMember->getBalance() - billAmount);
            UpdateSavedInfo();

            History h("Pay Bill", billAmount, newMember->getBalance());
            h.SaveToFile();

            cout << "Bill Payment is Successful...\n";
            break;
        }
        case 7:
            cout << "Current Balance: " << newMember->getBalance() << "$\n";
            break;
        case 8:
        {
            cout << "TranId  Description\tAmount\tBalance\n";

            ifstream history("History.txt", ios::app);
            while (getline(history, savedHistoryInfo))
            {
                string tranId, desc, amt, bal;

                stringstream ss(savedHistoryInfo);
                getline(ss, tranId, ',');
                getline(ss, desc, ',');
                getline(ss, amt, ',');
                getline(ss, bal, ',');
                cout << tranId << "\t" << desc << "\t" << amt << "\t" << bal << endl;
            }
            history.close();
            break;
        }
        case 9:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}

bool ConfirmOTP()
{
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(0, 9999);
    int pin = dist(mt), userOTP;
    cout << "MyCash OTP " << pin << endl;
    cout << "Enter OTP: ";
    cin >> userOTP;
    if (pin == userOTP)
        return true;
    else
        return false;
}

bool Confirmation()
{
    char c;
    cout << "Are you sure (y/n): ";
    cin >> c;
    switch (c)
    {
    case 'y':
        return true;
        break;
    case 'n':
        return false;
        break;
    default:
        cout << "Invalid input!!!" << endl;
        break;
    }
}

bool IsNumberSaved(string number)
{
    bool isMatch = false;

    // Open the file in append mode
    ifstream file("SavedMembers.txt");

    // Read each line and check for matching mobile number
    while (getline(file, savedUserInfo))
    {
        if (savedUserInfo.find(number) != std::string::npos)
        {
            stringstream ss(savedUserInfo);
            getline(ss, savedUsername, ',');
            getline(ss, savedPassword, ',');
            getline(ss, savedMobile, ',');

            if (savedMobile == number)
            {
                isMatch = true;
                break;
            }
        }
    }
    file.close();

    return isMatch;
}

void UpdateSavedInfo()
{
    ifstream original("SavedMembers.txt", ios::app);
    ofstream destination("temp.txt", ios::app);

    while (getline(original, savedUserInfo))
    {
        if (savedUserInfo.find(newMember->getMobile()) != std::string::npos)
        {
            destination << newMember->getUsername() << "," << newMember->getPassword() << "," << newMember->getMobile() << "," << newMember->getBalance() << "\n";
        }
        else
            destination << savedUserInfo << "\n";
    }
    original.close();
    destination.close();
    remove("SavedMembers.txt");
    rename("temp.txt", "SavedMembers.txt");
}
