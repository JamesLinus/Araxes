// BlacklightEVO tools/evofstools/evofs.c -- EVOfs tool
// Copyright (c) 2013-2015 The Cordilon Group -- http://www.blacklightevo.org
// The Blacklight project is under the terms of the ISC license. See license.md for details.
// Questions? Comments? Concerns? Email us: blacklight@cordilon.net

#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

int verbose = 0;

const char* image_filename = 0;
FILE* disk_image;

off_t disk_image_bytes;
unsigned int disk_image_sectors;

unsigned char sector_buffer[512];
int current_partition = -1;
unsigned int current_partition_size = 0;
unsigned int current_partition_offset = 0;

void toolbox_spew() {
	printf("The EVOfs toolbox is a set of tools designed to create, modify, and operate on\n");
	printf("EVOfs volumes. The toolbox should be able to operate on disk images without\n");
	printf("superuser permissions, but you most likely will need to run it as root if you\n");
	printf("intend to work directly on a physical volume.\n\n");
}

void print_usage(int full) {
	fprintf(stderr, "evofs - EVOfs toolbox\n");
	fprintf(stderr, "Usage: evofs [options] disk_image\n\n");
	
	if (!full) {
		fprintf(stderr, "Use the --help option for full usage.\n");
		return;
	}
	
	toolbox_spew();
	
	printf("Options:\n");
	printf("    -?, --help                Displays this screen\n");
	//printf("    -f, --floppy              Assumes disk_image is a floppy\n");
	//printf("    -p, --partition           Assumes disk_image is a single partition\n");
	printf("    -v, --verbose             Increased verbiage from operations\n");
}

char* minimum_size(unsigned int sectors) {
	static char sizestr[16] = {};
	double d;
	int divisor = 0;
	const char* sizediv;
	
	d = (double)sectors;
	d *= 512;
	while (d >= 1024) {
		d /= 1024;
		divisor++;
	}
	
	switch (divisor) {
		case 0:
			sizediv = "B";
			break;
		case 1:
			sizediv = "KiB";
			break;
		case 2:
			sizediv = "MiB";
			break;
		case 3:
			sizediv = "GiB";
			break;
		case 4:
			sizediv = "TiB";
			break;
	}
	
	sprintf(sizestr, "%.2f %s", d, sizediv);
	return sizestr;
}

const char* id_to_type(unsigned char id) {
	switch (id) {
		case 0x00:
			return "empty";
		case 0x01:
			return "FAT12";
		case 0x04:
		case 0x06:
			return "FAT16";
		case 0x05:
			return "DOS extended";
		case 0x07:
			return "NTFS";
		case 0x0B:
		case 0x0C:
			return "FAT32";
		case 0x80:
			return "Minix old";
		case 0x81:
			return "Minix";
		case 0x82:
			return "Linux swap";
		case 0x83:
			return "Linux native";
		case 0xE0:
			return "EVOfs";
		default:
			return "unknown";
	}
}

unsigned char* format_dent(const char* name, unsigned int fileblock) {
	static unsigned char dent[128];
	
	memset(dent, 0, 128);
	strncpy(dent, name, 123);	// strncpy doesn't add a terminating \0 if it doesn't have to pad
	*(unsigned int*)(dent+124) = fileblock;
	return dent;
}

void list_commands() {
	printf("List of commands (type \"help <command>\" for more help):\n");
	printf("help        Displays this help.\n");
	printf("quit        Exits the toolbox.\n");
	printf("list        Displays the partitions on the disk.\n");
	printf("select      Selects a partition to operate on.\n");
	printf("create*     Creates an EVOfs filesystem on the selected partition.\n");
	printf("mbr*        Injects MBR boot code into the disk.\n");
	printf("vbr*        Injects VBR boot code into the selected partition.\n");
	printf("\n");
}

