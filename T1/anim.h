// режим
#define OAM_NOTHING 0
// 0-ничего не менять
#define OAM_MANUAL  1
// 1-просто переход к след. части
#define OAM_AUTO    2
// 2-анимация TAnim
#define OAM_NONSTOP 3
// 2-бесконечная анимация TAnim

void StartAnimation(void);
void AnimateAll(void);
void GlbAnimation(void);
void ProvokeAnimation(int x,int y,int l,int dx, int dy, short Type, char Mode);
int SaveAnimations(void);
int LoadAnimations(int ver);

