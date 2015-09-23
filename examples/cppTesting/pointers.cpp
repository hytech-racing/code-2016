#include <iostream>
#include <cstdlib>

using namespace std;
int main() {
    char* sent = (char*)malloc(64);
    uint8_t rxError = 5;
    uint8_t txError = 2;
    uint16_t motor = 0;
    uint16_t flags = 39;
    uint16_t otherFlags = 292;

    *sent = rxError;
    *(sent + 8) = txError;
    *(sent + 2*8) = motor;
    *(sent + 2*8 + 16) = flags;
    *(sent + 2*8 + 2*16) = otherFlags;

    char* buf = (char*)sent;

    uint16_t recFlags = (uint16_t)(*(buf + 2*8 + 16));
    cout << recFlags << endl;

    free(sent);
}
