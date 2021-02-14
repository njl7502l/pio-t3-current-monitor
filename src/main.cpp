#include <Arduino.h>
#include <queue>
#include <MsTimer2.h>

class SimpleMovingAverage {
private:
  std::queue<int> voltageQueue;
  int voltageQueueSize = 100;
  int averageSum = 0;

public:
  void setVoltageQueueSize(int size) { voltageQueueSize = size; }

  void push(int val) {
    voltageQueue.push(val);
    averageSum += val;

    if (voltageQueue.size() > voltageQueueSize) {
      averageSum -= voltageQueue.front();
      voltageQueue.pop();
    }
  }

  float cal(void) { return averageSum / (float)voltageQueue.size(); }
} sma;

void flash() { sma.push(analogRead(A0)); }

void setup() {
  pinMode(13, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(9600);

  /*
   * 60Hzの交流電圧を全波整流して平滑後の波形には120Hzでリプルが乗る。
   * このリプル電圧の周期は8.333msなのでこの周期の倍数の移動平均を取ればリプルの影響を除ける。
   *
   * 8.333msは3周期で25msとなるので、今回は直近25ms間の平均をとる。
   */
  sma.setVoltageQueueSize(25);
  MsTimer2::set(1, flash);
  MsTimer2::start();
}

void loop() {
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);

  Serial.print(sma.cal());
  Serial.println();
}