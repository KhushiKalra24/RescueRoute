#ifndef AMBULANCE_HOSPITAL_MODULE_H
#define AMBULANCE_HOSPITAL_MODULE_H

#include <string>
#include <vector>

class Ambulance {
private:
    int id;
    std::string name;
    int currentNode;
    std::string status;

public:
    Ambulance();
    Ambulance(int id, const std::string& name, int node,
              const std::string& status);

    int getId() const;
    const std::string& getName() const;
    int getCurrentNode() const;
    const std::string& getStatus() const;

    void setCurrentNode(int node);
    void setStatus(const std::string& newStatus);

    bool isAvailable() const;
};

class Hospital {
private:
    int id;
    std::string name;
    int nodeId;
    int totalCapacity;
    int availableCapacity;
    std::string status;

public:
    Hospital();
    Hospital(int id, const std::string& name, int node,
             int total, int available, const std::string& status);

    int getId() const;
    const std::string& getName() const;
    int getNodeId() const;
    int getTotalCapacity() const;
    int getAvailableCapacity() const;
    const std::string& getStatus() const;

    bool hasCapacity() const;
    void admitPatient();
    void dischargePatient();
};

class AmbulanceHospitalModule {
private:
    std::vector<Ambulance> ambulances;
    std::vector<Hospital> hospitals;

public:
    // CSV file handling
    bool loadAmbulances(const std::string& filename);
    bool loadHospitals(const std::string& filename);

    bool saveAmbulances(const std::string& filename) const;
    bool saveHospitals(const std::string& filename) const;

    // Ambulance management
    Ambulance* findAvailableAmbulance(int patientNode);
    bool assignAmbulance(int patientNode, int& ambulanceId);
    bool releaseAmbulance(int ambulanceId);
    void displayAmbulances() const;

    // Hospital management
    Hospital* findAvailableHospital();
    bool assignHospital(int& hospitalId);
    bool dischargePatient(int hospitalId);
    void displayHospitals() const;

    // Useful for Emergency module / UI
    int getAvailableAmbulanceCount() const;
    int getAvailableHospitalCount() const;
};

#endif
