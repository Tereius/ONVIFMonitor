//
// Created by bjoern on 20.11.21.
//

extern "C" {
#include "ffmpeg.h"
}

int main(int argc, char *argv[]) {

	char *buf[5] = {"test", "-i", "/mnt/Common/Videos/ASMR.mkv", "test.mkv"};
	run(4, buf);
	int test = 3;
	test++;
}