void command_display_help(int argc, char* argv[]) {
	if (argc < 2) {
		printf(" _____   _____   __    \n"); 
		printf("| __\\ \\ / / _ \\ / _|___\n");
		printf("| _| \\ V / (_) |  _(_-<\n");
		printf("|___| \\_/ \\___/|_| /__/\n");
		printf("EVOfs toolbox version 0\n\n");
		
		toolbox_spew();
		
		list_commands();
		
		printf("Commands with an asterisk after them are potentially destructive, and require\n");
		printf("at minimum the parameter \"confirm\" to be passed to them to be used (a notice\n");
		printf("about the confirm parameter will be printed if you run the command without it.\n\n");
		return;
	}
	
	if (!strcmp(argv[1], "terse")) {
		list_commands();
		return;
	}
	
	if (!strcmp(argv[1], "help") || !strcmp(argv[1], "?")) {
		printf("help - prints help for the toolbox and its commands.\n\n");
		
		printf("Passing a command name to help (like you've just done) will display help for\n");
		printf("that command. Alternately, passing \"terse\" to the help command will display\n");
		printf("just a list of commands.\n\n");
		return;
	}
	
	if (!strcmp(argv[1], "quit") || !strcmp(argv[1], "exit") || !strcmp(argv[1], "ex")) {
		printf("quit - exits the toolbox.\n\n");
		
		printf("The quit command has no options.\n\n");
		return;
	}
	
	if (!strcmp(argv[1], "list") || !strcmp(argv[1], "l")) {
		printf("list - displays a list of partitions on the disk.\n\n");
		
		printf("The list command has no options.\n\n");
		return;
	}
	
	if (!strcmp(argv[1], "select")) {
		printf("select - Selects a partition to operate on.\n\n");
		
		printf("The select command, without any parameters, deselects the current partition.\n");
		printf("Passing the number of a valid (non-empty) partition as reported by the list\n");
		printf("command will select that partition for operation. The select command will not\n");
		printf("warn the user about any potential data on the partition being selected.\n\n");
		return;
	}
	
	if (!strcmp(argv[1], "create")) {
		printf("create - Creates an EVOfs filesystem on the selected partition.\n\n");
		
		printf("This command is potentially the most destructive of the commands in the toolbox\n");
		printf("and, as a result, requires either \"confirm\" or \"test\" parameters (but not both)\n");
		printf("to be passed to it. \"confirm\" will go ahead with the creation and write to the\n");
		printf("partition. \"test\" will execute a \"dry run\" of the creation, doing all the math\n");
		printf("and memory allocations/frees but without actually modifying the data on the\n");
		printf("partition. You can also pass \"verbose\" after either command to see the really\n");
		printf("technical behind-the-scenes stuff going on.\n\n");
		return;
	}
	
	printf("No help available for %s (experiment at your own risk).\n", argv[1]);
}

void command_list_partitions(int argc, char* argv[]) {
	fseek(disk_image, 0, 0);
	fread(sector_buffer, 512, 1, disk_image);
	
	printf("Partition list:\n");
	
	for (int i = 0; i < 4; i++) {
		if (sector_buffer[(0x1BE)+(i*0x10)+0x04]) {
			unsigned int ss = *(unsigned int*)(sector_buffer+(0x1BE)+(i*0x10)+0x08);
			unsigned int sc = *(unsigned int*)(sector_buffer+(0x1BE)+(i*0x10)+0x0C);
			printf("%d: LBA 0x%08X - 0x%08X (%s) - Type %02X (%s)%s\n", i, ss, ss+sc-1, minimum_size(sc), sector_buffer[(0x1BE)+(i*0x10)+0x04], id_to_type(sector_buffer[(0x1BE)+(i*0x10)+0x04]), ((sector_buffer[(0x1BE)+(i*0x10)] & 0x80) ? ", active" : ""));
		} else {
			printf("%d: No partition\n", i);
		}
	}
}

