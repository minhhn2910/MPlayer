#ifndef __MP3DRIVER_H__
#define __MP3DRIVER_H__

extern void Delay_ms(int );


extern void MP3_SCI_Write(char , unsigned int ) ;
extern void MP3_SCI_Read(char , char , unsigned int *);

extern unsigned int Mp3ReadRegister	(	unsigned char 		 );
extern void MP3_SDI_Write(char ) ;
extern void MP3_SDI_Write_32(char *);

extern void MP3_Set_Volume(char , char ) ;

extern void MP3_reset(void);

extern void MP3_pin_init(void);
extern void  MP3_Init(void);
extern void MP3_soft_reset(unsigned char);

extern void setclock_MP3(void);

extern void send_audio_data(char *);
extern void send_zero(void);
extern void initSSI(void);
extern void test_SSI(void);
extern void MP3_sineTest(void);
extern void MP3_stream_mode(void);
#endif 


