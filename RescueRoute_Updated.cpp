#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <limits>
using namespace std;
// BASIC FUNCTIONS
string toLower(string text)

{

    for (char& ch : text)

    {

        ch = static_cast<char>(

            tolower(static_cast<unsigned char>(ch))

        );

    }

    return text;

}

string trim(string text)

{

    size_t start = text.find_first_not_of(" \t\r");

    size_t end = text.find_last_not_of(" \t\r");

    if (start == string::npos)

        return "";

    return text.substr(start, end - start + 1);

}

vector<string> splitCSV(string line)

{

    vector<string> fields;

    string value;

    stringstream ss(line);

    while (getline(ss, value, ','))

        fields.push_back(trim(value));

    return fields;

}
// LOCATION INPUT
string getValidLocation()

{

    vector<string> locations =

    {

        "Clock Tower",

        "Prince Chowk",

        "Dehradun Railway Station",

        "ISBT Dehradun",

        "Rajpur Road",

        "Saharanpur Road",

        "Gandhi Road",

        "Haridwar Road",

        "Chakrata Road",

        "Survey Chowk",

        "Ballupur",

        "Patel Nagar",

        "Doon Hospital",

        "MAX Super Speciality Hospital",

        "Shri Mahant Indiresh Hospital",

        "Graphic Era Hospital"

    };

    while (true)

    {

        string location;

        cout << "Emergency Location\n";

        cout << "Please tell us your location:\n";

        cout << "> ";

        getline(cin, location);

        location = trim(location);

        try

        {

            if (location.empty())

            {

                throw invalid_argument(

                    "Please provide your current location so we can assist you."

                );

            }

            string inputLower = toLower(location);

            for (const string& validLocation : locations)

            {

                if (toLower(validLocation) == inputLower)

                    return validLocation;

            }

            throw invalid_argument(

                "We couldn't find that location in our service area. Please check the name and try again."

            );

        }

        catch (const invalid_argument& e)

        {

            cout << "\n" << e.what() << "\n\n";

        }

    }

}
// SEVERITY INPUT
int getSeverityNumber()

{

    while (true)

    {

        string input;

        cout << "\nPlease let us know how serious the situation is by selecting an option below.\n\n";

        cout << "  1. Minor Injury\n";

        cout << "  2. Mild Illness\n";

        cout << "  3. Moderate Injury\n";

        cout << "  4. Serious Injury\n";

        cout << "  5. Critical Situation\n\n";

        cout << "Please select an option (1-5): ";

        getline(cin, input);

        input = trim(input);

        try

        {

            if (input.empty())

                throw invalid_argument("Please do not leave a blank.");

            for (char ch : input)

            {

                if (!isdigit(static_cast<unsigned char>(ch)))

                {

                    throw invalid_argument(

                        "Please choose a number from 1 to 5."

                    );

                }

            }

            int choice = stoi(input);

            if (choice < 1 || choice > 5)

            {

                throw out_of_range(

                    "That option isn't available."

                );

            }

            return choice;

        }

        catch (const invalid_argument& e)

        {

            cout << "\nInvalid input: "

                 << e.what() << "\n";

        }

        catch (const out_of_range&)

        {

            cout << "\nInvalid selection. "

                 << "Please choose a severity level from 1 to 5.\n";

        }

        cout << "\n";

    }

}

string getSeverityText(int severity)

{

    switch (severity)

    {

        case 1:

            return "Minor Injury";

        case 2:

            return "Mild Illness";

        case 3:

            return "Moderate Injury";

        case 4:

            return "Serious Injury";

        case 5:

            return "Critical Situation";

    }

    return "Unknown";

}
int getPriority(string severity)

{

    severity = toLower(trim(severity));

    if (severity == "high")

        return 5;

    if (severity == "medium")

        return 3;

    if (severity == "low")

        return 1;

    return 0;

}

// LOCATION + GRAPH
unordered_map<int, vector<int>> graph;

unordered_map<int, string> nodeName;

unordered_map<string, int> locationId;

struct Edge

{

    int node;

    double distance;

};

unordered_map<int, vector<Edge>> weightedGraph;
// LOAD LOCATIONS
bool loadLocations(string filename)

