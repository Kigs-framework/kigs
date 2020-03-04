#ifndef _KIGS_INTERFACE_
#define _KIGS_INTERFACE_

#ifdef __cplusplus
extern "C" {
#endif

	int	AppInit();
	int	AppUpdate();
	int	AppClose();
	int AppSleep();
	int AppResume();

#ifdef __cplusplus
}
#endif

#endif
