#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct BankAccount
{
	int balance = 0;
	int overdraft_limit = -500;

	void deposit(int amount)
	{
		balance += amount;
		cout << "deposited " << amount << ", balance now " << balance << "\n";
	}

	void withdraw(int amount)
	{
		if (balance - amount >= overdraft_limit)
		{
			balance -= amount;
			cout << "withdrew " << amount << ", balance now " << balance << "\n";
		}
	}
};

struct Command
{
	BankAccount& account;
	enum Action{deposit, withdraw} action;
	int amount;

	Command(BankAccount& account, const Action action, const int amount) : account(account), action(action), amount(amount)
	{

	}

	void call() const
	{
		switch (action)
		{
		case deposit:
			account.deposit(amount);
			break;
		case withdraw:
			account.withdraw(amount);
			break;
		default: break;
		}
	}

	void undo() const
	{
		switch (action)
		{
		case withdraw:
			account.deposit(amount);
			break;
		case deposit:
			account.withdraw(amount);
			break;
		default: break;
		}
	}
};

int main()
{
	BankAccount ba;
	vector<Command> commands
	{
		Command{ba, Command::deposit, 100},
		Command{ba, Command::withdraw, 200}
	};

	for (auto& cmd : commands)
	{
		cmd.call();
	}

	cout << ba.balance << endl;

	for (auto& cmd : commands)
	{
		cmd.undo();
	}

	cout << ba.balance << endl;
	getchar();
	return 0;
}