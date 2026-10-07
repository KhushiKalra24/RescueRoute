#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <stdexcept>

using namespace std;

// Stores all information related to one emergency request
struct Emergency
{
    string requestId;
    string patientId;
    long long locationNode;
    string severity;
    string requestType;
    string status;
    int priority;
    int arrivalOrder;
};

class EmergencyManager
{
private:
    vector<Emergency> emergencies;
    int nextArrivalOrder = 0;

    // Priority Queue:
    // High -> Medium -> Low
    // If priority is same, older request is handled first.
    struct Compare
    {
        const vector<Emergency>* data;

        bool operator()(int a, int b) const
        {
            const Emergency& first = (*data)[a];
            const Emergency& second = (*data)[b];

            if (first.priority != second.priority)
                return first.priority < second.priority;

            return first.arrivalOrder > second.arrivalOrder;
        }
    };

    priority_queue<int, vector<int>, Compare> emergencyQueue{
        Compare{&emergencies}
    };

    string trim(string text)
    {
        size_t start = text.find_first_not_of(" \t\r\n");
        size_t end = text.find_last_not_of(" \t\r\n");

        if (start == string::npos)
            return "";

        return text.substr(start, end - start + 1);
    }

    string toLower(string text)
    {
        transform(text.begin(), text.end(), text.begin(),
                  [](unsigned char ch)
                  {
                      return static_cast<char>(tolower(ch));
                  });

        return text;
    }

    int getPriority(string severity)
    {
        severity = toLower(trim(severity));

        if (severity == "high")
            return 3;

        if (severity == "medium")
            return 2;

        return 1;
    }

    vector<string> splitCSV(string line)
    {
        vector<string> fields;
        string field;
        bool insideQuotes = false;

        for (char ch : line)
        {
            if (ch == '"')
            {
                insideQuotes = !insideQuotes;
            }
            else if (ch == ',' && !insideQuotes)
            {
                fields.push_back(trim(field));
                field.clear();
            }
            else
            {
                field += ch;
            }
        }

        fields.push_back(trim(field));
        return fields;
    }

    void rebuildQueue()
    {
        priority_queue<int, vector<int>, Compare> newQueue(
            Compare{&emergencies}
        );

        emergencyQueue = move(newQueue);

        for (int i = 0; i < static_cast<int>(emergencies.size()); i++)
        {
            if (toLower(emergencies[i].status) == "pending")
                emergencyQueue.push(i);
        }
    }

    // Creates the CSV file with some initial records if it does not exist.
    void createFileWithSampleData(const string& filename)
    {
        ofstream file(filename);

        if (!file.is_open())
            throw runtime_error("Unable to create emergency data file.");

        file << "request_id,patient_id,location_node,severity,request_type,status\n";
        file << "ER001,P101,1,High,Road Accident,Pending\n";
        file << "ER002,P102,3,Medium,Medical Emergency,Pending\n";
        file << "ER003,P103,5,Low,Minor Injury,Pending\n";
        file << "ER004,P104,2,High,Fire Emergency,Pending\n";
        file << "ER005,P105,7,Medium,Heart Problem,Pending\n";

        file.close();
    }

public:

    // ================= LOAD DATA FROM FILE =================

