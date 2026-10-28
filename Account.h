#pragma once
#include <functional>
#include <string>
#include <utility>

// A bank account tied to one card. The PIN is stored as a hash, never as plain text.
// (std::hash is fine for a demo; a real system would use bcrypt/argon2 with a salt.)
class Account {
public:
    Account(std::string cardNo, std::string holder, const std::string& pin, long balance)
        : cardNo_(std::move(cardNo)),
          holder_(std::move(holder)),
          pinHash_(hashPin(pin)),
          balance_(balance) {}

    const std::string& cardNo() const { return cardNo_; }
    const std::string& holder() const { return holder_; }
    long balance() const { return balance_; }
    bool isLocked() const { return locked_; }

    bool checkPin(const std::string& pin) const { return hashPin(pin) == pinHash_; }
    void lock() { locked_ = true; }

    bool withdraw(long amount) {
        if (amount <= 0 || amount > balance_) return false;
        balance_ -= amount;
        return true;
    }

    bool deposit(long amount) {
        if (amount <= 0) return false;
        balance_ += amount;
        return true;
    }

private:
    static std::size_t hashPin(const std::string& pin) {
        return std::hash<std::string>{}(pin);
    }

    std::string cardNo_;
    std::string holder_;
    std::size_t pinHash_;
    long balance_;
    bool locked_ = false;
};
