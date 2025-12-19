#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

vector<string> split_csv_line(const string& line) {
    vector<string> fields;
    string field;
    stringstream ss(line);
    while (getline(ss, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

int main() {
    ifstream inFile("data/telemetry_flags.csv");
    if (!inFile.is_open()) {
        cout << "Could not open the file." << endl;
        return 1;
    }

    ofstream outFile("data/state_log.csv");
    if (!outFile) {
        cout << "Could not output file." << endl;
        return 1;
    }

    bool nominal = true;
    bool degraded = false;
    bool safemode = false;

    string reason = "NONE";
    string mode = "NONE";

    size_t fieldNum = 8;

    outFile << "time_s,mode,reason\n";

    string line;
    getline(inFile, line);

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        vector<string> fields = split_csv_line(line);
        if (fields.size() != fieldNum) {
            continue;
        }

        int time_s = stoi(fields[0]);
        int v_loss = bool(stoi(fields[1]));
        int brownout = bool(stoi(fields[2]));
        int overheat = bool(stoi(fields[3]));
        int battery_low = bool(stoi(fields[4]));
        int battery_critical = bool(stoi(fields[5]));
        int heartbeat_loss = bool(stoi(fields[6]));
        int severity = stoi(fields[7]);

        reason = "NONE";
        if (heartbeat_loss) {
            reason = "HEARTBEAT_LOSS";
        }
        else if (brownout) {
            reason = "BROWNOUT";
        }
        else if (overheat) {
            reason = "OVERHEAT";
        }
        else if (battery_critical) {
            reason = "BATTERY_CRITICAL";
        }
        else if (battery_low) {
            reason = "BATTERY_LOW";
        }
        else if (v_loss) {
            reason = "VOLTAGE_LOSS";
        }

        if (nominal) {
            if (severity > 0) {
                if (heartbeat_loss || brownout || overheat || battery_critical) {
                    safemode = true;
                    cout << "NOMINAL -> SAFE_MODE" << endl;
                }
                else {
                    degraded = true;
                    cout << "NOMINAL -> DEGRADED" << endl;
                }
                nominal = false;
            }
        }
        else if (degraded) {
            if (!battery_low && !v_loss) {
                nominal = true;
                degraded = false;
                cout << "DEGRADED -> NOMINAL" << endl;
            }
            else if (heartbeat_loss || brownout || overheat || battery_critical) {
                safemode = true;
                degraded = false;
                cout << "DEGRADED -> SAFE_MODE" << endl;
            }
        }
        else if (safemode) {
            // Stay in safe mode!
        }

        if (nominal) {
            mode = "NOMINAL";
        }
        else if (degraded) {
            mode = "DEGRADED";
        }
        else if (safemode) {
            mode = "SAFE_MODE";
        }

        outFile << time_s << "," << mode << "," << reason << "\n";
    }
    inFile.close();
    outFile.flush();
    outFile.close();
    
    return 0;
}