    bool loadFromCSV(const string& filename)
    {
        try
        {
            ifstream file(filename);

            // If the file does not exist, create it automatically.
            if (!file.is_open())
            {
                cout << "\nData file not found.";
                cout << "\nCreating a new emergency_requests.csv file...\n";

                createFileWithSampleData(filename);
                file.open(filename);

                if (!file.is_open())
                    throw runtime_error("Could not create emergency data file.");
            }

            emergencies.clear();
            nextArrivalOrder = 0;

            string line;
            bool firstLine = true;

            while (getline(file, line))
            {
                if (trim(line).empty())
                    continue;

                // Ignore the CSV header
                if (firstLine)
                {
                    firstLine = false;
                    continue;
                }

                vector<string> fields = splitCSV(line);

                if (fields.size() < 6)
                {
                    cout << "Warning: Invalid record skipped.\n";
                    continue;
                }

                try
                {
                    Emergency e;

                    e.requestId = fields[0];
                    e.patientId = fields[1];
                    e.locationNode = stoll(fields[2]);
                    e.severity = fields[3];
                    e.requestType = fields[4];
                    e.status = fields[5];

                    e.priority = getPriority(e.severity);
                    e.arrivalOrder = nextArrivalOrder++;

                    emergencies.push_back(e);
                }
                catch (const invalid_argument&)
                {
                    cout << "Warning: Invalid location for "
                         << fields[0] << ". Record skipped.\n";
                }
                catch (const out_of_range&)
                {
                    cout << "Warning: Location value is too large for "
                         << fields[0] << ". Record skipped.\n";
                }
            }

            file.close();
            rebuildQueue();

            cout << emergencies.size()
                 << " emergency records loaded successfully.\n";

            return true;
        }
        catch (const exception& e)
        {
            cout << "\nFile Error: " << e.what() << endl;
            return false;
        }
    }

    // ================= SAVE ALL DATA =================

    bool saveToCSV(const string& filename)
    {
        try
        {
            ofstream file(filename);

            if (!file.is_open())
                throw runtime_error("Could not save emergency data.");

            file << "request_id,patient_id,location_node,"
                 << "severity,request_type,status\n";

            for (const Emergency& e : emergencies)
            {
                file << e.requestId << ","
                     << e.patientId << ","
                     << e.locationNode << ","
                     << e.severity << ","
                     << e.requestType << ","
                     << e.status << "\n";
            }

            file.close();
            return true;
        }
        catch (const exception& e)
        {
            cout << "\nFile Error: " << e.what() << endl;
            return false;
        }
    }

    // ================= ADD NEW EMERGENCY =================

    void addEmergency(const string& filename)
    {
        try
        {
            Emergency e;

            cout << "\n========== ADD NEW EMERGENCY ==========\n";

            cout << "Request ID: ";
            cin >> e.requestId;

            cout << "Patient ID: ";
            cin >> e.patientId;

            cout << "Location Node: ";
            cin >> e.locationNode;

            if (cin.fail())
            {
                cin.clear();
                cin.ignore(1000, '\n');
                throw invalid_argument("Location node must be a number.");
            }

            cout << "Severity (High/Medium/Low): ";
            cin >> e.severity;

            e.severity = toLower(e.severity);

            if (e.severity != "high" &&
                e.severity != "medium" &&
                e.severity != "low")
            {
                throw invalid_argument(
                    "Severity must be High, Medium or Low."
                );
            }

            cin.ignore();

            cout << "Request Type: ";
            getline(cin, e.requestType);

            e.status = "Pending";
            e.priority = getPriority(e.severity);
            e.arrivalOrder = nextArrivalOrder++;

            emergencies.push_back(e);
            rebuildQueue();

            // Save the complete updated list to the CSV file.
            if (!saveToCSV(filename))
            {
                emergencies.pop_back();
                rebuildQueue();
                throw runtime_error("Emergency could not be saved.");
            }

            cout << "\nEmergency added successfully!";
            cout << "\nRecord saved in: " << filename << endl;
            cout << "Assigned Priority: " << e.priority << endl;
        }
        catch (const exception& e)
        {
            cout << "\nError: " << e.what() << endl;
        }
    }

    // ================= VIEW PENDING =================

    void viewPendingEmergencies()
    {
        if (emergencyQueue.empty())
        {
            cout << "\nNo pending emergencies.\n";
            return;
        }

        priority_queue<int, vector<int>, Compare> temp = emergencyQueue;

        cout << "\n========== PENDING EMERGENCIES ==========\n";
        cout << "ID\tPatient\tSeverity\tType\t\tPriority\n";
        cout << "--------------------------------------------------------\n";

        while (!temp.empty())
        {
            int index = temp.top();
            temp.pop();

            const Emergency& e = emergencies[index];

            cout << e.requestId << "\t"
                 << e.patientId << "\t"
                 << e.severity << "\t\t"
                 << e.requestType << "\t"
                 << e.priority << endl;
        }
    }

