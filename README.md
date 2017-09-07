

# 12bit磁気式エンコーダAS5601 Breakout Board

<img src="/images/EncoderPCB_image.png" width="500px">

12 bit 分解能の磁気式アブソリュートエンコーダ AS5601 (ams AG製) を2.54mmピッチに変換する基板です。ネオジム磁石を同梱。  
5V、3.3V、GNDの電源ピンとI2C、A/B相インクリメンタル出力のピンが出ています。  
**※AS5600との違いはAS5600の出力方式がアナログ/PWM出力であるのに対し、AS5601はA/B相のインクリメンタル出力である点です。**

## AS5601の特徴
- 12 bit(4096)/1回転の高分解能
- 磁気式のため非接触で角度計測が可能であり、信頼性と耐久性に優れている
- インターフェイス：I2C、A/B相インクリメンタル出力、プッシュピン
- I2C：設定レジスタにアクセスでき、角度の読み取りや動作設定が可能
- サンプリング時間：150μs
- AS5601とネオジム磁石の軸中心が1mm程度ずれても値の読み取りが可能

## 基板仕様
- 基板サイズ：横20mm×縦13.5ｍｍ
- 基板マウント用穴：15mmピッチ、M3×2穴
- AS5601の中心位置：横方向は中央、縦方向はピン側の基板端から10.2mm
- 電源電圧は5Vもしくは3.3Vのどちらかを選択でき、5Vを供給する場合は3.3Vピンをオープンに、3.3Vを供給する場合は5Vピンにも3.3Vを供給してください。
- PUSHピンは接続されていないためアクセスできません

## 販売
検討中

## 内容物
- AS5601 Breakout Board本体
- ネオジム磁石 (φ6mm)

## 取付方法
1. 同梱のネオジム磁石を回転体の軸中心に取り付けます
2. AS5601の中心とネオジム磁石の中心が合うよう基板を固定部に取り付けます

取付例  
<img src="/images/Assemble_sample.jpg" width="250px">

## プログラム例
このプログラム例ではI2Cインターフェイスのみを用いて角度の取得を行っています。

### - Arduino (スケッチ)
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
      // Raw angle value (0x0000~0x0FFF) is stored in RawAngle
    }

### - HAL (STM32)
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

## License
MIT License
