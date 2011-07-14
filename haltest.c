#include <dbus/dbus.h>
#include <libhal.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <fstab.h>

char *
get_prop(LibHalContext *ctx, const char *udi, const char *prop)
{
	DBusError derr;
	char *value;
	char *ret;

	value = NULL;
	dbus_error_init(&derr);

	if (!libhal_device_property_exists(ctx, udi, prop, &derr)) {
		fprintf(stderr, "device %s doesn't have property '%s'\n",
			udi, prop);
		return NULL;
	}

	value = libhal_device_get_property_string(ctx, udi, prop, &derr);
	if (dbus_error_is_set(&derr)) {
		fprintf(stderr, "get_property_string: an error occurred: %s\n",
			derr.message);
		dbus_error_free(&derr);
		if (value)
			libhal_free_string(value);
		return NULL;
	}

	ret = strdup(value);
	libhal_free_string(value);

	return ret;
}

char *
get_prop_for_cap(LibHalContext *ctx, const char *cap, const char *prop)
{
	DBusError derr;
	int num;
	char **devs;
	char *value;

	devs = NULL;

	dbus_error_init(&derr);
	devs = libhal_find_device_by_capability(ctx, cap, &num, &derr);
	if (dbus_error_is_set(&derr)) {
		fprintf(stderr, "by_capability: an error occurred: %s\n", derr.message);
		dbus_error_free(&derr);
		goto error;
	}

	for (int i = 0; i < num; ++i) {
		value = get_prop(ctx, devs[i], prop);
		fprintf(stdout, "%s (%s) %s: %s\n", cap, devs[i], prop, value);
		free(value);
	}

	if (num == 0) {
		fprintf(stderr, "no devs found for '%s'\n", cap);
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


        /*
	  interesting strings from the anylogic 32-bit JNI license lib:

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
        */

	get_prop_for_cap(ctx, "net.80203", "info.product");
	get_prop_for_cap(ctx, "net.80203", "MAC");
	get_prop_for_cap(ctx, "block", "storage.model");
	get_prop_for_cap(ctx, "block.device", "storage.model");
	get_prop_for_cap(ctx, "block.storage_device", "storage.model");
	get_prop_for_cap(ctx, "processor", "info.product");

	get_prop(ctx, "/org/freedesktop/Hal/devices/computer", "smbios.bios.release_date");
	get_prop(ctx, "/org/freedesktop/Hal/devices/computer", "smbios.bios.version");

	return 0;
}
