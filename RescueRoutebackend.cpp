#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <cctype>
#include <limits>

using namespace std;

// ============================================================
// UTILITY FUNCTIONS
// ============================================================

string trim(string s)
{
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ' || s.back() == '\t'))
        s.pop_back();

    size_t start = 0;

    while (start < s.size() &&
           (s[start] == ' ' || s[start] == '\t'))
        start++;

    return s.substr(start);
}

string toLower(string s)
{
    for (char &c : s)
        c = tolower(static_cast<unsigned char>(c));

    return s;
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

int findColumn(const vector<string>& header, vector<string> names)
{
    for (int i = 0; i < (int)header.size(); i++)
    {
        string h = toLower(trim(header[i]));

        for (string name : names)
        {
            if (h == toLower(name))
                return i;
        }
    }

    return -1;
}

// ============================================================
// LOCATION + GRAPH
// ============================================================

unordered_map<int, vector<int>> graph;
unordered_map<string, int> id;
unordered_map<int, string> name;

struct Edge
{
    int node;
    double distance;
};

unordered_map<int, vector<Edge>> weightedGraph;

// ============================================================
// LOAD LOCATIONS
// ============================================================

bool loadLocations(string filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error: Could not open " << filename << endl;
        return false;
    }

    string line;

    getline(file, line);

    while (getline(file, line))
    {
        if (trim(line).empty())
            continue;

        vector<string> fields = splitCSV(line);

        if (fields.size() < 2)
            continue;

        try
        {
            int node = stoi(fields[0]);
            string location = fields[1];

            id[location] = node;
            id[toLower(location)] = node;

            name[node] = location;
        }
        catch (...)
        {
            continue;
        }
    }

    file.close();

    return !name.empty();
}

// ============================================================
// LOAD ROAD NETWORK
// ============================================================

bool loadRoadNetwork(string filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error: Could not open " << filename << endl;
        return false;
    }

    string line;

    getline(file, line);

    while (getline(file, line))
    {
        if (trim(line).empty())
            continue;

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

    return !graph.empty();
}

// ============================================================
// BFS
// ============================================================

vector<int> bfs(int start, int destination)
{
    queue<int> q;

    unordered_map<int, int> parent;
    unordered_map<int, bool> visited;

    q.push(start);
    visited[start] = true;
    parent[start] = -1;

    while (!q.empty())
    {
        int u = q.front();
        q.pop();

        if (u == destination)
            break;

        for (int v : graph[u])
        {
            if (!visited[v])
            {
                visited[v] = true;
                parent[v] = u;
                q.push(v);
            }
        }
    }

    if (!visited[destination])
        return {};

    vector<int> path;

    for (int x = destination; x != -1; x = parent[x])
        path.push_back(x);

    reverse(path.begin(), path.end());

    return path;
}

double calculateDistance(const vector<int>& path)
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

// ============================================================
// EMERGENCY
// ============================================================

struct Emergency
{
    int emergencyId;
    int patientId;
    int severity;
    string status;
    int arrivalOrder;
};

struct EmergencyCompare
{
    bool operator()(const Emergency& a,
                    const Emergency& b) const
    {
        if (a.severity != b.severity)
            return a.severity < b.severity;

        return a.arrivalOrder > b.arrivalOrder;
    }
};

// ============================================================
// EMERGENCY MANAGER
// ============================================================

class EmergencyManager
{
private:

    priority_queue<
        Emergency,
        vector<Emergency>,
        EmergencyCompare
    > pq;

    vector<Emergency> emergencies;

public:

