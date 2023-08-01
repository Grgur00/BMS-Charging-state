#include <math.h>
#define BMS_CAN_ID 0x1806E5F4
#define CCS_CAN_ID 0x18FF50E5
#define SOC_THRESHOLD 90
#define CAN_MSG_DLC 8
#define CHARGING_STATE_OFF 1
#define CHARGING_STATE_ON 0

#define MAX_VOLTAGE_HIGH_BYTE 0
#define MAX_VOLTAGE_LOW_BYTE 1
#define MAX_CURRENT_HIGH_BYTE 2
#define MAX_CURRENT_LOW_BYTE 3
#define CHARGING_STATE 4

float targetMaxCurrent = 58.2;
float targetMaxVoltage = 320.1;

void sendCANMessage(unsigned long id, byte* data, byte length) {

}

int getChargingState(float soc, float temperature, float chargingCurrent, float chargingVoltage){
    if (soc < SOC_THRESHOLD) {

        if (temperature > MAX_TEMPERATURE) {
        return CHARGING_STATE_OFF;
        }

        if (chargingCurrent > MAX_CHARGING_CURRENT){
        return CHARGING_STATE_OFF;
        }

        if(chargingVoltage > MAX_CHARGING_VOLTAGE) {
        return CHARGING_STATE_OFF;
        }

        else {
        return CHARGING_STATE_ON;
        }

    }

    else {
        return CHARGING_STATE_OFF;
    }

}

void chargingStateAlgorithm(float soc, float temperature, float chargingCurrent, float chargingVoltage) {

    int chargingState = getChargingState(soc, temperature, chargingCurrent, chargingVoltage);
    byte CAN_data[CAN_MSG_DLC];
    parseDataForCAN(CAN_data);

    if(chargingState == CHARGING_STATE_ON){
        CAN_data[CHARGING_STATE] = CHARGING_STATE_ON;
        sendCANMessage(BMS_CAN_ID, CAN_data, CAN_MSG_DLC);
    }

    else{
        CAN_data[CHARGING_STATE] = CHARGING_STATE_OFF;
        sendCANMessage(BMS_CAN_ID, CAN_data, CAN_MSG_DLC);
    }

}

void encodeDataForCAN(byte *dataForCAN){

    uint16_t voltageInt = (uint16_t)floor((targetMaxVoltage * 10));
    uint16_t currentInt = (uint16_t)floor((targetMaxCurrent * 10));

    uint8_t voltageShort = (uint8_t)voltageInt;
    uint8_t currentShort = (uint8_t)currentInt;


    dataForCAN[MAX_VOLTAGE_HIGH_BYTE] = (voltageShort >> 8) & 0xFF;
    dataForCAN[MAX_VOLTAGE_LOW_BYTE] = voltageShort & 0xFF;
    dataForCAN[MAX_CURRENT_HIGH_BYTE] = (currentShort >> 8) & 0xFF;
    dataForCAN[MAX_CURRENT_LOW_BYTE] = currentShort & 0xFF;
}

int main(){
}
