#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "Account.h"
#include "Dispenser.h"

// Every screen of the ATM is one state. Each state has one handler that
// does its work and decides which state comes next.
enum class State { Idle, PinEntry, Menu, Withdraw, Deposit, Exit };

class ATM {
public:
    explicit ATM(std::vector<Account> accounts) : accounts_(std::move(accounts)) {}

    void run() {
        std::cout << "=== Welcome to the C++ ATM ===\n";
        while (state_ != State::Exit) {
            switch (state_) {
                case State::Idle:     onIdle();     break;
                case State::PinEntry: onPinEntry(); break;
                case State::Menu:     onMenu();     break;
                case State::Withdraw: onWithdraw(); break;
                case State::Deposit:  onDeposit();  break;
                case State::Exit:                   break;
            }
        }
        std::cout << "Goodbye!\n";
    }

private:
    static constexpr int kMaxAttempts = 3;
    static constexpr long kWithdrawLimit = 20000;

    std::vector<Account> accounts_;
    Account* current_ = nullptr;
    State state_ = State::Idle;
    int attempts_ = 0;

    // Reads a line; if input ends (Ctrl+D / Ctrl+Z) shut the ATM down cleanly.
    bool readLine(const std::string& prompt, std::string& out) {
        std::cout << prompt;
        if (!std::getline(std::cin, out)) {
            state_ = State::Exit;
            return false;
        }
        return true;
    }

    static bool parseAmount(const std::string& s, long& out) {
        try {
            std::size_t pos = 0;
            out = std::stol(s, &pos);
            return pos == s.size() && out > 0;
        } catch (...) {
            return false;
        }
    }

    // ---- States ----

    void onIdle() {
        std::string card;
        if (!readLine("\nInsert card (enter card number, 'q' to quit): ", card)) return;
        if (card == "q") { state_ = State::Exit; return; }

        for (auto& acc : accounts_) {
            if (acc.cardNo() != card) continue;
            if (acc.isLocked()) {
                std::cout << "This card is locked. Please contact your bank.\n";
                return;
            }
            current_ = &acc;
            attempts_ = 0;
            state_ = State::PinEntry;
            return;
        }
        std::cout << "Card not recognised.\n";
    }

    void onPinEntry() {
        std::string pin;
        if (!readLine("Enter PIN: ", pin)) return;

        if (current_->checkPin(pin)) {
            std::cout << "\nHello, " << current_->holder() << "!\n";
            state_ = State::Menu;
            return;
        }

        ++attempts_;
        if (attempts_ >= kMaxAttempts) {
            current_->lock();
            std::cout << "Too many wrong attempts. Card locked and retained.\n";
            current_ = nullptr;
            state_ = State::Idle;
        } else {
            std::cout << "Wrong PIN. " << (kMaxAttempts - attempts_) << " attempt(s) left.\n";
        }
    }

    void onMenu() {
        std::string choice;
        std::cout << "\n1. Check balance\n2. Withdraw cash\n3. Deposit cash\n4. Eject card\n";
        if (!readLine("Choose an option: ", choice)) return;

        if (choice == "1") {
            std::cout << "Balance: Rs " << current_->balance() << "\n";
        } else if (choice == "2") {
            state_ = State::Withdraw;
        } else if (choice == "3") {
            state_ = State::Deposit;
        } else if (choice == "4") {
            std::cout << "Card ejected. Please take your card.\n";
            current_ = nullptr;
            state_ = State::Idle;
        } else {
            std::cout << "Invalid option.\n";
        }
    }

    void onWithdraw() {
        std::string input;
        if (!readLine("Amount to withdraw (multiple of 100): Rs ", input)) return;

        long amount = 0;
        if (!parseAmount(input, amount)) {
            std::cout << "Please enter a valid positive number.\n";
        } else if (amount > kWithdrawLimit) {
            std::cout << "Limit per transaction is Rs " << kWithdrawLimit << ".\n";
        } else if (Dispenser::notesFor(amount).empty()) {
            std::cout << "Amount must be a multiple of " << Dispenser::kSmallest << ".\n";
        } else if (!current_->withdraw(amount)) {
            std::cout << "Insufficient balance.\n";
        } else {
            std::cout << "Dispensing:\n";
            for (auto [note, count] : Dispenser::notesFor(amount)) {
                std::cout << "  " << count << " x Rs " << note << "\n";
            }
            std::cout << "New balance: Rs " << current_->balance() << "\n";
        }
        state_ = State::Menu;
    }

    void onDeposit() {
        std::string input;
        if (!readLine("Amount to deposit: Rs ", input)) return;

        long amount = 0;
        if (parseAmount(input, amount) && current_->deposit(amount)) {
            std::cout << "Deposited. New balance: Rs " << current_->balance() << "\n";
        } else {
            std::cout << "Please enter a valid positive number.\n";
        }
        state_ = State::Menu;
    }
};
