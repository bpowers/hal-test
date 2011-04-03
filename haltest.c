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


char *
get_prop(LibHalContext *ctx, const char *cap, const char *prop)
{
	DBusError derr;
	int num;
	char **devs;

	devs = NULL;

	dbus_error_init(&derr);
	devs = libhal_find_device_by_capability(ctx, cap, &num, &derr);
	if (dbus_error_is_set(&derr)) {
		fprintf(stderr, "by_capability: an error occurred: %s\n", derr.message);
		dbus_error_free(&derr);
		goto error;
	}

	for (int i = 0; i < num; ++i) {
		char *udi;
		char *value;

		udi = devs[i];
		value = NULL;

		if (!libhal_device_property_exists(ctx, udi, prop, &derr)) {
			fprintf(stderr, "device %s doesn't have property '%s'\n",
				udi, prop);
			continue;
		}

		value = libhal_device_get_property_string(ctx, udi, prop, &derr);
		if (dbus_error_is_set(&derr)) {
			fprintf(stderr, "get_property_string: an error occurred: %s\n",
				derr.message);
			dbus_error_free(&derr);
			if (value)
				libhal_free_string(value);
			goto error;
		}

		fprintf(stdout, "%s (%s) %s: %s\n", cap, udi, prop, value);

		libhal_free_string(value);
	}

error:
	if (devs)
		libhal_free_string_array(devs);
	return NULL;
}


int
main(int argc, char *argv[])
{
	DBusError derr;
	DBusConnection *conn;
	LibHalContext *ctx;

	dbus_error_init(&derr);

	ctx = libhal_ctx_new();
	if (!ctx) {
		fprintf(stderr, "ctx_new failed\n");
		return 1;
	}

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &derr);
	if (dbus_error_is_set(&derr)) {
		fprintf(stderr, "bus_get: an error occurred: %s\n", derr.message);
		dbus_error_free(&derr);
		return 1;
	}

	if (!libhal_ctx_set_dbus_connection(ctx, conn)) {
		fprintf(stderr, "couldn't set dbus conn\n");
		return 1;
	}

	libhal_ctx_init(ctx, &derr);
	if (dbus_error_is_set(&derr)) {
		fprintf(stderr, "ctx_init: an error occurred: %s\n", derr.message);
		dbus_error_free(&derr);
		return 1;
	}

	get_prop(ctx, "net.80203", "info.product");
	get_prop(ctx, "block", "info.product");
	get_prop(ctx, "processor", "info.product");

	//libhal_find_device_by_capability(); // net.80203
	//libhal_find_device_by_capability(); // block
	//libhal_find_device_by_capability(); // processor

	//if (libhal_device_property_exists()) { // info.product
	//	libhal_device_get_property();

	//	libhal_free_string();
	//}

	return 0;
}
