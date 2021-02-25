#include <stdio.h>
#include <wiringPi.h>

int main(void) {
	printf("Hello, World\n"
		"Danny Nuch\n"
		"4943\n"
	);
	
	wiringPiSetup();
	pinMode(2, OUTPUT);
	pinMode(0, INPUT);
	for (;;) {
		if (digitalRead(0)) {
			digitalWrite(2, HIGH);
		} else {
			digitalWrite(2, LOW);
		}
	}

	return 0;
}