void command_select_partition(int argc, char* argv[]) {
	int request = -1;
	fseek(disk_image, 0, 0);
	fread(sector_buffer, 512, 1, disk_image);
	
	// stupid hack to make command line partition editing work smoothly
	if (!strcmp(argv[0], "dummy")) {
		request = current_partition;
		goto calcoff;
	}
	
	if (argc < 2) {
		current_partition = -1;
		return;
	}
	
	if (!sscanf(argv[1], "%d", &request)) {
		printf("That's not a partition number.\n");
		return;
	}
	
	if (request > 3 || request < 0) {
		printf("That's not a partition number.\n");
		return;
	}
	
calcoff:
	if (sector_buffer[(0x1BE)+(request*0x10)+0x04]) {
		unsigned int ss = *(unsigned int*)(sector_buffer+(0x1BE)+(request*0x10)+0x08);
		unsigned int sc = *(unsigned int*)(sector_buffer+(0x1BE)+(request*0x10)+0x0C);
		current_partition_offset = ss;
		current_partition_size = sc;
		current_partition = request;
	} else {
		printf("Partition %d is empty.\n", request);
	}
}

void command_create_filesystem(int argc, char* argv[]) {
	int be_verbose = verbose;
	int dryrun = 0;
	
	fseek(disk_image, 0, SEEK_SET);
	fread(sector_buffer, 512, 1, disk_image);
	
	if (current_partition == -1) {
		printf("No partition is selected.\n");
		return;
	}
	
	if (argc < 2) {
		printf("Type \"create confirm\" to create an EVOfs filesystem on partition %d.\n", current_partition);
		printf("Type \"create test\" to do a dry run of the creation.\n");
		printf("Type \"create <action> verbose\" to be verbose regardless of global setting.\n");
		printf("WARNING: This will erase any filesystem currently on partition %d.\n", current_partition);
		return;
	}
	
	if (!strcmp(argv[1], "test")) {
		printf("Keyword \"test\" detected, doing a dry run (no actual writes will occur).\n");
		dryrun = 1;
	} else if (strcmp(argv[1], "confirm")) {
		printf("You didn't spell \"confirm\" right.\n");
		return;
	}
	
	if (argc > 2) {
		if (!strcmp(argv[2], "verbose")) {
			printf("Keyword \"verbose\" detected, dumping additional output.\n");
			be_verbose = 1;
		}
	}
	
	if (dryrun || be_verbose)
		printf("\n");
	
	printf("Here we go, creating an EVOfs filesystem on partition %d.\n", current_partition);
	
	sector_buffer[(0x1BE)+(current_partition*0x10)+0x04] = 0xE0;
	fseek(disk_image, 0, SEEK_SET);
	if (!dryrun)
		fwrite(sector_buffer, 512, 1, disk_image);
	
	if (be_verbose)
		printf("Updated partition table with new type ID.\n");
	
	fseek(disk_image, (current_partition_offset*512), SEEK_SET);
	fread(sector_buffer, 512, 1, disk_image);
	
	unsigned int offset_vbr = 0x00000000;
	unsigned int offset_stage2 = 0x00000001;
	unsigned int offset_reserved = 0x0000003D;
	unsigned int offset_blocktable = 0x00000040;
	unsigned int offset_data = 0;
	
	unsigned int size_blocktable = 0;
	unsigned int sectors_blocktable = 0;
	unsigned int size_data = 0;
	
	size_blocktable = current_partition_size;
	sectors_blocktable = size_blocktable/512 + ((size_blocktable % 512) ? 1 : 0);
	offset_data = offset_blocktable + sectors_blocktable;
	
	if (be_verbose)
		printf("-- current_partition_size is %u sectors.\n", current_partition_size);
	
	
	*(unsigned int*)(sector_buffer+(0x140)) = 0x214F5645;				// vol_magic
	*(unsigned int*)(sector_buffer+(0x144)) = current_partition_size;	// vol_size
	*(unsigned int*)(sector_buffer+(0x148)) = 0x00000000;				// vol_version
	*(unsigned int*)(sector_buffer+(0x14C)) = 0x00000001;				// vol_creator
	*(unsigned int*)(sector_buffer+(0x150)) = offset_data;				// vol_data_start
	*(unsigned int*)(sector_buffer+(0x154)) = time(NULL);				// vol_mount_time
	*(unsigned int*)(sector_buffer+(0x158)) = time(NULL)+1;				// vol_unmount_time
	
	fseek(disk_image, (current_partition_offset*512), SEEK_SET);
	if (!dryrun)
		fwrite(sector_buffer, 512, 1, disk_image);
	
	if (be_verbose)
		printf("-- Writing VBR (%d bytes to 0x%08X).\n", 512, (current_partition_offset*512));
	if (be_verbose)
		printf("Updated volume boot record with volume info.\n");
	
	printf("calloc'ing blocktable cache, this might take a while for large volumes\n");
	unsigned char* blocktable_cache = (unsigned char*)calloc(sectors_blocktable, 512);
	printf("calloc'd blocktable cache, calculating blocktable information\n");
	if (be_verbose)
		printf("-- Blocktable is %u bytes long.\n", sectors_blocktable*512);
	
	unsigned int blocktable_bytes_used = sectors_blocktable + 1;	// One extra byte for the root dir we'll create
	memset(blocktable_cache, 0x01, blocktable_bytes_used);
	
	printf("Writing blocktable to partition, this WILL take a while for large volumes\n");
	
	fseek(disk_image, (current_partition_offset*512)+(offset_blocktable*512), SEEK_SET);
	if (be_verbose)
		printf("-- Blocktable has %d bytes used.\n", blocktable_bytes_used);
	if (be_verbose)
		printf("-- Writing blocktable (%d bytes to 0x%08X).\n", 512*sectors_blocktable, (current_partition_offset*512)+(offset_blocktable*512));
	if (!dryrun)
		fwrite(blocktable_cache, 512, sectors_blocktable, disk_image);
		
	printf("Blocktable written to the partition.\n");
	
	if (be_verbose)
		printf("Creating root directory.\n");
	memset(sector_buffer, 0x00, 512);
	*(unsigned int*)(sector_buffer+(0x000)) = 0x00000001;	// fileblock ident
	*(unsigned int*)(sector_buffer+(0x004)) = 0x00000000;	// single sector, that's it
	*(unsigned int*)(sector_buffer+(0x008)) = 0x00000100;	// an empty directory contains . and ..
	*(unsigned int*)(sector_buffer+(0x00C)) = 0x00000003;	// flags: directory, system
	
	/*sector_buffer[0x100] = '.';
	sector_buffer[0x101] = 0;
	*(unsigned int*)(sector_buffer+(0x102)) = offset_data;
	
	sector_buffer[0x106] = '.';
	sector_buffer[0x107] = '.';
	sector_buffer[0x108] = 0;
	*(unsigned int*)(sector_buffer+(0x109)) = offset_data;*/
	memcpy(sector_buffer+0x100, format_dent(".", offset_data), 128);
	memcpy(sector_buffer+0x180, format_dent("..", offset_data), 128);
	
	fseek(disk_image, (current_partition_offset*512)+(offset_data*512), SEEK_SET);
	if (be_verbose)
		printf("Writing root directory.\n");
	if (be_verbose)
		printf("-- (%d bytes to 0x%08X).\n", 512, (current_partition_offset*512)+(offset_data*512));
	if (!dryrun)
		fwrite(sector_buffer, 512, 1, disk_image);
	
	if (dryrun)
		printf("\nDry run complete.");
	printf("\nSuccess! Created filesystem on partition %d.\n", current_partition);
	if (be_verbose)
		printf("-- Freeing blocktable cache.\n");
	free(blocktable_cache);
	printf("\n");
}

