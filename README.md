# C++ ATM Simulator

A console ATM written in modern C++ (C++17). The machine is modelled as a **state machine**, and cash withdrawals are paid out in the **smallest number of notes**.

## Features

- Card insertion, PIN entry, and a menu (balance, withdraw, deposit, eject)
- **3-attempt PIN lockout**: a card that fails 3 times is locked for good
- PINs are stored as hashes, not plain text
- Withdrawals are split into the fewest notes, e.g. Rs 2800 becomes 5 x 500 + 1 x 200 + 1 x 100
- Input validation: invalid amounts, non-multiples of 100, per-transaction limit, insufficient balance

## How it works

The ATM moves between states, and each state has its own handler:

```
Idle --card--> PinEntry --correct PIN--> Menu <--> Withdraw / Deposit
  ^               |                        |
  |  3 wrong PINs |                        |
  +----(locked)---+------ eject card ------+
```

| File | Purpose |
|------|---------|
| `Account.h` | Account data, hashed PIN, balance, lock flag |
| `Dispenser.h` | Greedy algorithm that picks the notes for a withdrawal |
| `ATM.h` | The state machine (one handler per state) |
| `main.cpp` | Sets up demo accounts and starts the ATM |

## Build and run

```bash
g++ -std=c++17 -Wall -Wextra main.cpp -o atm
./atm
```

Demo cards: `1111` (PIN `1234`) and `2222` (PIN `4321`).

## Ideas for improvement

- Save accounts and transactions to a file
- Track the ATM's own cash inventory per note type
- Add a mini-statement (last 5 transactions)
- Add a PIN change option
- Write unit tests for `Dispenser`