    bool loadFromCSV(string filename)
    {
        ifstream file(filename);

        if (!file.is_open())
        {
            cout << "Error: Could not open "
                 << filename << endl;

            return false;
        }

        string line;

        getline(file, line);

        vector<string> header = splitCSV(line);

        int emergencyIdColumn =
            findColumn(header,
            {"emergency_id", "request_id"});

        int patientIdColumn =
            findColumn(header,
            {"patient_id"});

        int severityColumn =
            findColumn(header,
            {"severity", "priority"});

        int statusColumn =
            findColumn(header,
            {"status"});

        if (emergencyIdColumn == -1 ||
            patientIdColumn == -1 ||
            severityColumn == -1 ||
            statusColumn == -1)
        {
            cout << "Error: Invalid emergency_requests.csv format."
                 << endl;

            return false;
        }

        int order = 0;

        while (getline(file, line))
        {
            if (trim(line).empty())
                continue;

            vector<string> fields = splitCSV(line);

            if ((int)fields.size() <= statusColumn)
                continue;

            try
            {
                Emergency e;

                e.emergencyId =
                    stoi(fields[emergencyIdColumn]);

                e.patientId =
                    stoi(fields[patientIdColumn]);

                e.severity =
                    stoi(fields[severityColumn]);

                e.status =
                    trim(fields[statusColumn]);

                e.arrivalOrder = order++;

                if (toLower(e.status) == "pending")
                {
                    pq.push(e);
                    emergencies.push_back(e);
                }
            }
            catch (...)
            {
                cout << "Warning: Invalid emergency record skipped."
                     << endl;
            }
        }

        file.close();

        cout << emergencies.size()
             << " pending emergency records loaded successfully."
             << endl;

        return true;
    }

    void viewPending()
    {
        priority_queue<
            Emergency,
            vector<Emergency>,
            EmergencyCompare
        > temp = pq;

        cout << "\n========== PENDING EMERGENCIES ==========\n";

        if (temp.empty())
        {
            cout << "No pending emergencies.\n";
            return;
        }

        while (!temp.empty())
        {
            Emergency e = temp.top();
            temp.pop();

            cout << "Emergency ID : " << e.emergencyId << endl;
            cout << "Patient ID   : " << e.patientId << endl;
            cout << "Severity     : " << e.severity << endl;
            cout << "Status       : " << e.status << endl;
            cout << "------------------------------------------\n";
        }
    }

    bool empty()
    {
        return pq.empty();
    }

    Emergency getNextEmergency()
    {
        Emergency e = pq.top();
        pq.pop();

        return e;
    }

    void addEmergency(int emergencyId,
                      int patientId,
                      int severity)
    {
        Emergency e;

        e.emergencyId = emergencyId;
        e.patientId = patientId;
        e.severity = severity;
        e.status = "PENDING";
        e.arrivalOrder = emergencies.size();

        pq.push(e);
        emergencies.push_back(e);
    }
};

// ============================================================
// PATIENT
// ============================================================

struct Patient
{
    int patientId;
    int node;
    string location;
};

// ============================================================
// LOAD PATIENTS
// ============================================================

unordered_map<int, Patient> patients;

