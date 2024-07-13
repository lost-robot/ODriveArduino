#pragma once

#include "ESP32SJA1000.h"
#include "ODriveCAN.h"

// Convenience struct for CAN messages since the MCP2515 library lacks a native message type.
struct CanMsg 
{
    uint32_t id;
    uint8_t len;
    uint8_t buffer[8];
};

// Must be defined by the application to use defaultCanReceiveCallback().
void onCanMessage(const CanMsg& msg);

// Sends a CAN message through the specified CAN interface.
// static bool sendMsg(ESP32SJA1000Class& can_intf, uint32_t id, uint8_t length, const uint8_t* data) 
// {
//     if (id & 0x80000000) 
//     {
//         can_intf.beginExtendedPacket(id & 0x1FFFFFFF, length, !data);
//     } 
//     else 
//     {
//         can_intf.beginPacket(id, length, !data);
//     }
    
//     if (data) 
//     {
//         for (int i = 0; i < length; ++i) 
//         {
//             can_intf.write(data[i]);
//         }
//     }

//     return can_intf.endPacket();
// }

static bool sendMsg(ESP32SJA1000Class& can_intf, uint32_t id, uint8_t length, const uint8_t* data) 
{
    // Ensure length is within valid CAN message length (0-8 bytes)
    if (length > 8) {
        return false;
    }

    // Determine packet type and begin the packet
    if (id & 0x80000000) 
    {
        // Extended packet
        can_intf.beginExtendedPacket(id & 0x1FFFFFFF, length, data == nullptr);
    } 
    else 
    {
        // Standard packet
        can_intf.beginPacket(id, length, data == nullptr);
    }

    // Write data if provided
    if (data) 
    {
        for (uint8_t i = 0; i < length; ++i) 
        {
            can_intf.write(data[i]);
        }
    }

    // Finalize and send the packet
    return can_intf.endPacket();
}

// Callback function to handle received CAN messages and pass them to the ODriveCAN instance.
static void onReceive(const CanMsg& msg, ODriveCAN& odrive) 
{
    odrive.onReceive(msg.id, msg.len, msg.buffer);
}

// Pump events for the specified CAN interface.
static void pumpEvents(ESP32SJA1000Class& intf) 
{
    // TODO: Investigate the cause of fewer dropped messages with this delay.
    // delay(10);
}

// Macro to create a CAN interface wrapper for ESP32SJA1000Class.
CREATE_CAN_INTF_WRAPPER(ESP32SJA1000Class)
