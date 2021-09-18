# 12bit (4096P/R) 磁気式エンコーダAS5601 Breakout Board

<img src="/images/EncoderPCB_image_BothSide.png" width="500px">

## 概要

12bit (4096P/R) 分解能の磁気式アブソリュートエンコーダ AS5601 (ams AG製) を2.54mmピッチに変換する基板です。ネオジム磁石を同梱。  
5V、3.3V、GNDの電源ピンとI2C、A/B相インクリメンタル出力のピンが出ています。電源電圧は5Vもしくは3.3Vです。  
**※AS5600との違いはAS5600の出力方式がアナログ/PWM出力であるのに対し、AS5601はA/B相のインクリメンタル出力である点です。**

### AS5601の特徴
- 4096P/R の高分解能 (インクリメンタル出力は最大2048ppr)
- 磁気式のため非接触で角度計測が可能であり、信頼性と耐久性に優れている
- インターフェイス：I2C、インクリメンタル出力、PUSHピン
- I2C：設定レジスタにアクセスでき、角度の読み取りや動作設定が可能
- インクリメンタル出力：8ppr～2048pprの分解能設定が可能なA/B相の直交出力
- サンプリング時間：150μs
- AS5601とネオジム磁石の軸中心が1mm程度ずれても値の読み取りが可能

### 基板仕様
- 基板サイズ：横20mm×縦13.5ｍｍ
- 基板マウント用穴：15mmピッチ、M3×2穴
- 電源電圧に5Vを供給する場合は3.3Vピンをオープンに、3.3Vを供給する場合は5Vピンにも3.3Vを供給してください。
- PUSHピンは接続されていないためアクセスできません