{

    ifstream file(filename);

    if (!file.is_open())

    {

        cout << "Error: locations.csv not found.\n";

        return false;

    }

    string line;

    getline(file, line);

    while (getline(file, line))

    {

        vector<string> fields = splitCSV(line);

        if (fields.size() < 2)

            continue;

        try

        {

            int node = stoi(fields[0]);

            string location = fields[1];

            nodeName[node] = location;

            locationId[location] = node;

            locationId[toLower(location)] = node;

        }

        catch (...)

        {

            continue;

        }

    }

    file.close();

    return true;

}
// LOAD ROAD NETWORK
bool loadRoadNetwork(string filename)

{

    ifstream file(filename);

    if (!file.is_open())

    {

        cout << "Error: road_network.csv not found.\n";

        return false;

    }

    string line;

    getline(file, line);

    while (getline(file, line))

    {

        vector<string> fields = splitCSV(line);

        if (fields.size() < 2)

            continue;

        try

        {

            int u = stoi(fields[0]);

            int v = stoi(fields[1]);

            double distance = 1.0;

            if (fields.size() >= 3)

            {

                try

                {

                    distance = stod(fields[2]);

                }

                catch (...)

                {

                    distance = 1.0;

                }

            }

            graph[u].push_back(v);

            graph[v].push_back(u);

            weightedGraph[u].push_back({v, distance});

            weightedGraph[v].push_back({u, distance});

        }

        catch (...)

        {

            continue;

        }

    }

    file.close();

    return true;

}
// BFS
vector<int> bfs(int start, int destination)

{

    queue<int> q;

    unordered_map<int, bool> visited;

    unordered_map<int, int> parent;

    q.push(start);

    visited[start] = true;

    parent[start] = -1;

    while (!q.empty())

    {

        int current = q.front();

        q.pop();

        if (current == destination)

            break;

        for (int next : graph[current])

        {

            if (!visited[next])

            {

                visited[next] = true;

                parent[next] = current;

                q.push(next);

            }

        }

    }

    if (!visited[destination])

        return {};

    vector<int> path;

    for (int node = destination;

         node != -1;

         node = parent[node])

    {

        path.push_back(node);

    }

    reverse(path.begin(), path.end());

    return path;

}

double calculateDistance(vector<int> path)

{

    double total = 0;

    for (int i = 0; i + 1 < (int)path.size(); i++)

    {

        int u = path[i];

        int v = path[i + 1];

        for (Edge e : weightedGraph[u])

        {

            if (e.node == v)

            {

                total += e.distance;

                break;

            }

        }

    }

    return total;

}
// EMERGENCY
struct Emergency

{

    int emergencyId;

    int patientId;

    int severity;

    int priority;

    string severityText;

    int locationNode;

    string requestType;

    string status;

    int arrivalOrder;

};

struct EmergencyCompare

{

    bool operator()(const Emergency& a,

                    const Emergency& b) const

    {

        if (a.priority != b.priority)
            return a.priority < b.priority;

        return a.arrivalOrder > b.arrivalOrder;

    }

};

class EmergencyManager

{

private:

    priority_queue<

        Emergency,

        vector<Emergency>,

        EmergencyCompare

    > pq;

    int nextArrivalOrder = 0;

public:

    bool loadFromCSV(string filename)

    {

        ifstream file(filename);

        if (!file.is_open())

            return false;

        string line;

        getline(file, line);

        while (getline(file, line))

        {

            if (trim(line).empty())

                continue;

            vector<string> fields = splitCSV(line);

            if (fields.size() < 4)

                continue;

            try

            {

                Emergency e;

                e.emergencyId = stoi(fields[0]);

                e.patientId = stoi(fields[1]);

                e.locationNode = stoi(fields[2]);

                e.severityText = fields[3];

                e.priority = getPriority(fields[3]);

                if (e.priority == 0)

                    continue;

                e.severity = e.priority;

                e.requestType = fields[4];

                e.status = fields[5];

                e.arrivalOrder = nextArrivalOrder++;

                if (toLower(e.status) == "pending")

                    pq.push(e);

            }

            catch (...)

            {

                continue;

            }

        }

        file.close();

        return true;

    }

    void addEmergency(int id,

                      int patientId,

                      int severity)

