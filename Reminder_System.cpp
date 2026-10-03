#include <iostream>
#include <string>
#include <ctime>
#include <fstream>

using namespace std;

// Save all array data into a text file "tasks.txt"
void saveTasksToFile(string tasks[], string categories[], string priorities[],
                     int dueHours[], int dueMinutes[], int dueDays[], 
                     int dueMonths[], int dueYears[], int taskCount) {
    ofstream outFile("tasks.txt");

    if (!outFile) {
        cout << "[ERROR] Could not open file for saving!\n";
        return;
    }

    // First line stores total number of tasks
    outFile << taskCount << "\n";

    for (int i = 0; i < taskCount; i++) {
        outFile << categories[i] << "\n";
        outFile << tasks[i] << "\n";
        outFile << priorities[i] << "\n";
        outFile << dueHours[i] << " " << dueMinutes[i] << " "
                << dueDays[i] << " " << dueMonths[i] << " " << dueYears[i] << "\n";
    }

    outFile.close();
}

// Read saved data from "tasks.txt" back into the arrays on startup
void loadTasksFromFile(string tasks[], string categories[], string priorities[],
                       int dueHours[], int dueMinutes[], int dueDays[], 
                       int dueMonths[], int dueYears[], int &taskCount) {
    ifstream inFile("tasks.txt");

    if (!inFile) {
        // File doesn't exist yet (first time running program)
        return;
    }

    inFile >> taskCount;

    for (int i = 0; i < taskCount; i++) {
        inFile >> categories[i];
        inFile.ignore(); // Clear newline after string read
        getline(inFile, tasks[i]);
        inFile >> priorities[i];
        inFile >> dueHours[i] >> dueMinutes[i] >> dueDays[i] >> dueMonths[i] >> dueYears[i];
    }

    inFile.close();
    cout << "[SYSTEM] Loaded " << taskCount << " task(s) from 'tasks.txt'.\n";
}

// Returns true if the day, month, and year form a real calendar date
bool isValidDate(int day, int month, int year) {
    // Basic range checks
    if (year < 2026 || month < 1 || month > 12 || day < 1) {
        return false;
    }

    // Days allowed per month
    int maxDays = 31;

    switch (month) {
        case 4: case 6: case 9: case 11:
            maxDays = 30; // April, June, September, November have 30 days
            break;
        case 2:
            // February leap year check (divisible by 4)
            if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
                maxDays = 29;
            } else {
                maxDays = 28;
            }
            break;
        default:
            maxDays = 31; // Jan, Mar, May, Jul, Aug, Oct, Dec
            break;
    }

    return day <= maxDays;
}

// Returns true if hour is 0-23 and minute is 0-59
bool isValidTime(int hour, int minute) {
    return (hour >= 0 && hour <= 23) && (minute >= 0 && minute <= 59);
}

