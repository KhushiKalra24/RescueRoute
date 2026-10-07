#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

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
    size_t start = text.find_first_not_of(" \t");
    size_t end = text.find_last_not_of(" \t");

    if (start == string::npos)
    {
        return "";
    }

    return text.substr(start, end - start + 1);
}

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
                {
                    return validLocation;
                }
            }

            throw invalid_argument(
                "We couldn't find that location in our service area. Please check the name and try again."
            );
        }
        catch (const invalid_argument& e)
        {
            cout << "\n"<< e.what() << "\n";
        }
    }
}

string getSeverity()
{
    while (true)
    {
        string input;

        cout << "\nPlease let us know how serious the situation is by selecting an option below.\n";

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
            {
                throw invalid_argument(
                    "Please do not leave a blank."
                );
            }

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
                    "That option isn't available. Please choose one of the options from 1 to 5."
                );
            }


            switch (choice)
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
        }


        catch (const invalid_argument& e)
        {
            cout << "\nInvalid input: "
                 << e.what() << "\n";
        }


        catch (const out_of_range& e)
        {
            cout << "\nInvalid selection. "
                 << "Please choose a severity level from 1 to 5.\n";
        }

        cout << "\n";
    }
}

int main()
{
    string location;
    string severity;

    cout << "========================================================\n";
    cout << "                    RESCUE ROUTE\n";
    cout << "       Emergency and Disaster Management System\n";
    cout << "========================================================\n\n";

    location = getValidLocation();

    severity = getSeverity();

    string emergencyID = "401";

    string ambulanceID =
        "[ AMBULANCE ID GOES HERE ]";

    string ambulanceStatus =
        "ASSIGNED";

    string hospitalName =
        "[ HOSPITAL NAME GOES HERE ]";

    string availableCapacity =
        "[ AVAILABLE CAPACITY GOES HERE ]";

    string hospitalStatus =
        "AVAILABLE";

    string path =
        "[BFS PATH GOES HERE ]";

    string roadSegments =
        "[ ROAD SEGMENTS GO HERE ]";

    string totalDistance =
        "[ DISTANCE GOES HERE ]";

    cout << "\n\n";

    cout << "========================================================\n";
    cout << "                    RESCUE ROUTE\n";
    cout << "       Emergency and Disaster Management System\n";
    cout << "========================================================\n\n";

    cout << "CURRENT EMERGENCY\n";
    cout << "--------------------------------------------------------\n";

    cout << "Emergency ID       : "
         << emergencyID << "\n";

    cout << "Location           : "
         << location << "\n";

    cout << "Severity           : "
         << severity << "\n";

    cout << "Status             : ACTIVE\n\n";

    cout << "AMBULANCE\n";
    cout << "--------------------------------------------------------\n";

    cout << "Assigned ID        : "
         << ambulanceID << "\n";

    cout << "Status             : "
         << ambulanceStatus << "\n\n";

    cout << "HOSPITAL\n";
    cout << "--------------------------------------------------------\n";

    cout << "Name               : "
         << hospitalName << "\n";

    cout << "Available Capacity : "
         << availableCapacity << "\n";

    cout << "Status             : "
         << hospitalStatus << "\n\n";

    cout << "ROUTE\n";
    cout << "--------------------------------------------------------\n";

    cout << "Path               : "
         << path << "\n";

    cout << "Road Segments      : "
         << roadSegments << "\n";

    cout << "Total Distance     : "
         << totalDistance << "\n\n";

    cout << "========================================================\n";
    cout << "       Kindly stay where you are, help is on the way.\n";
    cout << "           We highly appreciate your patience.\n";
    cout << "========================================================\n";

    return 0;
}
