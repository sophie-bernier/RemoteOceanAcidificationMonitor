#include <proO.h>

ProCVData data;

void setup() {
  Serial.begin(9600);
  while (!Serial)
  {
    delay(1);
  }
  data.setDataFromString("W M,2020,01,17,00,20,00,55651,51716,532.59,40.00,5.60,0.90,0981,12.1");
  Serial.println();
  data.printAllData(Serial);
  Serial.println();
  data.convertReadableToBitfield();
  data.printAllBitfieldData(Serial);
  data.convertBitfieldToReadable();
  Serial.println();
  data.printAllData(Serial);
}

void loop() {
  // put your main code here, to run repeatedly:

}