int main() {
    string tasks[100];
    string categories[100];
    string priorities[100];
    
    int dueDays[100];
    int dueMonths[100];
    int dueYears[100];
    int dueHours[100];
    int dueMinutes[100];

    int taskCount = 0;
    int choice = 0;

    // Loads task on start up
    loadTasksFromFile(tasks, categories, priorities, dueHours, dueMinutes, dueDays, dueMonths, dueYears, taskCount);

    while (choice != 4) {
        cout << "\n=== MAIN MENU ===\n";
        cout << "1. Add reminder\n";
        cout << "2. View reminders\n";
        cout << "3. Delete reminder\n";
        cout << "4. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                if (taskCount >= 100) {
                    cout << "List is full!\n";
                } else {
                    // Category Selection
                    cout << "\nSelect Category:\n";
                    cout << "1. University\n2. Personal\n3. Work\n4. Other\n";
                    cout << "Enter choice: ";
                    int catChoice;
                    cin >> catChoice;

                    switch (catChoice) {
                        case 1: categories[taskCount] = "University"; break;
                        case 2: categories[taskCount] = "Personal"; break;
                        case 3: categories[taskCount] = "Work"; break;
                        default: categories[taskCount] = "Other"; break;
                    }

                    cin.ignore(); // Clear newline buffer

                    // Task Name
                    cout << "Enter task name: ";
                    getline(cin, tasks[taskCount]);

                    // Input due time and date, and also checking if the input is valid
                    bool dateTimeValid = false;
                    do {
                        cout << "Enter Hour due (0-23): ";
                        cin >> dueHours[taskCount];
                        cout << "Enter Minute due (0-59): ";
                        cin >> dueMinutes[taskCount];

                        cout << "Enter Day due (1-31): ";
                        cin >> dueDays[taskCount];
                        cout << "Enter Month due (1-12): ";
                        cin >> dueMonths[taskCount];
                        cout << "Enter Year due: ";
                        cin >> dueYears[taskCount];
 
                        bool validTime = (dueHours[taskCount] >= 0 && dueHours[taskCount] <= 23) && 
                                         (dueMinutes[taskCount] >= 0 && dueMinutes[taskCount] <= 59);
                        bool validDate = isValidDate(dueDays[taskCount], dueMonths[taskCount], dueYears[taskCount]);
                    
                        dateTimeValid = validTime && validDate;
                    
                        if (!dateTimeValid) {
                            cout << "\nInvalid date or time input! Please enter valid values.\n\n";
                        }
                    } while (!dateTimeValid);

                    // Priority
                    cout << "Select Priority (1=Red, 2=Yellow, 3=Green): ";
                    int prioChoice;
                    cin >> prioChoice;

                    switch (prioChoice) {
                        case 1: priorities[taskCount] = "RED"; break;
                        case 2: priorities[taskCount] = "YELLOW"; break;
                        default: priorities[taskCount] = "GREEN"; break;
                    }

                    taskCount++;
                    // Saves task to file after adding
                    saveTasksToFile(tasks, categories, priorities, dueHours, dueMinutes, dueDays, dueMonths, dueYears, taskCount);
                    cout << "Reminder added and saved successfully!\n";
                }
                break;
            }

            case 2: {
                cout << "\n--- YOUR REMINDERS ---\n";
                if (taskCount == 0) {
                    cout << "No reminders found!\n";
                } else {
                    time_t now = time(nullptr);

                    for (int i = 0; i < taskCount; i++) {
                        tm targetTime = {};
                        targetTime.tm_mday = dueDays[i];
                        targetTime.tm_mon = dueMonths[i] - 1;   // 0-11 ( January = 0, etc)
                        targetTime.tm_year = dueYears[i] - 1900; // Years since 1900
                        targetTime.tm_hour = dueHours[i];
                        targetTime.tm_min = dueMinutes[i];
                        targetTime.tm_sec = 0;
                        targetTime.tm_isdst = -1;

                        time_t deadlineTimestamp = mktime(&targetTime);
                        double secondsLeft = difftime(deadlineTimestamp, now);

                        cout << i + 1 << ". [" << categories[i] << "] " 
                             << tasks[i] << " [" << priorities[i] << " Priority]\n";
                        
                        // Outputs the due time and date
                        cout << "   Due: " << dueHours[i] << ":" << (dueMinutes[i] < 10 ? "0" : "") << dueMinutes[i] 
                             << " on " << dueDays[i] << "-" << dueMonths[i] << "-" << dueYears[i];

                        // Time remaining calculation
                        if (secondsLeft < 0) {
                            cout << " [OVERDUE!]\n";
                        } else {
                            int totalHoursLeft = secondsLeft / 3600;
                            int daysLeft = totalHoursLeft / 24;
                            int remainingHours = totalHoursLeft % 24;

                            if (daysLeft > 0) {
                                cout << " (" << daysLeft << " days, " << remainingHours << " hrs left)\n";
                            } else {
                                cout << " (" << totalHoursLeft << " hrs left)\n";
                            }
                        }
                    }
                }
                break;
            }

            case 3: {
                if (taskCount == 0) {
                    cout << "No reminders to delete!\n";
                } else {
                    cout << "Enter number to delete (1 to " << taskCount << "): ";
                    int del;
                    cin >> del;
                  
                    // Moves the tasks to the left of them to fill the empty spot
                    if (del >= 1 && del <= taskCount) {
                        for (int i = del - 1; i < taskCount - 1; i++) {
                            tasks[i] = tasks[i + 1];
                            categories[i] = categories[i + 1];
                            priorities[i] = priorities[i + 1];
                            dueDays[i] = dueDays[i + 1];
                            dueMonths[i] = dueMonths[i + 1];
                            dueYears[i] = dueYears[i + 1];
                            dueHours[i] = dueHours[i + 1];
                            dueMinutes[i] = dueMinutes[i + 1];
                        }
                        taskCount--;
                        saveTasksToFile(tasks, categories, priorities, dueHours, dueMinutes, dueDays, dueMonths, dueYears, taskCount);
                        cout << "Deleted and saved successfully!\n";
                    } else {
                        cout << "Invalid number!\n";
                    }
                }
                break;
            }

            case 4:
                saveTasksToFile(tasks, categories, priorities, dueHours, dueMinutes, dueDays, dueMonths, dueYears, taskCount);
                cout << "Tasks saved. Goodbye!\n";
                break;

            default:
                cout << "Invalid option, try again.\n";
                break;
        }
    }

    return 0;
}
