#include "../include/supervisorProgram.h"
#include "../include/pcanFunctions.h"

#include "pcanFunctions.h"
#include <fcntl.h>
#include <libpcan.h> // PCAN library
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define DOOR_OPEN_TIME 5

static HANDLE h2;
static HANDLE h_TX;
static HANDLE h_RX;
static pthread_t door_open_timer_thread;
static bool are_doors_open = false;

static void *door_timer(void *unused) {
    are_doors_open = true;
    printf("%s: doors are open\n", __func__);
    sleep(DOOR_OPEN_TIME);
    printf("%s: closing doors\n", __func__);
    are_doors_open = false;
}

// TODO: add error check to can init and can status
static bool init_can_bus() {
    int status = 0;
    // Open a CAN channel
    h_TX = LINUX_CAN_Open("/dev/pcanusb32", O_RDWR);
    h_RX = LINUX_CAN_Open("/dev/pcanusb32", O_RDWR); // Open PCAN channel

    // Initialize an opened CAN 2.0 channel with a 125kbps bitrate, accepting
    // standard frames
    status = CAN_Init(h_TX, CAN_BAUD_125K, CAN_INIT_TYPE_ST);
    status = CAN_Init(h_RX, CAN_BAUD_125K, CAN_INIT_TYPE_ST);

    // Clear the channel - new - Must clear the channel before Tx/Rx
    status = CAN_Status(h_TX);
    status = CAN_Status(h_RX);

    return true;
}

static void can_rx_data(TPCANMsg *incoming_can_msg) {
    int status = 0;
    // Clear the channel - new - Must clear the channel before Tx/Rx
    status = CAN_Status(h_RX);

    printf("Can status: %d\n", status);
    while ((status = CAN_Read(h_RX, incoming_can_msg)) ==
           PCAN_RECEIVE_QUEUE_EMPTY) {
        sleep(1);
    }
    if (status != PCAN_NO_ERROR) { // If there is an error, display the code
        printf("Error 0x%x\n", (int)status);
    }
}

static void send_can_msg(int id, int data) {
    int status = 0;

    // Clear the channel - new - Must clear the channel before Tx/Rx
    status = CAN_Status(h_TX);

    printf("Can status: %d\n", status);
    TPCANMsg Txmsg;
    printf("%s sending ID %x and FLOOR: %x\n", __func__, id, data);
    Txmsg.ID = id;
    Txmsg.MSGTYPE = MSGTYPE_STANDARD;
    Txmsg.LEN = 1;
    Txmsg.DATA[0] = data;

    sleep(1);
    CAN_Write(h_TX, &Txmsg);

    printf("%s sending ID %x and FLOOR: %x\n", __func__, id, data);
}

bool supervisor_program() {
    TPCANMsg can_msg;
    if (!init_can_bus()) {
        return false;
    }
    for (;;) {
        can_rx_data(&can_msg);
        if (are_doors_open) {
            continue;
        } else {
            printf("  - R ID:%4x LEN:%1x DATA:%02x \n", // Display the CAN message
                   (int)can_msg.ID, (int)can_msg.LEN, (int)can_msg.DATA[0]);
        }
        switch (can_msg.ID) {
        case ID_F1_TO_SC: {
            if (can_msg.DATA[0] == 0x01) {
                send_can_msg(ID_SC_TO_EC, GO_TO_FLOOR1);
            }
            break;
        }
        case ID_F2_TO_SC: {
            if (can_msg.DATA[0] == 0x01) {
                send_can_msg(ID_SC_TO_EC, GO_TO_FLOOR2);
            }
            break;
        }
        case ID_F3_TO_SC: {
            if (can_msg.DATA[0] == 0x01) {
                send_can_msg(ID_SC_TO_EC, GO_TO_FLOOR3);
            }
            break;
        }
        case ID_CC_TO_SC: {
            switch (can_msg.DATA[0]) {
            case GO_TO_FLOOR1: {
                send_can_msg(ID_SC_TO_EC, GO_TO_FLOOR1);
                break;
            }
            case GO_TO_FLOOR2: {
                send_can_msg(ID_SC_TO_EC, GO_TO_FLOOR2);
                break;
            }
            case GO_TO_FLOOR3: {
                send_can_msg(ID_SC_TO_EC, GO_TO_FLOOR3);
                break;
            }
            case MSG_OPEN: {
                pthread_create(&door_open_timer_thread, NULL, door_timer, NULL);
                break;
            }
            default: {
                printf("unable to parse car ID: %x, Data: %x", can_msg.ID,
                       can_msg.DATA[0]);
                break;
            }
            }
            break;
        }
        default: { break; }
        }
    }
    return true;
}
