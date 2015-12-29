leds
================================================

// 配置LEDS的相关GPIO端口为输出;    
void leds_init(void);      

// 让第number个LED灯点亮;     
void leds_ON(int number);     

// 让第number个LED灯熄灭;          
void leds_OFF(int number);         

// 点亮所有LED灯         
// void leds_ON_all(void);          

// 熄灭所有LED灯            
// void leds_OFF_all(void);          

// 流水灯                    
void leds_flowing(unsigned short numbers);               

// 二进制加法灯, 按照二进制数字闪烁;           
void leds_binary(unsigned short numbers);             
