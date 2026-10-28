#pragma once
#include <utility>
#include <vector>

// Works out which notes to hand out for a withdrawal.
class Dispenser {
public:
    // Denominations, largest first. Greedy is optimal for this set
    // (each note value fits the ones below it in a "canonical" way).
    static constexpr int kNotes[] = {500, 200, 100};
    static constexpr int kSmallest = 100;

    // Returns {denomination, count} pairs, e.g. 2800 -> {500,5} {200,1} {100,1}.
    // Returns an empty vector if the amount can't be paid out.
    static std::vector<std::pair<int, int>> notesFor(long amount) {
        std::vector<std::pair<int, int>> result;
        if (amount <= 0 || amount % kSmallest != 0) return result;

        for (int note : kNotes) {
            int count = static_cast<int>(amount / note);
            if (count > 0) {
                result.push_back({note, count});
                amount -= static_cast<long>(count) * note;
            }
        }
        return result;
    }
};
