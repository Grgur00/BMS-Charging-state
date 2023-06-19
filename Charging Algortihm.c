#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#define BMS_CAN_ID 0x1806E5F4 // ID of message we send to Charger with target Voltage, Current and charging state
#define CCS_CAN_ID 0x18FF50E5 // ID of message we should recive from Charger with it's current Temperature, Charging Current, Charging Voltage and SoC 

#define SOC_THRESHOLD 90
#define CAN_MSG_DLC 8
#define CHARGING_STATE_OFF 1
#define CHARGING_STATE_ON 0

/*Data indexes for message BMS sends to Charger*/
#define MAX_VOLTAGE_HIGH_BYTE_INDEX 0
#define MAX_VOLTAGE_LOW_BYTE_INDEX 1
#define MAX_CURRENT_HIGH_BYTE_INDEX 2
#define MAX_CURRENT_LOW_BYTE_INDEX 3
#define CHARGING_STATE_INDEX 4

/*Data indexes for message BMS receives from Charger*/
#define DATA_VOLTAGE_HIGH_BYTE_INDEX 0
#define DATA_VOLTAGE_LOW_BYTE_INDEX 1
#define DATA_CURRENT_HIGH_BYTE_INDEX 2
#define DATA_CURRENT_LOW_BYTE_INDEX 3
#define DATA_STATUS_FLAGS_INDEX 4 

/*Indexes of flags received form Charger*/
#define HARDWARE_FAILURE_FLAG 0
#define TEMPERATURE_OF_CHARGER_FLAG 1
#define INPUT_VOLTAGE_FLAG 2
#define STARTING_STATE_FLAG 3
#define COMMUNICATION_STATE_FLAG 4

/*Definition of flags*/
/*Hardware Faliure flags*/
#define NORMAL 0
#define HARDWARE_FALIURE 1
/*Temperature of Charger flags*/
#define NORMAL_TEMPERATURE 0
#define OVER_TEMPERATURE_PROTECTION 1
/*Input Voltage flags*/
#define NORMAL_INPUT_VOLTAGE 0
#define WRONG_INPUT_VOLTAGE 1
/*Starting State flags*/
#define BATTERY_DETECTED 0
#define BATTERY_NOT_DETECTED 1
/*Communication State flags*/
#define NORMAL_COMMUNICATION 0
#define COMMUNACTION_TIMED_OUT 1

uint8_t chargerFlags; //flag byte received from Charger

/*Variables sent to Charger*/
float targetMaxCurrent = 58.2;  
float targetMaxVoltage = 320.1; 

/*Variables received from Charger */
float chargingCurrent;
float chargingVoltage;

/*Variables received from different sources*/
float soc;
float hottestCellTemperature;


void SendCANMessage(unsigned long id, uint8_t* data, int length) {

}

uint8_t ReceiveChargerCANMessage(unsigned long id){

}

int GetChargingState(float soc, float temperature, float chargingCurrent, float chargingVoltage){
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

void ChargingStateAlgorithm() {
    
    receiveChargerCANMessage(CCS_CAN_ID);
    
    targetMaxCurrent = calculateChargingCurrent(hottestCellTemperature, targetMaxVoltage);

    int chargingState = getChargingState(soc, hottestCellTemperature, chargingCurrent, chargingVoltage);
    
    uint8_t CAN_data[CAN_MSG_DLC];
    EncodeDataForCAN(CAN_data);

    if(chargingState == CHARGING_STATE_ON){
        CAN_data[CHARGING_STATE_INDEX] = CHARGING_STATE_ON;
        SendCANMessage(BMS_CAN_ID, CAN_data, CAN_MSG_DLC);
    }

    else{
        CAN_data[CHARGING_STATE_INDEX] = CHARGING_STATE_OFF;
        SendCANMessage(BMS_CAN_ID, CAN_data, CAN_MSG_DLC);
    }

}

void EncodeDataForCAN(uint8_t *dataForCAN){

    uint16_t voltageInt = (uint16_t)floor((targetMaxVoltage * 10));
    uint16_t currentInt = (uint16_t)floor((targetMaxCurrent * 10));

    uint8_t voltageShort = (uint8_t)voltageInt;
    uint8_t currentShort = (uint8_t)currentInt;


    dataForCAN[MAX_VOLTAGE_HIGH_BYTE_INDEX] = (voltageShort >> 8) & 0xFF;
    dataForCAN[MAX_VOLTAGE_LOW_BYTE_INDEX] = voltageShort & 0xFF;
    dataForCAN[MAX_CURRENT_HIGH_BYTE_INDEX] = (currentShort >> 8) & 0xFF;
    dataForCAN[MAX_CURRENT_LOW_BYTE_INDEX] = currentShort & 0xFF;
}


void DecodeChargerMessage(uint8_t *dataFromCharger){

    uint16_t voltageInt = ((uint16_t)dataFromCharger[DATA_VOLTAGE_HIGH_BYTE_INDEX] << 8) | dataFromCharger[DATA_VOLTAGE_LOW_BYTE_INDEX];
    uint16_t currentInt = ((uint16_t)dataFromCharger[DATA_CURRENT_HIGH_BYTE_INDEX] << 8) | dataFromCharger[DATA_CURRENT_LOW_BYTE_INDEX];

    chargingVoltage = voltageInt * 0.1;
    chargingCurrent = currentInt * 0.1;

    chargerFlags = dataFromCharger[DATA_STATUS_FLAGS_INDEX];
}


bool IsHardwareFaluireFlagSet(uint8_t chargerFlags){
    if ((chargerFlags & HARDWARE_FAILURE_FLAG) == HARDWARE_FAILURE_FLAG){
        return HARDWARE_FALIURE; // Hardware faliure flag is set
    }

    return NORMAL; //Hardware faliure flag is not set
}

bool IsTemperatureFlagSet(uint8_t chargerFlags) {
    if ((chargerFlags & TEMPERATURE_OF_CHARGER_FLAG) == TEMPERATURE_OF_CHARGER_FLAG) {
        return OVER_TEMPERATURE_PROTECTION; // Temperature flag is set
    }

    return NORMAL_TEMPERATURE; // Temperature flag is not set
}

bool IsInputVoltageFlagSet(uint8_t chargerFlags) {
    if ((chargerFlags &  INPUT_VOLTAGE_FLAG) == INPUT_VOLTAGE_FLAG) {
        return WRONG_INPUT_VOLTAGE; // Input voltage flag is set
    }

    return NORMAL_INPUT_VOLTAGE; // Input voltage flag is not set
}

bool IsStartingStateFlagSet(uint8_t chargerFlags) {
    if ((chargerFlags & STARTING_STATE_FLAG) == STARTING_STATE_FLAG) {
        return BATTERY_NOT_DETECTED; // Starting state flag is set
    }

    return BATTERY_DETECTED; // Starting state flag is not set
}

bool IsCommunicationFlagSet(uint8_t chargerFlags) {
    if ((chargerFlags & COMMUNICATION_STATE_FLAG)== COMMUNICATION_STATE_FLAG) {
        return COMMUNACTION_TIMED_OUT; // Communication flag is set
    }

    return NORMAL_COMMUNICATION; // Communication flag is not set
}

int calculateChargingCurrent(int temperature, int voltage){
} 

int main(){
}