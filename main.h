#define StepEnPin GPIO_Pin_4
extern int S;
extern int t1;
extern int r2;
extern int pulse1;
extern int pulse2;
extern int StepCount;
extern TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
void Delay(u32 nCount);
void f(int Vt,int a,int d,int S);
