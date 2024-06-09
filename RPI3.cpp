/*
 * See documentation at https://nRF24.github.io/RF24
 * See License information at root directory of this library
 * Author: Brendan Doherty (2bndy5)
 */

/**
 * A simple example of sending data from 1 nRF24L01 transceiver to another.
 *
 * This example was written to be used on 2 devices acting as "nodes".
 * Use `ctrl+c` to quit at any time.
 */
#include <ctime>       // time()
#include <iostream>    // cin, cout, endl
#include <string>      // string, getline()
#include <time.h>      // CLOCK_MONOTONIC_RAW, timespec, clock_gettime()
#include <RF24/RF24.h> // RF24, RF24_PA_LOW, delay()

using namespace std;

/****************** Linux ***********************/
// Radio CE Pin, CSN Pin, SPI Speed
// CE Pin uses GPIO number with BCM and SPIDEV drivers, other platforms use their own pin numbering
// CS Pin addresses the SPI bus number at /dev/spidev<a>.<b>
// ie: RF24 radio(<ce_pin>, <a>*10+<b>); spidev1.0 is 10, spidev1.1 is 11 etc..
#define CSN_PIN 0
#ifdef MRAA
    #define CE_PIN 15 // GPIO22
#else
    #define CE_PIN 22
#endif
// Generic:
RF24 radio(CE_PIN, CSN_PIN);
/****************** Linux (BBB,x86,etc) ***********************/
// See http://nRF24.github.io/RF24/pages.html for more information on usage
// See http://iotdk.intel.com/docs/master/mraa/ for more information on MRAA
// See https://www.kernel.org/doc/Documentation/spi/spidev for more information on SPIDEV

float payload = -1; //

void setRole(); // set the node's role
void master();  // TX node's behavior
void slave();   // RX node's behavior

// custom defined timer for evaluating transmission time in microseconds
struct timespec startTimer, endTimer;
uint32_t getMicros(); // prototype to get elapsed time in microseconds

int main(int argc, char** argv)
{

    // perform hardware check
    if (!radio.begin()) {
        cout << "radio hardware is not responding!!" << endl;
        return 0; // quit now
    }

    // A variable to uniquely identify which address this radio will use to transmit and receive
    bool radioNumber = 1; // 0 uses address[0] to transmit, 1 uses address[1] to transmit and the other one uses to receive

    // print program's name
    cout << argv[0] << endl;

    // Let these addresses be used for the pair
    uint8_t address[2][6] = {"LIFT1", "SPEAK"};

    // Set the radioNumber via the terminal on startup. It will change the transmit direction. 1 for LIFT to BUS, 0 for BUS to LIFT
    string input;
    input.assign(argv[1]);
    radioNumber = input.length() > 0 && (uint8_t)input[0] == 49;

    // save on transmission time by setting the radio to only transmit the
    // number of bytes we need to transmit a float
    radio.setPayloadSize(sizeof(payload)); // float datatype occupies 4 bytes

    // Set the PA Level low to try preventing power supply related problems
    // because these examples are likely run with nodes in close proximity to
    // each other.
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.


    radio.openWritingPipe(address[radioNumber]); // using pipe 0
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1

    // For debugging info
    // radio.printDetails();       // (smaller) function that prints raw register values
    // radio.printPrettyDetails(); // (larger) function that prints human readable data

    // ready to execute program now
    setRole(); // calls master() and slave() one after another
    return 0;
}

/**
 * Role of this program will pingpong.
 */
void setRole()
{
    while(1){
        master();
        slave();
    }
} // setRole()

/**
 * make this node act as the transmitter
 */
void master()
{
    radio.stopListening(); // put radio in TX mode

    cout << "Start TX role." << endl;

    unsigned int mcount = 0; // keep track of master time
    while (mcount < 2 && payload != 1) { // if not received any values since last transmit no transmition
        clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);    // start the timer
        bool report = radio.write(&payload, sizeof(float)); // transmit & save the report
        uint32_t timerElapsed = getMicros();                // end the timer

        if (report) {
            // payload was delivered
            cout << "Transmission successful! Time to transmit = ";
            cout << timerElapsed;                     // print the timer result
            cout << " us. Sent: " << payload << endl; // print payload sent
            if(mcount >= 2){
                payload = -1;
            }
        }
        else {
            // payload was not delivered
            cout << "Transmission failed or timed out" << endl;
        }
        mcount++;
        // to make this program readable in the terminal
        delay(1000); // slow transmissions down by 1 second
    }
    cout << "Leaving TX role.\n" << endl;
}

/**
 * make this node act as the receiver
 */
void slave()
{

    radio.startListening(); // put radio in RX mode

    cout << "Start RX role." << endl;

    time_t startTimer = time(nullptr);       // start a timer
    while (time(nullptr) - startTimer < 6) { // use 6 second timeout
        uint8_t pipe;
        if (radio.available(&pipe)) {                        // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize();          // get the size of the payload
            radio.read(&payload, bytes);                     // fetch payload from FIFO
            cout << "Received ";                             // print the size of the payload
            cout << ": " << payload << endl;                 // print the payload's value
        }
    } //After 6 seconds of listening convert to transmiter again
    cout << "Leaving RX role.\n" << endl;
    radio.stopListening();
}

/**
 * Calculate the elapsed time in microseconds
 */
uint32_t getMicros()
{
    // this function assumes that the timer was started using
    // `clock_gettime(CLOCK_MONOTONIC_RAW, &startTimer);`

    clock_gettime(CLOCK_MONOTONIC_RAW, &endTimer);
    uint32_t seconds = endTimer.tv_sec - startTimer.tv_sec;
    uint32_t useconds = (endTimer.tv_nsec - startTimer.tv_nsec) / 1000;

    return ((seconds)*1000 + useconds) + 0.5;
}