void command_inject_mbr(int argc, char* argv[]) {
	fseek(disk_image, 0, SEEK_SET);
	fread(sector_buffer, 512, 1, disk_image);
	
	/*if (current_partition == -1) {
		printf("No partition is selected.\n");
		return;
	}*/
	
	if (argc < 2) {
		printf("Type \"mbr confirm\" to inject the boot code at ../../boot/evofs/mbr.\n");
		printf("WARNING: This will erase existing MBR boot code. Partitions will not be harmed.\n");
		return;
	}
	
	if (strcmp(argv[1], "confirm")) {
		printf("You didn't spell \"confirm\" right.\n");
		return;
	}
	
	FILE* file_mbr = fopen("../../boot/evofs/mbr", "rb");
	if (!file_mbr) {
		printf("Error: something went wrong opening ../../boot/evofs/mbr for read.\n");
		return;
	}
	
	fseek(file_mbr, 0, SEEK_SET);
	fread(sector_buffer, 440, 1, file_mbr);
	
	fseek(disk_image, 0, SEEK_SET);
	fwrite(sector_buffer, 512, 1, disk_image);
	
	fclose(file_mbr);
	printf("MBR injected.\n");
}

void command_inject_vbr(int argc, char* argv[]) {
	fseek(disk_image, (current_partition_offset*512), SEEK_SET);
	fread(sector_buffer, 512, 1, disk_image);
	
	if (current_partition == -1) {
		printf("No partition is selected.\n");
		return;
	}
	
	if (argc < 2) {
		printf("Type \"vbr confirm\" to inject the boot code at ../../boot/evofs/vbr.\n");
		printf("WARNING: This will erase existing VBR boot code. FS info will not be harmed.\n");
		return;
	}
	
	if (strcmp(argv[1], "confirm")) {
		printf("You didn't spell \"confirm\" right.\n");
		return;
	}
	
	FILE* file_vbr = fopen("../../boot/evofs/vbr", "rb");
	if (!file_vbr) {
		printf("Error: something went wrong opening ../../boot/evofs/vbr for read.\n");
		return;
	}
	
	fseek(file_vbr, 0, SEEK_SET);
	fread(sector_buffer, 320, 1, file_vbr);
	
	sector_buffer[510] = 0x55;
	sector_buffer[511] = 0xAA;
	
	fseek(disk_image, (current_partition_offset*512), SEEK_SET);
	fwrite(sector_buffer, 512, 1, disk_image);
	
	fclose(file_vbr);
	printf("VBR injected.\n");
}

