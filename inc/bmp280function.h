/******************************************************************************/
/** ģ�����ƣ���չ��������                                                   **/
/** �ļ����ƣ�bmp280function.c                                               **/
/** ��    ����V1.1.0                                                         **/
/** ��    �飺����ʵ��BMP280ѹ�����¶ȴ������Ĳ���                           **/
/**           BMP280��37���ɲ����Ĵ���������26��У׼���ݣ���ַ���£�         **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |�Ĵ�����  |��ַ|bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|��λ״̬|��д| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_xlsb |0xFC|    temp_xlsb<7:4> | 0  | 0  | 0  | 0  |  0x00  |ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_lsb  |0xFB|              temp_lsb<7:0>            |  0x00  |ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |temp_msb  |0xFA|              temp_msb<7:0>            |  0x80  |ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_xlsb|0xF9|   press_xlsb<7:4> | 0  | 0  | 0  | 0  |  0x00  |ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_lsb |0xF8|             press_lsb<7:0>            |  0x00  |ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |press_msb |0xF7|             press_msb<7:0>            |  0x80  |ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |config    |0xF5|   t_sb[2:0]  |  filter[2:0] |    |e[0]|  0x00  |��д| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |ctrl_meas |0xF4|  osrs_t[2:0] |  osrs_p[2:0] |mode[1:0]|  0x00  |��д| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  | status   |0xF3|                   |m[0]|         |u[0]|  0x00  |ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  | reset    |0xE0|             reset[7:0]                |  0x00  |ֻд| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |   id     |0xD0|             chip_id[7:]               |  0x58  |ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib25   |0xA1|        calibration data[��26]         |�����趨|ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  |calib00   |0x88|        calibration data[��26]         |�����趨|ֻ��| **/
/**  +----------+----+----+----+----+----+----+----+----+----+--------+----+ **/
/**  ���Ʋ����Ĵ���ѹ���������¶Ȳ��������ã�                                **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |osrs_p[2:0]|����|    ����    |    |osrs_t[2:0]|����|     ����    |   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   000     |  - |      -     |    |   000     |  - |     -       |   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   001     | x1 |16bit/2.62Pa|    |   001     | x1 |16bit/0.0050C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   010     | x2 |17bit/1.31Pa|    |   010     | x2 |17bit/0.0025C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   011     | x4 |18bit/0.66Pa|    |   011     | x4 |18bit/0.0012C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |   100     | x8 |19bit/0.33Pa|    |   100     | x8 |19bit/0.0006C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**    |101,110,111| x16|20bit/0.16Pa|    |101,110,111| x16|20bit/0.0003C|   **/
/**    +-----------+----+------------+    +-----------+----+-------------+   **/
/**--------------------------------------------------------------------------**/
/**ʹ��˵����                                                                **/
/**    1������BMP280ObjectType����ʵ�壬�����ú���BMP280Initialization��ʼ�� **/
/**       ����ʵ�塣��ʼ��ʱ����Ҫ�������²�����                             **/
/**       bmpAddress,����I2C�ӿڱ�����豸��ַ��0xEC��0xEE��SPI�ӿ�ʱ����Ҫ  **/
/**       port,ͨѶ�ӿ����ͣ�I2C����SPI                                      **/
/**       t_sb,������ڣ����üĴ������ò���                                  **/
/**       filter,�������˲����ã����üĴ������ò���                          **/
/**       spi3W_en,�Ƿ�����3��SPI���ƣ����üĴ������ò���                    **/
/**       osrs_t,�¶Ȳ����������ã��������ƼĴ������ò���                    **/
/**       osrs_p,ѹ���������ȣ��������ƼĴ������ò���                        **/
/**       mode,��Դģʽ���������ƼĴ������ò���                              **/
/**       Read,�����ݲ���ָ�룬�亯�����ͼ���������                          **/
/**       Write,д���ݲ���ָ�룬�亯�����ͼ���������                         **/
/**       Delayms,��ʱ����ָ�룬�亯�����ͼ���������                         **/
/**       ChipSelect,Ƭѡ����ָ�룬SPI�ӿڱ��衣I2C�ӿڲ���Ҫ������NULL      **/
/**    2��ʵ������void f(struct BMP280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t *rData,uint16_t rSize)�ĺ����������丳ֵ��BMP280ObjectType **/
/**       ����ʵ���Read����ָ�롣                                           **/
/**    3��ʵ������void f(struct BMP280Object *bmp,uint8_t regAddress,        **/
/**       uint8_t command)�ĺ����������丳ֵ��BMP280ObjectType����ʵ��       **/
/**       ��Write����ָ�롣                                                  **/
/**    4��ʵ������void f(volatile uint32_t nTime)�ĺ����������丳ֵ��        **/
/**       BMP280ObjectType����ʵ���Delayms����ָ�롣ʵ�ֺ�����ʱ��          **/
/**    5��ʵ������void f(bool en)�ĺ����������丳ֵBMP280ObjectType����ʵ��  **/
/**       ��ChipSelcet����ָ�롣                                             **/
/**    6��������Ӧ�ĺ���ʵ����Ҫ�Ĳ�����                                     **/
/**--------------------------------------------------------------------------**/
/** �޸ļ�¼��                                                               **/
/**     �汾      ����              ����              ˵��                   **/
/**     V1.0.0  2017-07-17          ľ��              �����ļ�               **/
/**     V1.0.1  2018-05-20          ľ��              ���豸����Ϊ�ṹ����   **/
/**     V1.1.0  2019-05-30          ľ��              �޸Ĳ��������Ͷ���     **/
/**                                                                          **/
/******************************************************************************/ 