## 販売  
[スイッチサイエンス委託販売ページ](https://www.switch-science.com/catalog/3494/)  
※大量注文や在庫に関する問い合わせは[こちら](mailto:info.y2kb@gmail.com)までご連絡ください。  

## 内容物
- AS5601 Breakout Board 本体
- エンコーダ用ネオジム磁石

<img src="/images/Product_photo.jpg" width="200px">

### 回路図  
<img src="/images/CircuitDiagram.png" width="450px">

### 寸法  
<img src="/images/EncoderPCB_image.jpg" width="200px">

## 取付方法
1. 同梱のネオジム磁石を回転体の軸中心に取り付けます
2. AS5601の中心とネオジム磁石の中心が合うよう基板を固定部に取り付けます

**取付例**  
<img src="/images/Assemble_sample.jpg" width="250px">

## Arduinoによるプログラム例  

### I2Cによる角度の取得  
#### 配線図  
<img src="/images/WiringDiagram_I2C.png" width="300px">

#### [ソースファイル](https://github.com/y2kblog/AS5601_BreakoutBoard_V1_0/blob/master/SampleCode/Arduino/I2C_SampleCode/I2C_SampleCode.ino)

<!--
#### ソースファイルのディレクトリ  
"SampleCode/Arduino/I2C_SampleCode/I2C_SampleCode.ino"
-->

#### ミニマムコード  

```cpp
#include <stdint.h>
#include <Wire.h>
#define AS5600_AS5601_DEV_ADDRESS      0x36
#define AS5600_AS5601_REG_RAW_ANGLE    0x0C

void setup() {
  // I2C init
  Wire.begin();
  Wire.setClock(400000);

  // Read RAW_ANGLE value from encoder
  Wire.beginTransmission(AS5600_AS5601_DEV_ADDRESS);
  Wire.write(AS5600_AS5601_REG_RAW_ANGLE);
  Wire.endTransmission(false);
  Wire.requestFrom(AS5600_AS5601_DEV_ADDRESS, 2);
  uint16_t RawAngle = 0;
  RawAngle  = ((uint16_t)Wire.read() << 8) & 0x0F00;
  RawAngle |= (uint16_t)Wire.read();
  // Raw angle value (0 ~ 4095) is stored in RawAngle
}

void loop() {
}
```

### インクリメンタル出力ピンを用いた角度の取得  
#### 配線図  
<img src="/images/WiringDiagram_Incremental.png" width="300px">

#### [ソースファイル](https://github.com/y2kblog/AS5601_BreakoutBoard_V1_0/blob/master/SampleCode/Arduino/Incremental_SampleCode/Incremental_SampleCode.ino)

<!--
#### ソースファイルのディレクトリ  
"SampleCode/Arduino/Incremental_SampleCode/Incremental_SampleCode.ino"
-->

#### ミニマムコード
```cpp
#include <stdint.h>
#include <Wire.h>
#define AS5600_AS5601_DEV_ADDRESS       0x36
#define AS5601_REG_ABN                  0x09

volatile int32_t EncoderCount;

void Encoder_GPIO_init(void) {
  DDRD  &= ~((1 << PD2) | (1 << PD3));  // Set PD2 and PD3 as input
  EICRA = 0b00000101; // Trigger event of INT0 and INT1 : Any Logic Change
  EIMSK = 0b00000011; // Enable interrupt INT0 and INT1
  sei();              //Enable Global Interrupt
}

// Encoder "A" pin logic change interrupt callback function
ISR(INT0_vect) {
  updateEncoderCount();
}

// Encoder "B" pin logic change interrupt callback function
ISR(INT1_vect) {
  updateEncoderCount();
}

void updateEncoderCount(void) {
  const static int8_t EncoderIndexTable[] =
    {0, -1, 1, 0,  1, 0, 0, -1,  -1, 0, 0, 1,  0, 1, -1, 0};
  static uint8_t EncoderPinState_Now, EncoderPinState_Prev = 0;

  EncoderPinState_Now = (PIND >> 2) & 0x03; // Bit1 : PD3 (Encoder B), Bit0 : PD2 (Encoder A)
  EncoderCount += EncoderIndexTable[EncoderPinState_Prev << 2 | EncoderPinState_Now];
  EncoderPinState_Prev = EncoderPinState_Now;
}

void Encoder_I2C_init(void) {
  // Set AS5601 resolution 2048ppr
  Wire.beginTransmission(AS5600_AS5601_DEV_ADDRESS);
  Wire.write(AS5601_REG_ABN);
  Wire.write(0b00001000);   // ABN(3:0)
  Wire.endTransmission();
  delay(1);
}

void setup() {
  // I2C init
  Wire.begin();
  Wire.setClock(400000);

  // Peripheral init
  Encoder_I2C_init();
  Encoder_GPIO_init();
}

void loop() {
  // Angle value (0 ~ 2047) is stored in EncoderCount
}
```

<!--
#### HAL (STM32)

**サンプルコード**

    // I2Cの初期化は終わっているとする

    #define AS5600_AS5601_DEV_ADDRESS      (0x36<<1)
    #define AS5600_AS5601_REG_RAW_ANGLE    0x0C

    uint8_t buf[2];
    HAL_I2C_Mem_Read(&hi2c, AS5600_AS5601_DEV_ADDRESS,
      AS5600_AS5601_REG_RAW_ANGLE, I2C_MEMADD_SIZE_8BIT,
      (uint8_t*)buf, 2, 1000);
    uint16_t RawAngle;
    RawAngle = (uint16_t) buf[0] << 8 | (uint16_t) buf[1];
    RawAngle &= 0x0FFF;
    // Raw angle value (0x0000~0x0FFF) is stored in RawAngle


### コンフィグのメモリへの焼き付け  

-->

### 設定レジスタの焼き付けによる分解能の固定

AS5601のデフォルトの分解能は8pprなので、初期状態ですと電源投入毎に分解能を設定する必要があります。
そこで、分解能を2048pprに設定後にBurn_Settingコマンドにより設定レジスタを焼くことで、電源を落として再度電源を投入しても起動時から2048pprに設定された状態で固定され、I2Cにより分解能等の設定情報を再設定する必要がなくなります。  
**※一度書き込むと以後分解能やフィルタ設定などを変更することはできなくなります。**

サンプルコードは[こちら](https://github.com/y2kblog/AS5601_BreakoutBoard_V1_0/blob/master/SampleCode/Arduino/burnConfig_SampleCode/burnConfig_SampleCode.ino)。

## 資料

### 3Dデータ
- STEPファイルダウンロード：<a href="https://github.com/y2kblog/AS5601_BreakoutBoard_V1_0/raw/master/PCB_source/step/AS5601_BreakoutBoard_V1_0_step.zip" download="">AS5601_BreakoutBoard_V1_0_step.zip</a>  


## License
MIT License
