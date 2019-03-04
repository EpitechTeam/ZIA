int testsEntity();
int testsThreadPool();
int testsPipeline();
int testsEvents();
int testsContext();
int testsOrchestrator();

int main() {
	// zany::Header non("NOT A DATE");
	// zany::Header oui("Sun, 06 Nov 1994 08:49:37 GMT");

	// non.isDate();
	// oui.isDate();

	testsEntity();
	testsThreadPool();
	testsPipeline();
	testsEvents();
	testsContext();
	testsOrchestrator();
}
