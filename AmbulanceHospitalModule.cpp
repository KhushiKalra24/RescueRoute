#include "AmbulanceHospitalModule.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

// ==================== AMBULANCE ====================

Ambulance::Ambulance()
    : id(0), name(""), currentNode(0), status("AVAILABLE") {}

Ambulance::Ambulance(int id, const string& name, int node,
                     const string& status)
    : id(id), name(name), currentNode(node), status(status) {}

int Ambulance::getId() const {
    return id;
}

const string& Ambulance::getName() const {
    return name;
}

int Ambulance::getCurrentNode() const {
    return currentNode;
}

const string& Ambulance::getStatus() const {
    return status;
}

void Ambulance::setCurrentNode(int node) {
    currentNode = node;
}

void Ambulance::setStatus(const string& newStatus) {
    status = newStatus;
}

bool Ambulance::isAvailable() const {
    return status == "AVAILABLE";
}

// ==================== HOSPITAL ====================

Hospital::Hospital()
    : id(0), name(""), nodeId(0), totalCapacity(0),
      availableCapacity(0), status("AVAILABLE") {}

Hospital::Hospital(int id, const string& name, int node,
                   int total, int available, const string& status)
    : id(id), name(name), nodeId(node), totalCapacity(total),
      availableCapacity(available), status(status) {}

int Hospital::getId() const {
    return id;
}

const string& Hospital::getName() const {
    return name;
}

int Hospital::getNodeId() const {
    return nodeId;
}

int Hospital::getTotalCapacity() const {
    return totalCapacity;
}

int Hospital::getAvailableCapacity() const {
    return availableCapacity;
}

const string& Hospital::getStatus() const {
    return status;
}

bool Hospital::hasCapacity() const {
    return availableCapacity > 0 && status == "AVAILABLE";
}

void Hospital::admitPatient() {
    if (availableCapacity <= 0) {
        throw runtime_error("Hospital has no available capacity.");
    }

    --availableCapacity;

    if (availableCapacity == 0) {
        status = "FULL";
    }
}

void Hospital::dischargePatient() {
    if (availableCapacity >= totalCapacity) {
        return;
    }

    ++availableCapacity;
    status = "AVAILABLE";
}

// ==================== AMBULANCE & HOSPITAL MODULE ====================

bool AmbulanceHospitalModule::loadAmbulances(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << '\n';
        return false;
    }

    ambulances.clear();

    string line;
    getline(file, line); // Header

    try {
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string idStr, name, nodeStr, status;

            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, nodeStr, ',');
            getline(ss, status, ',');

            if (idStr.empty()) continue;

            ambulances.emplace_back(
                stoi(idStr),
                name,
                stoi(nodeStr),
                status
            );
        }
    }
    catch (const exception& e) {
        cerr << "Error reading ambulance data: "
             << e.what() << '\n';
        return false;
    }

    return true;
}

bool AmbulanceHospitalModule::loadHospitals(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open " << filename << '\n';
        return false;
    }

    hospitals.clear();

    string line;
    getline(file, line); // Header

    try {
        while (getline(file, line)) {
            if (line.empty()) continue;

            stringstream ss(line);

            string idStr, name, nodeStr;
            string totalStr, availableStr, status;

            getline(ss, idStr, ',');
            getline(ss, name, ',');
            getline(ss, nodeStr, ',');
            getline(ss, totalStr, ',');
            getline(ss, availableStr, ',');
            getline(ss, status, ',');

            if (idStr.empty()) continue;

            hospitals.emplace_back(
                stoi(idStr),
                name,
                stoi(nodeStr),
                stoi(totalStr),
                stoi(availableStr),
                status
            );
        }
    }
    catch (const exception& e) {
        cerr << "Error reading hospital data: "
             << e.what() << '\n';
        return false;
    }

    return true;
}

Ambulance* AmbulanceHospitalModule::findAvailableAmbulance(
    int patientNode) {

    // First preference: ambulance at patient's location.
    for (auto& ambulance : ambulances) {
        if (ambulance.isAvailable() &&
            ambulance.getCurrentNode() == patientNode) {
            return &ambulance;
        }
    }

    // Otherwise use any available ambulance.
    for (auto& ambulance : ambulances) {
        if (ambulance.isAvailable()) {
            return &ambulance;
        }
    }

    return nullptr;
}