void command_inject_stage2(int argc, char* argv[]) {
	unsigned char* stage2_buffer = malloc(512*60);
	int i;
	
	if (current_partition == -1) {
		printf("No partition is selected.\n");
		return;
	}
	
	if (argc < 2) {
		printf("Type \"stage2 confirm\" to inject the boot code at ../../boot/evofs/stage2.\n");
		printf("WARNING: This will erase existing stage2 boot code. FS info will not be harmed.\n");
		return;
	}
	
	if (strcmp(argv[1], "confirm")) {
		printf("You didn't spell \"confirm\" right.\n");
		return;
	}
	
	FILE* file_stage2 = fopen("../../boot/evofs/stage2", "rb");
	if (!file_stage2) {
		printf("Error: something went wrong opening ../../boot/evofs/stage2 for read.\n");
		return;
	}
	
	fseek(file_stage2, 0, SEEK_SET);
	if ((i = fread(stage2_buffer, 512*60, 1, file_stage2)) != 1)
		printf("something went wrong: %d\n", i);
	
	fseek(disk_image, (current_partition_offset*512)+512, SEEK_SET);
	if ((i = fwrite(stage2_buffer, 1, 512*60, disk_image)) != 512*60)
		printf("something went wrong: %d\n", i);
	
	fclose(file_stage2);
	printf("stage2 injected.\n");
}