    {

        Emergency e;

        e.emergencyId = id;

        e.patientId = patientId;

        e.severity = severity;

        e.priority = severity;

        e.severityText = getSeverityText(severity);

        e.locationNode = -1;

        e.requestType = "New Emergency";

        e.status = "PENDING";

        e.arrivalOrder = nextArrivalOrder++;

        pq.push(e);

    }

    Emergency getNextEmergency()

    {

        Emergency e = pq.top();

        pq.pop();

        return e;

    }

    bool empty()

    {

        return pq.empty();

    }

};
// PATIENT
struct Patient

{

    int patientId;

    int node;

};

unordered_map<int, Patient> patients;

bool loadPatients(string filename)

{

    ifstream file(filename);

    if (!file.is_open())

        return false;

    string line;

    getline(file, line);

    while (getline(file, line))

    {

        vector<string> fields = splitCSV(line);

        if (fields.size() < 2)

            continue;

        try

        {

            Patient p;

            p.patientId = stoi(fields[0]);

            p.node = -1;

            if (fields.size() >= 2)

            {

                try

                {

                    p.node = stoi(fields[1]);

                }

                catch (...)

                {

                    string loc = fields[1];

                    if (locationId.count(toLower(loc)))

                        p.node = locationId[toLower(loc)];

                }

            }

            patients[p.patientId] = p;

        }

        catch (...)

        {

            continue;

        }

    }

    file.close();

    return true;

}
// AMBULANCE
struct Ambulance

{

    int id;

    string name;

    int node;

    string status;

};

vector<Ambulance> ambulances;

bool loadAmbulances(string filename)

{

    ifstream file(filename);

    if (!file.is_open())

        return false;

    string line;

    getline(file, line);

    while (getline(file, line))

    {

        vector<string> fields = splitCSV(line);

        if (fields.size() < 4)

            continue;

        try

        {

            Ambulance a;

            a.id = stoi(fields[0]);

            a.name = fields[1];

            a.node = stoi(fields[2]);

            a.status = fields[3];

            ambulances.push_back(a);

        }

        catch (...)

        {

            continue;

        }

    }

    file.close();

    return true;

}

int findAvailableAmbulance(int patientNode)

{

    int selected = -1;

    int bestSegments = numeric_limits<int>::max();

    for (int i = 0; i < (int)ambulances.size(); i++)

    {

        if (toLower(ambulances[i].status) != "available")

            continue;

        vector<int> route =

            bfs(ambulances[i].node, patientNode);

        if (route.empty())

            continue;

        int segments = route.size() - 1;

        if (segments < bestSegments)

        {

            bestSegments = segments;

            selected = i;

        }

    }

    return selected;

}
// HOSPITAL
struct Hospital

{

    int id;

    string name;

    int node;

    int totalCapacity;

    int availableCapacity;

    string status;

};

vector<Hospital> hospitals;

bool loadHospitals(string filename)

{

    ifstream file(filename);

    if (!file.is_open())

        return false;

    string line;

    getline(file, line);

    while (getline(file, line))

    {

        vector<string> fields = splitCSV(line);

        if (fields.size() < 6)

            continue;

        try

        {

            Hospital h;

            h.id = stoi(fields[0]);

            h.name = fields[1];

            h.node = stoi(fields[2]);

            h.totalCapacity = stoi(fields[3]);

            h.availableCapacity = stoi(fields[4]);

            h.status = fields[5];

            hospitals.push_back(h);

        }

        catch (...)

        {

            continue;

        }

    }

    file.close();

    return true;

}

int findHospital(int patientNode)

{

    int selected = -1;

    int bestSegments = numeric_limits<int>::max();

    for (int i = 0; i < (int)hospitals.size(); i++)

    {

        if (hospitals[i].availableCapacity <= 0)

            continue;

        if (toLower(hospitals[i].status) != "available")

            continue;

        vector<int> route =

            bfs(patientNode, hospitals[i].node);

        if (route.empty())

            continue;

        int segments = route.size() - 1;

        if (segments < bestSegments)

        {

            bestSegments = segments;

            selected = i;

        }

    }

    return selected;

}
// MAIN
int main()