bool loadPatients(string filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Warning: Could not open "
             << filename << endl;

        return false;
    }

    string line;

    getline(file, line);

    vector<string> header = splitCSV(line);

    int patientIdColumn =
        findColumn(header,
        {"patient_id", "id"});

    int nodeColumn =
        findColumn(header,
        {"location_node", "node_id", "current_node"});

    int locationColumn =
        findColumn(header,
        {"location", "current_location"});

    if (patientIdColumn == -1)
    {
        cout << "Error: patient_id not found in patients.csv"
             << endl;

        return false;
    }

    while (getline(file, line))
    {
        if (trim(line).empty())
            continue;

        vector<string> fields = splitCSV(line);

        try
        {
            Patient p;

            p.patientId =
                stoi(fields[patientIdColumn]);

            p.node = -1;
            p.location = "";

            if (nodeColumn != -1 &&
                nodeColumn < (int)fields.size())
            {
                p.node = stoi(fields[nodeColumn]);

                if (name.count(p.node))
                    p.location = name[p.node];
            }

            if (locationColumn != -1 &&
                locationColumn < (int)fields.size())
            {
                p.location = fields[locationColumn];

                if (p.node == -1)
                {
                    string lowerLocation =
                        toLower(p.location);

                    if (id.count(lowerLocation))
                        p.node = id[lowerLocation];
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

// ============================================================
// AMBULANCE
// ============================================================

struct Ambulance
{
    int id;
    string name;
    int node;
    string status;
};

vector<Ambulance> ambulances;

// ============================================================
// LOAD AMBULANCES
// ============================================================

bool loadAmbulances(string filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error: Could not open "
             << filename << endl;

        return false;
    }

    string line;

    getline(file, line);

    vector<string> header = splitCSV(line);

    int idColumn =
        findColumn(header,
        {"ambulance_id", "id"});

    int nameColumn =
        findColumn(header,
        {"ambulance_name", "name"});

    int nodeColumn =
        findColumn(header,
        {"current_node", "node_id", "location_node"});

    int statusColumn =
        findColumn(header,
        {"status"});

    if (idColumn == -1 ||
        nodeColumn == -1 ||
        statusColumn == -1)
    {
        cout << "Error: Invalid ambulances.csv format."
             << endl;

        return false;
    }

    while (getline(file, line))
    {
        if (trim(line).empty())
            continue;

        vector<string> fields = splitCSV(line);

        try
        {
            Ambulance a;

            a.id = stoi(fields[idColumn]);

            if (nameColumn != -1 &&
                nameColumn < (int)fields.size())
            {
                a.name = fields[nameColumn];
            }
            else
            {
                a.name = "AMB-" + to_string(a.id);
            }

            a.node = stoi(fields[nodeColumn]);
            a.status = fields[statusColumn];

            ambulances.push_back(a);
        }
        catch (...)
        {
            continue;
        }
    }

    file.close();

    cout << ambulances.size()
         << " ambulance records loaded successfully."
         << endl;

    return true;
}

// ============================================================
// FIND AVAILABLE AMBULANCE
// ============================================================

int findAvailableAmbulance(int patientNode)
{
    int bestIndex = -1;
    int bestSegments = numeric_limits<int>::max();

    for (int i = 0; i < (int)ambulances.size(); i++)
    {
        if (toLower(ambulances[i].status) != "available")
            continue;

        vector<int> path =
            bfs(ambulances[i].node, patientNode);

        if (path.empty())
            continue;

        int segments = path.size() - 1;

        if (segments < bestSegments)
        {
            bestSegments = segments;
            bestIndex = i;
        }
    }

    return bestIndex;
}

// ============================================================
// HOSPITAL
// ============================================================

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

// ============================================================
// LOAD HOSPITALS
// ============================================================

bool loadHospitals(string filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error: Could not open "
             << filename << endl;

        return false;
    }

    string line;

    getline(file, line);

    vector<string> header = splitCSV(line);

    int idColumn =
        findColumn(header,
        {"hospital_id", "id"});

    int nameColumn =
        findColumn(header,
        {"hospital_name", "name"});

    int nodeColumn =
        findColumn(header,
        {"node_id", "location_node"});

    int totalColumn =
        findColumn(header,
        {"total_capacity", "total_beds", "total_num_beds"});

    int availableColumn =
        findColumn(header,
        {"available_capacity", "available_beds"});

    int statusColumn =
        findColumn(header,
        {"status"});

    while (getline(file, line))
    {
        if (trim(line).empty())
            continue;

        vector<string> fields = splitCSV(line);

        try
        {
            Hospital h;

            if (idColumn != -1)
                h.id = stoi(fields[idColumn]);
            else
                h.id = hospitals.size() + 1;

            if (nameColumn != -1 &&
                nameColumn < (int)fields.size())
            {
                h.name = fields[nameColumn];
            }
            else
            {
                h.name = "Hospital " + to_string(h.id);
            }

            h.node = -1;

            if (nodeColumn != -1 &&
                nodeColumn < (int)fields.size())
            {
                h.node = stoi(fields[nodeColumn]);
            }

            h.totalCapacity = 0;
            h.availableCapacity = 0;

            if (totalColumn != -1 &&
                totalColumn < (int)fields.size())
            {
                h.totalCapacity =
                    stoi(fields[totalColumn]);
            }

            if (availableColumn != -1 &&
                availableColumn < (int)fields.size())
            {
                h.availableCapacity =
                    stoi(fields[availableColumn]);
            }

            if (statusColumn != -1 &&
                statusColumn < (int)fields.size())
            {
                h.status = fields[statusColumn];
            }
            else
            {
                h.status =
                    h.availableCapacity > 0
                    ? "AVAILABLE"
                    : "FULL";
            }

            hospitals.push_back(h);
        }
        catch (...)
        {
            continue;
        }
    }

    file.close();

    cout << hospitals.size()
         << " hospital records loaded successfully."
         << endl;

    return true;
}

// ============================================================
// FIND HOSPITAL
// ============================================================

int findHospital(int patientNode)
{
    int bestIndex = -1;
    int bestSegments = numeric_limits<int>::max();

    for (int i = 0; i < (int)hospitals.size(); i++)
    {
        if (hospitals[i].availableCapacity <= 0)
            continue;

        if (toLower(hospitals[i].status) != "available")
            continue;

        if (hospitals[i].node == -1)
            continue;

        vector<int> path =
            bfs(patientNode, hospitals[i].node);

        if (path.empty())
            continue;

        int segments = path.size() - 1;

        if (segments < bestSegments)
        {
            bestSegments = segments;
            bestIndex = i;
        }
    }

    return bestIndex;
}

// ============================================================
// DISPLAY AMBULANCES
// ============================================================

void displayAmbulances()
{
    cout << "\n========== AMBULANCE STATUS ==========\n";

    for (const Ambulance& a : ambulances)
    {
        cout << "ID: " << a.id
             << " | Name: " << a.name
             << " | Node: " << a.node
             << " | Status: " << a.status
             << endl;
    }
}

// ============================================================
// DISPLAY HOSPITALS
// ============================================================

void displayHospitals()
{
    cout << "\n========== HOSPITAL STATUS ==========\n";

    for (const Hospital& h : hospitals)
    {
        cout << "ID: " << h.id
             << " | Name: " << h.name
             << " | Node: " << h.node
             << " | Capacity: "
             << h.availableCapacity
             << "/" << h.totalCapacity
             << " | Status: " << h.status
             << endl;
    }
}

// ============================================================
// MAIN INTEGRATION
// ============================================================

int main()
{
    const string emergencyFile =
        "emergency_requests.csv";

    const string patientFile =
        "patients.csv";

    const string ambulanceFile =
        "ambulances.csv";

    const string hospitalFile =
        "hospital_data.csv";

    const string locationFile =
        "locations.csv";

    const string roadFile =
        "road_network.csv";

    cout << "========================================================\n";
    cout << "                    RESCUE ROUTE\n";
    cout << "       Emergency and Disaster Management System\n";
    cout << "========================================================\n";

    // --------------------------------------------------------
    // LOAD DATA
    // --------------------------------------------------------

    cout << "\n========== LOADING DATA ==========\n";

    if (!loadLocations(locationFile))
        return 1;

    cout << name.size()
         << " locations loaded successfully.\n";

    if (!loadRoadNetwork(roadFile))
        return 1;

    cout << graph.size()
         << " graph nodes loaded successfully.\n";

    EmergencyManager emergencyManager;

    if (!emergencyManager.loadFromCSV(emergencyFile))
        return 1;

    loadPatients(patientFile);

    if (!loadAmbulances(ambulanceFile))
        return 1;

    if (!loadHospitals(hospitalFile))
        return 1;

    // --------------------------------------------------------
    // SHOW DATA
    // --------------------------------------------------------

    emergencyManager.viewPending();

    displayAmbulances();

    displayHospitals();

    // --------------------------------------------------------
    // SELECT HIGHEST PRIORITY EMERGENCY
    // --------------------------------------------------------

    if (emergencyManager.empty())
    {
        cout << "\nNo pending emergencies to process.\n";
        return 0;
    }

    Emergency emergency =
        emergencyManager.getNextEmergency();

    cout << "\n========================================================\n";
    cout << "              CURRENT EMERGENCY\n";
    cout << "========================================================\n";

    cout << "Emergency ID : "
         << emergency.emergencyId << endl;

    cout << "Patient ID   : "
         << emergency.patientId << endl;

    cout << "Severity     : "
         << emergency.severity << endl;

    cout << "Status       : "
         << emergency.status << endl;

    // --------------------------------------------------------
    // FIND PATIENT
    // --------------------------------------------------------

    if (!patients.count(emergency.patientId))
    {
        cout << "\nPatient information not found.\n";
        cout << "Emergency remains pending.\n";
        return 0;
    }

    Patient patient =
        patients[emergency.patientId];

    if (patient.node == -1)
    {
        cout << "\nPatient location could not be determined.\n";
        cout << "Emergency remains pending.\n";
        return 0;
    }

    cout << "\nPatient Location : ";

    if (name.count(patient.node))
        cout << name[patient.node];
    else
        cout << patient.node;

    cout << endl;

    // --------------------------------------------------------
    // FIND AMBULANCE
    // --------------------------------------------------------

    int ambulanceIndex =
        findAvailableAmbulance(patient.node);

    if (ambulanceIndex == -1)
    {
        cout << "\nNo available ambulance currently.\n";
        cout << "Emergency remains pending.\n";
        return 0;
    }

    Ambulance& ambulance =
        ambulances[ambulanceIndex];

    cout << "\n========== AMBULANCE ASSIGNMENT ==========\n";

    cout << "Ambulance ID : "
         << ambulance.id << endl;

    cout << "Ambulance    : "
         << ambulance.name << endl;

    cout << "Previous Status : "
         << ambulance.status << endl;

    ambulance.status = "BUSY";

    cout << "New Status      : "
         << ambulance.status << endl;

    // --------------------------------------------------------
    // FIND HOSPITAL
    // --------------------------------------------------------

    int hospitalIndex =
        findHospital(patient.node);

    if (hospitalIndex == -1)
    {
        cout << "\nNo reachable hospital with available capacity.\n";

        ambulance.status = "AVAILABLE";

        cout << "Ambulance released.\n";
        cout << "Emergency remains pending.\n";

        return 0;
    }

    Hospital& hospital =
        hospitals[hospitalIndex];

    // --------------------------------------------------------
    // HOSPITAL ASSIGNMENT
    // --------------------------------------------------------

    cout << "\n========== HOSPITAL ASSIGNMENT ==========\n";

    cout << "Hospital : "
         << hospital.name << endl;

    cout << "Available Capacity Before : "
         << hospital.availableCapacity << endl;

    hospital.availableCapacity--;

    if (hospital.availableCapacity == 0)
        hospital.status = "FULL";

    cout << "Available Capacity After  : "
         << hospital.availableCapacity << endl;

    cout << "Hospital Status           : "
         << hospital.status << endl;

    // --------------------------------------------------------
    // BFS ROUTE
    // --------------------------------------------------------

    vector<int> route =
        bfs(patient.node, hospital.node);

    if (route.empty())
    {
        cout << "\nNo route found.\n";

        ambulance.status = "AVAILABLE";
        hospital.availableCapacity++;

        if (hospital.availableCapacity > 0)
            hospital.status = "AVAILABLE";

        return 0;
    }

    // --------------------------------------------------------
    // FINAL RESPONSE
    // --------------------------------------------------------

    cout << "\n========================================================\n";
    cout << "                RECOMMENDED ROUTE\n";
    cout << "========================================================\n";

    for (int i = 0; i < (int)route.size(); i++)
    {
        if (name.count(route[i]))
            cout << name[route[i]];
        else
            cout << route[i];

        if (i != (int)route.size() - 1)
            cout << "\n        ↓\n";
    }

    cout << "\n\nRoad Segments : "
         << route.size() - 1 << endl;

    cout << "Distance      : "
         << calculateDistance(route)
         << " km" << endl;

    // --------------------------------------------------------
    // FINAL STATUS
    // --------------------------------------------------------

    cout << "\n========================================================\n";
    cout << "                  FINAL STATUS\n";
    cout << "========================================================\n";

    cout << "Emergency selected       : SUCCESS\n";
    cout << "Ambulance assigned        : SUCCESS\n";
    cout << "Hospital selected         : SUCCESS\n";
    cout << "Hospital capacity updated : SUCCESS\n";
    cout << "BFS route found           : SUCCESS\n";
    cout << "Dispatch simulation       : SUCCESS\n";

    cout << "\n========================================================\n";
    cout << "                 RESCUEROUTE COMPLETE\n";
    cout << "========================================================\n";

    return 0;
}