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