bool AmbulanceHospitalModule::assignAmbulance(
    int patientNode, int& ambulanceId) {

    Ambulance* ambulance = findAvailableAmbulance(patientNode);

    if (ambulance == nullptr) {
        return false;
    }

    ambulance->setStatus("BUSY");
    ambulanceId = ambulance->getId();

    return true;
}

bool AmbulanceHospitalModule::releaseAmbulance(int ambulanceId) {
    for (auto& ambulance : ambulances) {
        if (ambulance.getId() == ambulanceId) {
            ambulance.setStatus("AVAILABLE");
            return true;
        }
    }

    return false;
}

void AmbulanceHospitalModule::displayAmbulances() const {
    cout << "\n========== AMBULANCE STATUS ==========\n";

    if (ambulances.empty()) {
        cout << "No ambulance records available.\n";
        return;
    }

    for (const auto& ambulance : ambulances) {
        cout << "ID: " << ambulance.getId()
             << " | Name: " << ambulance.getName()
             << " | Node: " << ambulance.getCurrentNode()
             << " | Status: " << ambulance.getStatus()
             << '\n';
    }
}

Hospital* AmbulanceHospitalModule::findAvailableHospital() {
    for (auto& hospital : hospitals) {
        if (hospital.hasCapacity()) {
            return &hospital;
        }
    }

    return nullptr;
}

bool AmbulanceHospitalModule::assignHospital(int& hospitalId) {
    Hospital* hospital = findAvailableHospital();

    if (hospital == nullptr) {
        return false;
    }

    try {
        hospital->admitPatient();
        hospitalId = hospital->getId();
        return true;
    }
    catch (const exception&) {
        return false;
    }
}

bool AmbulanceHospitalModule::dischargePatient(int hospitalId) {
    for (auto& hospital : hospitals) {
        if (hospital.getId() == hospitalId) {
            hospital.dischargePatient();
            return true;
        }
    }

    return false;
}

void AmbulanceHospitalModule::displayHospitals() const {
    cout << "\n========== HOSPITAL STATUS ==========\n";

    if (hospitals.empty()) {
        cout << "No hospital records available.\n";
        return;
    }

    for (const auto& hospital : hospitals) {
        cout << "ID: " << hospital.getId()
             << " | Name: " << hospital.getName()
             << " | Node: " << hospital.getNodeId()
             << " | Capacity: "
             << hospital.getAvailableCapacity()
             << "/" << hospital.getTotalCapacity()
             << " | Status: " << hospital.getStatus()
             << '\n';
    }
}

int AmbulanceHospitalModule::getAvailableAmbulanceCount() const {
    int count = 0;

    for (const auto& ambulance : ambulances) {
        if (ambulance.isAvailable()) {
            ++count;
        }
    }

    return count;
}

int AmbulanceHospitalModule::getAvailableHospitalCount() const {
    int count = 0;

    for (const auto& hospital : hospitals) {
        if (hospital.hasCapacity()) {
            ++count;
        }
    }

    return count;
}

bool AmbulanceHospitalModule::saveAmbulances(
    const string& filename) const {

    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not save " << filename << '\n';
        return false;
    }

    file << "ambulance_id,ambulance_name,current_node,status\n";

    for (const auto& ambulance : ambulances) {
        file << ambulance.getId() << ','
             << ambulance.getName() << ','
             << ambulance.getCurrentNode() << ','
             << ambulance.getStatus() << '\n';
    }

    return true;
}

bool AmbulanceHospitalModule::saveHospitals(
    const string& filename) const {

    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not save " << filename << '\n';
        return false;
    }

    file << "hospital_id,hospital_name,node_id,"
         << "total_capacity,available_capacity,status\n";

    for (const auto& hospital : hospitals) {
        file << hospital.getId() << ','
             << hospital.getName() << ','
             << hospital.getNodeId() << ','
             << hospital.getTotalCapacity() << ','
             << hospital.getAvailableCapacity() << ','
             << hospital.getStatus() << '\n';
    }

    return true;
}