    // ================= PROCESS EMERGENCY =================

    void processNextEmergency(const string& filename)
    {
        try
        {
            if (emergencyQueue.empty())
            {
                cout << "\nNo pending emergencies to process.\n";
                return;
            }

            int index = emergencyQueue.top();
            emergencyQueue.pop();

            Emergency& e = emergencies[index];

            cout << "\n========== PROCESSING EMERGENCY ==========\n";
            cout << "Request ID   : " << e.requestId << endl;
            cout << "Patient ID   : " << e.patientId << endl;
            cout << "Location     : " << e.locationNode << endl;
            cout << "Severity     : " << e.severity << endl;
            cout << "Request Type : " << e.requestType << endl;
            cout << "Priority     : " << e.priority << endl;

            e.status = "Processed";

            // Save the changed status permanently.
            if (!saveToCSV(filename))
            {
                e.status = "Pending";
                rebuildQueue();
                throw runtime_error(
                    "Could not update emergency record in file."
                );
            }

            cout << "Status       : Processed";
            cout << "\nChanges saved successfully.\n";
        }
        catch (const exception& e)
        {
            cout << "\nError: " << e.what() << endl;
        }
    }

    // ================= VIEW ALL =================

    void showAllEmergencies()
    {
        if (emergencies.empty())
        {
            cout << "\nNo emergency records available.\n";
            return;
        }

        cout << "\n========== ALL EMERGENCY RECORDS ==========\n";

        for (const Emergency& e : emergencies)
        {
            cout << "\nRequest ID   : " << e.requestId;
            cout << "\nPatient ID   : " << e.patientId;
            cout << "\nLocation     : " << e.locationNode;
            cout << "\nSeverity     : " << e.severity;
            cout << "\nRequest Type : " << e.requestType;
            cout << "\nStatus       : " << e.status;
            cout << "\nPriority     : " << e.priority;
            cout << "\n------------------------------------------";
        }

        cout << endl;
    }

    // ================= PENDING COUNT =================

    int pendingCount()
    {
        int count = 0;

        for (const Emergency& e : emergencies)
        {
            if (toLower(e.status) == "pending")
                count++;
        }

        return count;
    }
};

// ================= MAIN FUNCTION =================

int main()
{
    // Keep this CSV in the same folder as the C++ file.
    const string filename = "emergency_requests.csv";

    try
    {
        EmergencyManager manager;

        if (!manager.loadFromCSV(filename))
            return 1;

        int choice;

        do
        {
            cout << "\n\n========== RESCUEROUTE ==========\n";
            cout << "     EMERGENCY MANAGEMENT SYSTEM\n";
            cout << "=================================\n";
            cout << "1. View pending emergencies\n";
            cout << "2. Add new emergency\n";
            cout << "3. Process highest-priority emergency\n";
            cout << "4. View all emergencies\n";
            cout << "5. Show pending count\n";
            cout << "0. Exit\n";

            cout << "\nEnter your choice: ";
            cin >> choice;

            if (cin.fail())
            {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "\nPlease enter a valid number.\n";
                continue;
            }

            switch (choice)
            {
                case 1:
                    manager.viewPendingEmergencies();
                    break;

                case 2:
                    manager.addEmergency(filename);
                    break;

                case 3:
                    manager.processNextEmergency(filename);
                    break;

                case 4:
                    manager.showAllEmergencies();
                    break;

                case 5:
                    cout << "\nPending emergencies: "
                         << manager.pendingCount() << endl;
                    break;

                case 0:
                    cout << "\nExiting RescueRoute...\n";
                    break;

                default:
                    cout << "\nInvalid choice. Please try again.\n";
            }

        } while (choice != 0);
    }
    catch (const exception& e)
    {
        cout << "\nProgram Error: " << e.what() << endl;
    }

    return 0;
}
