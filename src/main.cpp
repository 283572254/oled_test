#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// The remote service we wish to connect to.
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static boolean doConnect = false;
static boolean connected = false;
static BLEAddress* pServerAddress;
static BLERemoteCharacteristic* pRemoteCharacteristic;

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    connected = true;
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
  }
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveName() && advertisedDevice.getName() == "ESP32") {
      BLEDevice::getScan()->stop();
      pServerAddress = new BLEAddress(advertisedDevice.getAddress());
      doConnect = true;
    }
  }
};

bool connectToServer() {
  BLEClient*  pClient  = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());

  // Connect to the remote BLE Server.
  if (pClient->connect(*pServerAddress)) {
    Serial.println("Connected to server");
    
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.println("Failed to find our service UUID");
      return false;
    }

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.println("Failed to find our characteristic UUID");
      return false;
    }

    // Read the value of the characteristic.
    if (pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    connected = true;
    return true;
  } else {
    return false;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  BLEDevice::getScan()->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  BLEDevice::getScan()->start(5);
}

void loop() {
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("Failed to connect to the BLE Server.");
    }
    doConnect = false;
  }

  delay(1000);
}
