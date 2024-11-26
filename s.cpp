
#include <iostream>
#include <cstdlib>

using namespace std;

int main() {

    // Run `globus login` before you run this program.

    // Change 785fc086-9e01-11ef-866f-73df89a31e54 to your UUID.
    int result = system("globus transfer 82f1b5c6-6e9b-11e5-ba47-22000b92c6ec:afrl-challenge-data/musinski_afrl_am_package_v2.1/Calibration\\ Data/Strain\\ Data/A46_xx.csv 785fc086-9e01-11ef-866f-73df89a31e54:/~/data/A46_xx.csv");

    if (result == 0) {
        cout << "Command executed successfully." << endl;
    } else {
        cerr << "Error executing command." << endl;
    }

    return 0;
}
