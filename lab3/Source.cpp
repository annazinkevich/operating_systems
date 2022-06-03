#include<iostream>
#include<windows.h>
using namespace std;

HANDLE hSemaphoreEmpty;
HANDLE hSemaphoreFull;
HANDLE hSemaphoreMutex;

class SyncQueue {

	int* queue;
	int count;
	int size;
	int producedNumber = 0;
	int consumedNumber = 0;

public:

	SyncQueue(int size) {
		count = 0;
		this->size = size;
	}

	~SyncQueue(){}

	void Insert(int element) {

		WaitForSingleObject(hSemaphoreEmpty, INFINITE);
		WaitForSingleObject(hSemaphoreMutex, INFINITE);

		int* tmp;
		tmp = queue;
		queue = new int[count + 1];

		for (int i = 0;i < count;i++) {
			queue[i] = tmp[i];
		}

		queue[count] = element;
		count++;
		producedNumber++;

		if (count > 1) {
			delete[]tmp;
		}

		ReleaseSemaphore(hSemaphoreMutex, 1, NULL);
		ReleaseSemaphore(hSemaphoreFull, 1, NULL);
		
	}

	int remove() {
		WaitForSingleObject(hSemaphoreFull, INFINITE);
		WaitForSingleObject(hSemaphoreMutex, INFINITE);

		int* tmp;
		tmp = queue;
		queue = new int[count - 1];
		int element = tmp[0];

		for (int i = 0;i < count;i++) {
			queue[i] = tmp[i+1];
		}

		count--;
		consumedNumber++;

		if (count >= 0) {
			delete[]tmp;
		}

		ReleaseSemaphore(hSemaphoreMutex, 1, NULL);
		ReleaseSemaphore(hSemaphoreEmpty, 1, NULL);

		return element;
	}

	int getProducedNumber() {
		return producedNumber;
	}

	int getConsumedNumber() {

		return consumedNumber;

	}
};

struct arg {

	int id;
	int n;
	SyncQueue* queue;

};

void producer(arg& producerArgs) {

	for (int i = 0;i < producerArgs.n;i++) {
		(*producerArgs.queue).Insert(producerArgs.id);
		cout << "Produced number: "<<(*producerArgs.queue).getProducedNumber()<<endl;
		Sleep(7);
	}

}

void consumer(arg& consumerArgs) {

	for (int i = 0;i < consumerArgs.n;i++) {
		(*consumerArgs.queue).remove();
		cout << "Consumed number: " << (*consumerArgs.queue).getConsumedNumber() << endl;
		Sleep(7);
	}

}

int main() {

	cout << "n=";
	int n;
	cin >> n;

	SyncQueue queue(n);

	cout << "number of producer threads:";
	int producerNumber;
	cin >> producerNumber;

	cout << "number of consumer threads:";
	int consumerNumber;
	cin >> consumerNumber;

	arg * producerArgs = new arg[producerNumber];
	arg*consumerArgs = new arg[consumerNumber];
	
	cout << "Producers id and amount of numbers to produce:\n";
	for (int i = 0;i < producerNumber;i++) {
		cin >> producerArgs[i].id;
		cin >> producerArgs[i].n;
		producerArgs[i].queue = &queue;
	}

	cout << "Amount of numbers to consume:\n";
	for (int i = 0;i < consumerNumber;i++) {
		cin >> consumerArgs[i].n;
		consumerArgs[i].id = 1;
		consumerArgs[i].queue = &queue;
	}
	
	hSemaphoreEmpty = CreateSemaphore(NULL, n, n, L"SemaphoreEmpty");
	if (hSemaphoreEmpty == NULL) {
		return GetLastError();
	}

	hSemaphoreFull = CreateSemaphore(NULL, 0, n, L"SemaphoreFull");
	if (hSemaphoreFull == NULL) {
		return GetLastError();
	}

	hSemaphoreMutex = CreateSemaphore(NULL, 1, 1, L"SemaphoreMutex");
	if (hSemaphoreMutex == NULL) {
		return GetLastError();
	}

	HANDLE* hProducers = new HANDLE[producerNumber];
	DWORD IDhProducersThread;

	for (int i = 0;i < producerNumber;i++) {
		hProducers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)producer, &producerArgs[i], 0, &IDhProducersThread);
		if (hProducers[i] == NULL) {
			return GetLastError();
		}
	}

	HANDLE* hConsumers = new HANDLE[consumerNumber];
	DWORD IDhConsumerThread;

	for (int i = 0;i < consumerNumber;i++) {
		hConsumers[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)consumer, &consumerArgs[i], 0, &IDhConsumerThread);
		if (hConsumers[i] == NULL) {
			return GetLastError();
		}
	}

	WaitForMultipleObjects(producerNumber, hProducers, TRUE, INFINITE);
	WaitForMultipleObjects(consumerNumber, hConsumers, TRUE, INFINITE);

	for (int i = 0;i < producerNumber;i++) {
		CloseHandle(hProducers[i]);
	}

	for (int i = 0;i < consumerNumber;i++) {
		CloseHandle(hConsumers[i]);
	}

	CloseHandle(hSemaphoreEmpty);
	CloseHandle(hSemaphoreFull);
	CloseHandle(hSemaphoreMutex);

	return 0;
}