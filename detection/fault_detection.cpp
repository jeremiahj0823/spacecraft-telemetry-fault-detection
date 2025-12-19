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
    const double v_loss_threshold = 27.5;
    const int v_persist_s = 10;
    const double brownout_threshold = 21.0;
    const int heartbeat_loss_s = 2; 

    bool heartbeat_loss = false;
    bool brownout = false;
    bool v_loss = false;

    int heartbeat_missing_counter = 0;
    int v_loss_counter = 0;

    int severity = 0;

    size_t fieldNum = 5;

    ifstream inFile("data/telemetry_data.csv");
    if (!inFile.is_open()) {
        cout << "Could not open the file." << endl;
        return 1;
    }

    ofstream outFile("data/telemetry_flags.csv");
    if (!outFile) {
        cout << "Could not output file." << endl;
        return 1;
    }

    outFile << "time_s,v_loss,brownout,heartbeat_loss,severity\n";

    string line;
    getline(inFile, line); // skip the header

    while (getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }
        vector<string> fields = split_csv_line(line);
        if (fields.size() != fieldNum) {
            continue;
        }

        int t;
        double panel_v;
        double battery_pct;
        double temp_c;
        int heartbeat;

        try {
            t = stoi(fields[0]);
            panel_v = stod(fields[1]);
            battery_pct = stod(fields[2]);
            temp_c = stod(fields[3]);
            heartbeat = stoi(fields[4]);
        }
        catch(...) {
            continue;
        }

        if (!heartbeat_loss) {
            if (heartbeat == 0) {
                heartbeat_missing_counter++;
            }
            else {
                heartbeat_missing_counter = 0;
            }
            if (heartbeat_missing_counter >= heartbeat_loss_s) {
                heartbeat_loss = true;
                cout << "Heartbeat loss detected at t = " << t << endl;
            }
        }

        if (!brownout && panel_v <= brownout_threshold) {
            brownout = true;
            cout << "Brownout detected at t = " << t << endl;
        }

        if (!v_loss) {
            if (panel_v < v_loss_threshold) {
                v_loss_counter++;
            }
            else {
                v_loss_counter = 0;
            }
            if (v_loss_counter >= v_persist_s) {
                v_loss = true;
                cout << "Panel voltage loss detected at t = " << t << endl;
            }
        }

        if (heartbeat_loss) {
            severity = 3;
        }
        else if (brownout) {
            severity = 2;
        }
        else if (v_loss) {
            severity = 1;
        }
        else {
            severity = 0;
        }

        outFile << t << "," << (int)v_loss << "," << (int)brownout << "," << (int)heartbeat_loss << "," << severity << "\n";
    }

    inFile.close();
    outFile.flush();
    outFile.close();

    return 0;
}