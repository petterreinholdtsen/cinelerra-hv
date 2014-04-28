/*
 *  version.h
 */

#define SND_LIB_MAJOR		0 /**< major number of library version */
#define SND_LIB_MINOR		9 /**< minor number of library version */
#define SND_LIB_SUBMINOR	0 /**< subminor number of library version */
#define SND_LIB_EXTRAVER	100000 /**< extra version number, used mainly for betas */
/** library version */
#define SND_LIB_VERSION		((SND_LIB_MAJOR<<16)|\
				 (SND_LIB_MINOR<<8)|\
				  SND_LIB_SUBMINOR)
/** library version (string) */
#define SND_LIB_VERSION_STR	"0.9.0rc2"

