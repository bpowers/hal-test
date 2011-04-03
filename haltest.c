#include <dbus/dbus.h>
#include <libhal.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


/*
libhal_ctx_init
libhal_ctx_new
libhal_ctx_set_dbus_connection
libhal_device_get_property_string
libhal_device_property_exists
libhal_find_device_by_capability
libhal_free_string
libhal_free_string_array

<[^_]
 [^]
 [^]
 [^]
 [^]
,[^_]
[^_]
[^_]
[^_]


dbus_bus_get
dbus_error_init

error: libhal_ctx_init: %s: %s
error: libhal_ctx_new
error: libhal_ctx_set_dbus_connection: %s: %s


info.product

 MAC 
block
block.device
block.storage_device
net.80203
net.address
/org/freedesktop/Hal/devices/computer
processor
smbios.bios.release_date
smbios.bios.version
storage.model

setfsent
getfsent
endfsent


*/

int
main(int argc, char *argv[])
{
	DBusError derr;
	LibHalContext *ctx;

	dbus_error_init(&derr);

	ctx = libhal_ctx_new();
	if (!ctx) {
		fprintf(stderr, "ctx_new failed");
		return 1;
	}

	

	return 0;
}