#ifndef __BMP280FUNCTION_H
#define __BMP280FUNCTION_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

/* ����ѹ�������趨ö������ */
typedef enum BMP280PressureSample {
  BMP280_PRES_SAMPLE_X0=0x00,   //ѹ��������
  BMP280_PRESE_SAMPLE_X1=0x04,  //ѹ������16λ
  BMP280_PRES_SAMPLE_X2=0x08,   //ѹ������17λ
  BMP280_PRES_SAMPLE_X4=0x0C,   //ѹ������18λ
  BMP280_PRES_SAMPLE_X8=0x10,   //ѹ������19λ
  BMP280_PRES_SAMPLE_X16=0x14   //ѹ������20λ
}BMP280PressureSampleType;

/* �����¶Ȳ����趨ö������ */
typedef enum BMP280TemperatureSample {
  BMP280_TEMP_SAMPLE_X0=0x00,   //�¶Ȳ�����
  BMP280_TEMP_SAMPLE_X1=0x20,   //�¶Ȳ���16λ
  BMP280_TEMP_SAMPLE_X2=0x40,   //�¶Ȳ���17λ
  BMP280_TEMP_SAMPLE_X4=0x60,   //�¶Ȳ���18λ
  BMP280_TEMP_SAMPLE_X8=0x80,   //�¶Ȳ���19λ
  BMP280_TEMP_SAMPLE_X16=0xA0   //�¶Ȳ���20λ
}BMP280TemperatureSampleType;

/* �����Դģʽ�趨ö������ */
typedef enum BMP280PowerMode {
  BMP280_POWER_SLEEP_MODE=0x00,         //˯��ģʽ
  BMP280_POWER_FORCED_MODE=0x01,        //ǿ��ģʽ
  BMP280_POWER_NORMAL_MODE=0x03         //����ģʽ
}BMP280PowerModeType;

/* ����ȴ�ʱ���趨ö������ */
typedef enum BMP280TimeStandby {
  BMP280_T_SB_0P5=0x00,		//�ȴ�ʱ��0.5ms
  BMP280_T_SB_62P5=0x20,	//�ȴ�ʱ��62.5ms
  BMP280_T_SB_125=0x40,		//�ȴ�ʱ��125ms
  BMP280_T_SB_250=0x60,		//�ȴ�ʱ��250ms
  BMP280_T_SB_500=0x80,		//�ȴ�ʱ��500ms
  BMP280_T_SB_1000=0xA0,	//�ȴ�ʱ��1000ms
  BMP280_T_SB_2000=0xC0,	//�ȴ�ʱ��2000ms
  BMP280_T_SB_4000=0xE0		//�ȴ�ʱ��4000ms
}BMP280TimeStandbyType;

/* ����IIR�˲��趨ö������ */
typedef enum BMP280IIRFilterCoeff {
  BMP280_IIR_FILTER_COEFF_OFF=0x00,   //�ر��˲���
  BMP280_IIR_FILTER_COEFF_X2=0x04,    //�˲���ϵ��2
  BMP280_IIR_FILTER_COEFF_X4=0x08,    //�˲���ϵ��4
  BMP280_IIR_FILTER_COEFF_X8=0x0C,    //�˲���ϵ��8
  BMP280_IIR_FILTER_COEFF_X16=0x10    //�˲���ϵ��16
}BMP280IIRFilterCoeffType;

