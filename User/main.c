#include "stm32f10x.h"
#include "lcd1602.h"

int main(void) {
    // 系统时钟初始化（需根据实际配置）
    SystemInit();
    
    LCD_Init();
    LCD_Clear();
    
//    LCD_SetCursor(1, 1);
//    LCD_Print("2666");
    LCD_PrintData(1,3,0x41);
    //LCD_PrintString(2,1,"26222aC");
    
    while(1) {
        // 动态更新显示
    }
}
