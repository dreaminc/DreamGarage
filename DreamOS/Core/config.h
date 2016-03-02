#ifndef CONFIG_H_
#define CONFIG_H_

// DREAM OS
// DreamOS/Core/config.h
// Core configuration header - this file should be force included in the build (/FI) 
// These configurations should apply to all files

// Type precision
// Float vs Double
#define FLOAT_PRECISION
//#define DOUBLE_PRECISION

// TODO: This is currently a configuration setting
// If in the future DreamOS needs to support different APIs 
// this should no longer be a user defined thing
#define RIGHT_HANDED
//#define LEFT_HANDED

#endif // ! CONFIG_H_