/* ����3��SPIʹ��ö������ */
typedef enum BMP280UseSPI3w {
  BMP280_SPI3W_DISABLE=0x00,    //������3��SPI
  BMP280_SPI3W_ENABLE=0x01      //����3��SPI
}BMP280UseSPI3wType;

/* ����BMP280ͨѶ�ӿ�ö�� */
typedef enum BMP280Port{
  BMP280_I2C,
  BMP280_SPI
}BMP280PortType;

/* ������������������������ */
typedef struct Bmp280CalibParam {
  uint16_t dig_t1;      //�¶�����ϵ��1
  int16_t  dig_t2;      //�¶�����ϵ��2
  int16_t  dig_t3;      //�¶�����ϵ��3
  uint16_t dig_p1;      //ѹ������ϵ��1
  int16_t  dig_p2;      //ѹ������ϵ��2
  int16_t  dig_p3;      //ѹ������ϵ��3
  int16_t  dig_p4;      //ѹ������ϵ��4
  int16_t  dig_p5;      //ѹ������ϵ��5
  int16_t  dig_p6;      //ѹ������ϵ��6
  int16_t  dig_p7;      //ѹ������ϵ��7
  int16_t  dig_p8;      //ѹ������ϵ��8
  int16_t  dig_p9;      //ѹ������ϵ��9
  int32_t  t_fine;      //����ѹ���������¶�����
}Bmp280CalibParamType;

/* ����Ƭѡ�ź�ö�� */
typedef enum BMP280CS {
  BMP280CS_Enable,
  BMP280CS_Disable
}BMP280CSType;

/*����BMP280��������*/
typedef struct BMP280Object{
  uint8_t bmpAddress;   //I2CͨѶʱ���豸��ַ
  uint8_t chipID;       //оƬID
  uint8_t config;       //���üĴ���
  uint8_t ctrlMeas;     //�������ƼĴ���
  BMP280PortType port;  //�ӿ�ѡ��
  Bmp280CalibParamType caliPara;        //У׼����
  float pressure;       //ѹ��ֵ
  float temperature;    //�¶�ֵ
  void (*Read)(struct BMP280Object *bmp,uint8_t regAddress,uint8_t *rData,uint16_t rSize);      //�����ݲ���ָ��
  void (*Write)(struct BMP280Object *bmp,uint8_t regAddress,uint8_t command);   //д���ݲ���ָ��
  void (*Delayms)(volatile uint32_t nTime);     //��ʱ����ָ��
  void (*ChipSelect)(BMP280CSType cs);  //ʹ��SPI�ӿ�ʱ��Ƭѡ����
}BMP280ObjectType;

/* ��������ݲ�������ָ������ */
typedef void (*BMP280Read)(BMP280ObjectType *bmp,uint8_t regAddress,uint8_t *rData,uint16_t rSize);
/* ����д���ݲ�������ָ������ */
typedef void (*BMP280Write)(BMP280ObjectType *bmp,uint8_t regAddress,uint8_t command);
/* ������ʱ��������ָ������ */
typedef void (*BMP280Delayms)(volatile uint32_t nTime);
/* ����ʹ��SPI�ӿ�ʱ��Ƭѡ��������ָ������ */
typedef void (*BMP280ChipSelect)(BMP280CSType cs);

/*��ȡѹ�����¶�����*/
void GetBMP280Measure(BMP280ObjectType *bmp);

/*ʵ��BMP280��ʼ������*/
void BMP280Initialization(BMP280ObjectType *bmp,                //BMP280����
                          uint8_t bmpAddress,                   //I2C�ӿ����豸��ַ
                          BMP280PortType port,                  //�ӿ�ѡ��
                          BMP280TimeStandbyType t_sb,           //�������
                          BMP280IIRFilterCoeffType filter,      //������
                          BMP280UseSPI3wType spi3W_en,          //3��SPI����
                          BMP280TemperatureSampleType osrs_t,   //�¶Ⱦ���
                          BMP280PressureSampleType osrs_p,      //ѹ������
                          BMP280PowerModeType mode,             //��Դģʽ
                          BMP280Read Read,                      //�����ݲ���ָ��
                          BMP280Write Write,                    //д���ݲ���ָ��
                          BMP280Delayms Delayms,                //��ʱ����ָ��
                          BMP280ChipSelect ChipSelect           //Ƭѡ����ָ��
                         );

#endif
/*********** (C) COPYRIGHT 1999-2019 Moonan Technology *********END OF FILE****/