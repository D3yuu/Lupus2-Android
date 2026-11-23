#ifndef _EL_SERVICEDEFS_H_
#define _EL_SERVICEDEFS_H_

// Lupus2 production server and Windows client use the legacy TEA transport.
// Enabling the improved cipher here makes auth decrypt LOGIN3 with the wrong
// protocol and immediately close the connection.
//#define _IMPROVED_PACKET_ENCRYPTION_
//#define __PERFORMANCE_CHECK__

#endif //_EL_SERVICEDEFS_H_
