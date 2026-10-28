#include "ATM.h"

int main() {
    // Demo accounts: card number, holder, PIN, opening balance (Rs).
    ATM atm({
        Account("1111", "Asha", "1234", 25000),
        Account("2222", "Ravi", "4321", 5000),
    });
    atm.run();
    return 0;
}
