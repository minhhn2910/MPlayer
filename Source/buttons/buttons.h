#ifndef __BUTTONS_H__
#define __BUTTONS_H__
extern unsigned char btnUpHold;
extern unsigned char btnDownHold;
extern unsigned char btnLeftHold;
extern unsigned char btnRightHold;
extern unsigned char btnSelectHold;

extern unsigned char btnUp;
extern unsigned char btnDown;
extern unsigned char btnLeft;
extern unsigned char btnRight;
extern unsigned char btnSelect;

void init_buttons(void);
void buttons_process(void);
#endif 
