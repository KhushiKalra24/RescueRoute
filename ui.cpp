#include <iostream>
#include <string>

using namespace std;

int main()
{
    string startLocation;
    string destination;
    cout << "========================================================\n";
    cout << "                    RESCUE ROUTE\n";
    cout << "              Emergency Response System\n";
    cout << "========================================================\n\n";

    cout << "Starting Location\n";
    cout << "> ";
    getline(cin, startLocation);

    cout << "\nDestination\n";
    cout << "> ";
    getline(cin, destination);

    string emergencyID = "401";

    string ambulanceID = "AMB-01";
    string ambulanceStatus = "AVAILABLE";

    int totalBeds = 70;
    int availableBeds = 15;

    cout << "\n\n========================================================\n";
    cout << "                    RESCUE ROUTE\n";
    cout << "              Emergency Response System\n";
    cout << "========================================================\n\n";


    cout << "EMERGENCY DETAILS\n";
    cout << "--------------------------------------------------------\n";
    cout << "Emergency ID       : " << emergencyID << "\n";
    cout << "Starting Location  : " << startLocation << "\n";
    cout << "Destination        : " << destination << "\n\n";


    cout << "AMBULANCE\n";
    cout << "--------------------------------------------------------\n";
    cout << "Ambulance ID       : " << ambulanceID << "\n";
    cout << "Status             : " << ambulanceStatus << "\n\n";


    cout << "HOSPITAL\n";
    cout << "--------------------------------------------------------\n";
    cout << "Hospital           : " << destination << "\n";
    cout << "Total Bed Capacity : " << totalBeds << "\n";
    cout << "Available Beds     : " << availableBeds << "\n\n";


    cout << "ROUTE INFORMATION\n";
    cout << "--------------------------------------------------------\n";
    cout << "Path               : [ A* GENERATED PATH ]\n";
    cout << "Distance           : [ CALCULATED DISTANCE ]\n";
    cout << "Algorithm          : A*\n";
    cout << "Road Condition     : [ SIMULATED CONDITION ]\n";
    cout << "Status             : ROUTE FOUND\n\n";


    cout << "========================================================\n";

    return 0;
}