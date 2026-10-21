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
