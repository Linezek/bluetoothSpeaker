#include "mbed.h"
#include "ble/BLE.h"

uint16_t customServiceUUID  = 0xA000;
uint16_t readCharUUID       = 0xA001;
uint16_t writeCharUUID      = 0xA002;



class MyService {
public:
    
    /**
     * Instantiate a battery service.
     *
     * The construction of a BatteryService adds a GATT battery service in @p
     * _ble GattServer and sets the initial charge level of the battery to @p
     * level.
     *
     * @param[in] _ble BLE device which will host the battery service.
     * @param[in] level Initial charge level of the battery. It is a percentage
     * where 0% means that the battery is fully discharged and 100% means that
     * the battery is fully charged.
     */
    MyService(BLE &_ble, PwmOut &_speaker) :
        ble(_ble),
        speaker(_speaker),
        writeChar(writeCharUUID, writeValue)
    {
        /* Set Up custom Characteristics */
        static uint8_t readValue[1] = {0};
        ReadOnlyArrayGattCharacteristic<uint8_t, sizeof(readValue)> readChar(readCharUUID, readValue);
          
        /* Set up custom service */
        GattCharacteristic *characteristics[] = {&readChar, &writeChar};
        GattService        customService(customServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));
     
        ble.gattServer().onDataWritten(this, &MyService::writeCharCallback);
        ble.addService(customService);
    }

    /**
     * Update the battery charge level that the service exposes.
     *
     * The server sends a notification of the new value to clients that have
     * subscribed to the battery level characteristic updates, and clients
     * reading the charge level after the update obtain the updated value.
     *
     * @param newLevel Charge level of the battery. It is a percentage of the
     * remaining charge between 0% and 100%.
     *
     * @attention This function must be called in the execution context of the
     * BLE stack.
     */
    /*void updateBatteryLevel(uint8_t newLevel)
    {
        MBED_ASSERT(newLevel <= 100);
        batteryLevel = newLevel;
        ble.gattServer().write(
            batteryLevelCharacteristic.getValueHandle(),
            &batteryLevel,
            1
        );
    }*/
    
    void writeCharCallback(const GattWriteCallbackParams *params)
    {
        /* Check to see what characteristic was written, by handle */
        if(params->handle == writeChar.getValueHandle()) {
            /* toggle LED if only 1 byte is written */
            if(params->len == 1) {
                printf("Data received: length = %d, data = 0x",params->len);
                for(int x=0; x < params->len; x++) {
                    printf("%x", params->data[x]);
                }
                printf("\n\r");
                
                printf("%d\n\r", params->data[0]);
                
                float t = params->data[0];
                
                if (t < 1) {
                    speaker.write(0);   
                } else {                
                    t = (t / 255.0) * (8000.0 - 0) + 0;
                    note = t;
                    speaker.period(1.0 / note);
                    speaker = 0.5;                    
                }
            }
            /* Print the data if more than 1 byte is written */
            else {
                printf("Data received: length = %d, data = 0x",params->len);
                for(int x=0; x < params->len; x++) {
                    printf("%x", params->data[x]);
                }
                printf("\n\r");
            }
            /* Update the readChar with the value of writeChar */
            // BLE::Instance(BLE::DEFAULT_INSTANCE).gattServer().write(readChar.getValueHandle(), params->data, params->len);
        }
    }
    
    float get_note() {return note;}

protected:
    /**
     * Reference to the underlying BLE instance that this object is attached to.
     *
     * The services and characteristics are registered in the GattServer of
     * this BLE instance.
     */
    BLE &ble;

    /**
     * The current battery level represented as an integer from 0% to 100%.
     */
    //uint8_t batteryLevel;

    /**
     * The GATT characteristic, which exposes the charge level.
     */
    //ReadOnlyGattCharacteristic<uint8_t> batteryLevelCharacteristic;
    uint16_t writeValue[1];
    WriteOnlyArrayGattCharacteristic<uint16_t, sizeof(writeValue)> writeChar;
    float note;
    PwmOut speaker;
};