{

    cout << "========================================================\n";

    cout << "                    RESCUE ROUTE\n";

    cout << "       Emergency and Disaster Management System\n";

    cout << "========================================================\n\n";
    // USER INPUT
    string location = getValidLocation();

    int severity = getSeverityNumber();
    // LOAD ACTUAL DATASET
    if (!loadLocations("locations.csv"))

    {

        cout << "\nUnable to load location data.\n";

        return 1;

    }

    if (!loadRoadNetwork("road_network.csv"))

    {

        cout << "\nUnable to load road network.\n";

        return 1;

    }

    loadPatients("patients.csv");

    if (!loadAmbulances("ambulances.csv"))

    {

        cout << "\nUnable to load ambulance data.\n";

        return 1;

    }

    if (!loadHospitals("hospital_data.csv"))

    {

        cout << "\nUnable to load hospital data.\n";

        return 1;

    }
    // EMERGENCY MANAGER
    EmergencyManager manager;

    manager.loadFromCSV("emergency_requests.csv");

    // User's new emergency

    manager.addEmergency(

        406,

        0,

        severity

    );
    // FIND LOCATION NODE
    int locationNode =

        locationId[toLower(location)];
    // FIND HIGHEST PRIORITY EMERGENCY
    Emergency emergency =

        manager.getNextEmergency();
    // FIND AMBULANCE
    int ambulanceIndex =

        findAvailableAmbulance(locationNode);

    if (ambulanceIndex == -1)

    {

        cout << "\nNo available ambulance found.\n";

        return 0;

    }

    Ambulance& ambulance =

        ambulances[ambulanceIndex];

    ambulance.status = "BUSY";
    // FIND HOSPITAL
    int hospitalIndex =

        findHospital(locationNode);

    if (hospitalIndex == -1)

    {

        ambulance.status = "AVAILABLE";

        cout << "\nNo hospital with available capacity found.\n";

        return 0;

    }

    Hospital& hospital =

        hospitals[hospitalIndex];

    hospital.availableCapacity--;

    if (hospital.availableCapacity == 0)

        hospital.status = "FULL";
    // BFS ROUTE
    vector<int> route =

        bfs(locationNode, hospital.node);

    if (route.empty())

    {

        hospital.availableCapacity++;

        ambulance.status = "AVAILABLE";

        cout << "\nNo route available.\n";

        return 0;

    }
    // FINAL DASHBOARD
    cout << "\n\n";

    cout << "========================================================\n";

    cout << "                    RESCUE ROUTE\n";

    cout << "       Emergency and Disaster Management System\n";

    cout << "========================================================\n\n";

    cout << "CURRENT EMERGENCY\n";

    cout << "--------------------------------------------------------\n";

    cout << "Emergency ID       : "

         << emergency.emergencyId << "\n";

    cout << "Location           : "

         << location << "\n";

    cout << "Severity           : "

         << emergency.severityText << "\n";

    cout << "Priority            : "

         << emergency.priority << "\n";

    cout << "Status             : ACTIVE\n\n";

    cout << "AMBULANCE\n";

    cout << "--------------------------------------------------------\n";

    cout << "Assigned ID        : "

         << ambulance.id << "\n";

    cout << "Ambulance Name     : "

         << ambulance.name << "\n";

    cout << "Status             : "

         << ambulance.status << "\n\n";

    cout << "HOSPITAL\n";

    cout << "--------------------------------------------------------\n";

    cout << "Name               : "

         << hospital.name << "\n";

    cout << "Available Capacity : "

         << hospital.availableCapacity << "\n";

    cout << "Status             : "

         << hospital.status << "\n\n";

    cout << "ROUTE\n";

    cout << "--------------------------------------------------------\n";

    cout << "Path               : ";

    for (int i = 0; i < (int)route.size(); i++)

    {

        cout << nodeName[route[i]];

        if (i != (int)route.size() - 1)

            cout << " -> ";

    }

    cout << "\n";

    cout << "Road Segments      : "

         << route.size() - 1 << "\n";

    cout << "Total Distance     : "

         << calculateDistance(route)

         << " km\n\n";

    cout << "========================================================\n";

    cout << "       Kindly stay where you are, help is on the way.\n";

    cout << "           We highly appreciate your patience.\n";

    cout << "========================================================\n";

    cout << "\nPress Enter to exit...";
cin.get();

    return 0;

}