int main(int argc, char* argv[]) {
	//int i = 0;
	struct stat st_diskimage;
	char command_input[150];
	char* commandv[32];
	
	char* argv_dummy_confirm[] = {"dummy", "confirm", };
	
	int exec_vbr = 0;
	int exec_stage2 = 0;
	
	memset(sector_buffer, 0, 512);
	
	for (int i = 0; i < argc; i++) {
		if (argc == 1)
			break;
		
		if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--verbose")) {
			verbose = 1;
			continue;
		}
		
		if (!strcmp(argv[i], "-?") || !strcmp(argv[i], "--help")) {
			print_usage(1);
			return 0;
		}
		
		if (!strcmp(argv[i], "-p0")) {
			current_partition = 0;
			continue;
		}
		else if (!strcmp(argv[i], "-p1")) {
			current_partition = 1;
			continue;
		}
		else if (!strcmp(argv[i], "-p2")) {
			current_partition = 2;
			continue;
		}
		else if (!strcmp(argv[i], "-p3")) {
			current_partition = 3;
			continue;
		}
		else if (!strncmp(argv[i], "-p", 2)) {
			fprintf(stderr, "Error: invalid partition \"%s\".\n", argv[i]+2);
			continue;
		}
		
		if (!strcmp(argv[i], "--vbr")) {
			exec_vbr = 1;
			continue;
		}
		
		if (!strcmp(argv[i], "--stage2")) {
			exec_stage2 = 1;
			continue;
		}
			
		if (i == argc-1) {
			image_filename = argv[i];
			break;
		}
	}
	
	if (image_filename == 0) {
		print_usage(0);
		return -1;
	}
	
	disk_image = fopen(image_filename, "rb+");
	
	if (!disk_image) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: %s doesn't exist.\n", image_filename);
			print_usage(0);
			return -1;
		}
	}

    stat(image_filename, &st_diskimage);
    disk_image_bytes = st_diskimage.st_size;
    disk_image_sectors = disk_image_bytes / 512;
	
	printf("Disk image is %u sectors (%s) long.\n", disk_image_sectors, minimum_size(disk_image_sectors));
	
	//printf("sizeof(off_t) == %lu\n", sizeof(off_t));
	
	//command_list_partitions(0, 0);
	
	if (current_partition != -1)
		command_select_partition(2, argv_dummy_confirm);
	
	if (exec_vbr)
		command_inject_vbr(2, argv_dummy_confirm);
	
	if (exec_stage2)
		command_inject_stage2(2, argv_dummy_confirm);
	
	if (exec_vbr || exec_stage2) {
		fclose(disk_image);
		return 0;
	}
	
	printf("\nEVOfs Toolbox - version 0\n");
	printf("Not sure what to do? Try the \"help\" or \"list\" commands.\n");
	
	for (;;) {
		int command_count = 0;
		if (current_partition == -1)
			printf("> ");
		else
			printf("%02X> ", current_partition);
		
		fgets(command_input, 150, stdin);
		if (strrchr(command_input, '\n'))
			*(strrchr(command_input, '\n')) = 0;
			
		if (!strlen(command_input))
			continue;
		
		char* cmdp = strtok(command_input, " ");
		while (cmdp) {
			commandv[command_count++] = cmdp;
			cmdp = strtok(NULL, " ");
		}
		
		/*printf("command_count = %d\n", command_count);
		for (int i = 0; i < command_count; i++) {
			printf("commandv[%d] = \"%s\"\n", i, commandv[i]);
		}*/
		
		if (!strcmp(commandv[0], "list") || !strcmp(commandv[0], "l"))
			command_list_partitions(command_count, commandv);
		else if (!strcmp(commandv[0], "quit") || !strcmp(commandv[0], "exit") || !strcmp(commandv[0], "q") || !strcmp(commandv[0], "ex"))
			break;
		else if (!strcmp(commandv[0], "select") || !strcmp(commandv[0], "s"))
			command_select_partition(command_count, commandv);
		else if (!strcmp(commandv[0], "create") || !strcmp(commandv[0], "c"))
			command_create_filesystem(command_count, commandv);
		else if (!strcmp(commandv[0], "mbr") || !strcmp(commandv[0], "m"))
			command_inject_mbr(command_count, commandv);
		else if (!strcmp(commandv[0], "vbr") || !strcmp(commandv[0], "v"))
			command_inject_vbr(command_count, commandv);
		else if (!strcmp(commandv[0], "help") || !strcmp(commandv[0], "?"))
			command_display_help(command_count, commandv);
		else
			printf("No such command \"%s\".\n", commandv[0]);
	}
	
	fclose(disk_image);
	return 0;
}
