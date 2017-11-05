// rf22_mesh_server1.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, routed reliable messaging server
// with the RHMesh class.
// It is designed to work with the other examples rf22_mesh_*
// Hint: you can simulate other network topologies by setting the
// RH_TEST_NETWORK define in RHRouter.h

// Mesh has much greater memory requirements, and you may need to limit the
// max message length to prevent wierd crashes
#define RH_MESH_MAX_MESSAGE_LEN 50

#include <RHMesh.h>
#include <RH_RF95.h>

// In this small artifical network of 4 nodes,

#define SERVER1_ADDRESS 1
#define SERVER2_ADDRESS 2
#define SERVER3_ADDRESS 3

#define STATUS_PIN 7

// Singleton instance of the radio driver
RH_RF95 driver;

// Class to manage message delivery and receipt, using the driver declared above
RHMesh manager(driver, SERVER1_ADDRESS);

void setup()
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("RF96W init failed");
  // Defaults after init are 434.0MHz, 0.05MHz AFC pull-in, modulation FSK_Rb2_4Fd36
  driver.setTxPower(23, false);
  pinMode(STATUS_PIN, OUTPUT);
}

uint8_t data[] = "This is a message from SERVER1";
// Dont put this on the stack:
uint8_t buf[RH_MESH_MAX_MESSAGE_LEN];

void loop()
{

  if (manager.sendtoWait(data, sizeof(data), SERVER2_ADDRESS) == RH_ROUTER_ERROR_NONE)
  {
    // It has been reliably delivered to the next node.
    // Now wait for a reply from the ultimate server
    uint8_t len = sizeof(buf);
    uint8_t from;
    blink();
    if (manager.recvfromAckTimeout(buf, &len, 3000, &from))
    {
      Serial.print("SERVER1 got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is rf22_mesh_server1, rf22_mesh_server2 and rf22_mesh_server3 running?");
    }
  }
  else{
     Serial.println("sendtoWait failed. Are the intermediate mesh servers running?");
 }

}

void blink(){
    digitalWrite(STATUS_PIN, HIGH);
    delay(20);
    digitalWrite(STATUS_PIN, LOW);
}
