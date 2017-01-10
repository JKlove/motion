#include<stm32f10x_lib.>
#include<main.h>
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
GPIO_InitTypeDef GPIO_InitStructure;
ErrorStatus HSEStartUpStatus;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
int pulse;
int StepCount;
int pulse1;
int pulse2;
int t1;
int t2;
int r1;
int r2;
void RCC_Configuration(void);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void TIM2_Configuration(void);
void f(int Vt,int a,int d,int S);
#define VECT_TAB_RAM
int main(void)
{  
  #ifdef DEBUG
    debug();/*[初始化外围设备指针]*/
  #endif
  RCC_Configuration(); //初始化时钟与复位  
  NVIC_Configuration();//初始化中断嵌套
  TIM2_Configuration();//初始化定时器
  GPIO_Configuration();
  
  GPIO_WriteBit(GPIOD, GPIO_Pin_7, (BitAction)(0));
  GPIO_WriteBit(GPIOD, GPIO_Pin_6, (BitAction)(0));  //DCY1 DCY2为00，即Normal %0 DECAY
  
  GPIO_WriteBit(GPIOE, GPIO_Pin_7, (BitAction)(1));
  GPIO_WriteBit(GPIOB, GPIO_Pin_1, (BitAction)(0));  //M1M2为10，即1-2-phase
  //GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)(1));  //正向旋转
  //GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4)));正、反向旋转控制
  
  GPIO_WriteBit(GPIOB, GPIO_Pin_0, (BitAction)(0));
  GPIO_WriteBit(GPIOC, GPIO_Pin_5, (BitAction)(1));  //TQ1 TQ2为01，即Current Ratio为50%
  
  GPIO_WriteBit(GPIOA, GPIO_Pin_7, (BitAction)(1));  //StepReset位
  GPIO_WriteBit(GPIOC, GPIO_Pin_4, (BitAction)(1));  //StepEn 使能位 
  
 while(1)
  {
  r1=0;
  r2=10;
  StepCount=0;
  GPIO_WriteBit(GPIOA,GPIO_Pin_4,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_4)));
  TIM2_Configuration();
  do
  {
  }while(r2);
  
  TIM_Cmd(TIM2, DISABLE);
  Delay(7000000);
  }
}

void GPIO_Configuration(void) 
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;   //PA的3.4.7接CLK,CW/CCW,StepReset
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin= GPIO_Pin_5 | GPIO_Pin_6;   //PA的6.7接Protect和Mo 
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA,&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4 | GPIO_Pin_5;   //PC的4.5接StepEn和TQ2 
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOC,&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1;   //PB的0.1接TQ1和M2
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOB,&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;   //PE7接M1 
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOE,&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6 | GPIO_Pin_7;   //PD的67接DCY2和DCY1
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOD,&GPIO_InitStructure);
} 

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM   
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif                                                  
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;  //设置TIM2通道输入中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; /*配置优先级组*/
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;      /*允许TIM2全局中断*/
  NVIC_Init(&NVIC_InitStructure);
}
void TIM2_Configuration(void)
{ 
  TIM_SetCounter( TIM2, 0x0000);
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);        /*清除更新标志位*/ 
  TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update); //清除TIM2等待中断更新中断标志位
  TIM_ARRPreloadConfig(TIM2, ENABLE);        /*预装载寄存器的内容被立即传送到影子寄存器 */ 
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE); //使能TIM2的更新    
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period = 2000;      //设定的最大计数值2000，最大计数值是0xffff 
  TIM_TimeBaseStructure.TIM_Prescaler = 72;     //分频72
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;     // 时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  TIM_Cmd(TIM2, ENABLE);       //TIM2 enable counter
}
void RCC_Configuration(void)
{       
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);          
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC
                                | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE,ENABLE); 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //打开TIM2的时钟
}   
void Delay(u32 nCount)
{
  do{
    }
  while(nCount--);
}

void f(int Vt,int a,int d,int S)
{ 
  int pulse1;
  int pulse2;
  t1=Vt/a;
  
 if(StepCount<t1)                 //加速阶段，分t1级加速
  {
  r1++;
  pulse1=(150000*t1)/(r1*Vt);
  TIM_SetAutoreload(TIM2,pulse1);
  }
                          
 if(t1<=StepCount<=4*S)          //匀速阶段运行要求的步数或者距离
 {
  pulse=150000/Vt;
  TIM_SetAutoreload(TIM2,pulse);
 }

 if(StepCount>4*S)                            //减速阶段，走完S步后开始减速，分t2-1级减速
  {
    r2--;
   
  if(t2>=1)
  {
  pulse2=(150000*t2)/(r2*Vt);
  TIM_SetAutoreload(TIM2,pulse2);
  }
 
  }
}
void TIM2_IRQHandler(void)
{   
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
  {
    TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    GPIO_WriteBit(GPIOA,GPIO_Pin_3,(BitAction)(1-GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_3)));
    StepCount=StepCount+1;
    f(300,10,10,4000);
    
  }
}
