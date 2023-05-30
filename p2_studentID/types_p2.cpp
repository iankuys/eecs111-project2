#include "types_p2.h"
#include "utils.h"

using namespace std;

extern struct timeval t_global_start;

void Person::set_gender(int data) { gender = data; }
int Person::get_gender(void)      { return gender; }

void Person::set_order(unsigned long data) { order = data; }
unsigned long Person::get_order(void)      { return order; }

void Person::set_use_order(unsigned long data) { use_order = data; }
unsigned long Person::get_use_order(void)      { return use_order; }

void Person::set_time(long data) { time_to_stay_ms = data; }
int Person::ready_to_leave(void) {
	struct timeval t_curr;
	gettimeofday(&t_curr, NULL);

	if (get_elasped_time(t_start, t_curr) >= time_to_stay_ms) { return 1; }
	else { return 0; }
}




void Person::start(void) {
	gettimeofday(&t_start, NULL);
	printf("(%lu)th person enters the restroom: \n", order);
	printf(" - (%lu) milliseconds after the creation\n", get_elasped_time(t_create, t_start));
}

void Person::complete(void) {
	gettimeofday(&t_end, NULL);
	printf("(%lu)th person comes out of the restroom: \n", order);
	printf(" - (%lu) milliseconds after the creation\n", get_elasped_time(t_create, t_end));
	printf(" - (%lu) milliseconds after using the restroom\n", get_elasped_time(t_start, t_end));
}

Person::Person() {
	gettimeofday(&t_create, NULL);
}



// You need to use this function to print the Restroom's status
void Restroom::print_status(void) {
	printf("Print restroom status\n");
	string status_str;
	if (status == EMPTY){
		status_str = "Empty";
	} else if (status == MENPRESENT){
		status_str = "MenPresent";
	} else {
		status_str = "WomenPresent";
	}

	printf("(%s)", status_str.c_str());
}


// Call by reference
// This is just an example. You can implement any function you need
void Restroom::add_person(Person& p) {
	// Do nothing;
	int gender = p.get_gender();
	if (gender == 0){
		queue_male++;
	}else{
		queue_female++;
	}

	queue_total = queue_male + queue_female;
}

void Restroom::man_wants_to_enter(Person& p){
	struct timeval t_curr;

	int t_interval = rand() % 8 + 3; // time to stay in the restroom (3 milliseconds – 10 milliseconds)
	p.set_time(t_interval);
	gettimeofday(&t_curr, NULL);
	queue_male--;
	queue_total =  queue_male + queue_female;
	printf("[%lu ms][Queue] Send (Man) into the restroom (Stays %d ms), Status: ", get_elasped_time(t_global_start, t_curr), t_interval);
	printf("Total: %d (Men: %d, Women: %d)\n", queue_total, queue_male, queue_female);
	queue_male++;
	used_total = queue_male + used_female;
	status = MENPRESENT;
	gettimeofday(&t_curr, NULL);
	printf("[%lu ms][Restroom] (Man) goes into the restroom, State is ", get_elasped_time(t_global_start, t_curr));
	print_status();
	printf(": Total: %d (Men: %d, Women: %d)\n", used_total, queue_male, used_female);
	p.start();

}

void Restroom::woman_wants_to_enter(Person& p){
	struct timeval t_curr;

	int t_interval = rand() % 8 + 3; // time to stay in the restroom (3 milliseconds – 10 milliseconds)
	p.set_time(t_interval);
	gettimeofday(&t_curr, NULL);
	queue_female--;
	queue_total =  queue_male + queue_female;
	printf("[%lu ms][Queue] Send (Man) into the restroom (Stays %d ms), Status: ", get_elasped_time(t_global_start, t_curr), t_interval);
	printf("Total: %d (Men: %d, Women: %d)\n", queue_total, queue_male, queue_female);
	used_female++;
	used_total = queue_male + used_female;
	status = WOMENPRESENT;
	gettimeofday(&t_curr, NULL);
	printf("[%lu ms][Restroom] (Man) goes into the restroom, State is ", get_elasped_time(t_global_start, t_curr));
	print_status();
	printf(": Total: %d (Men: %d, Women: %d)\n", used_total, queue_male, used_female);
	p.start();

}

void Restroom::man_leaves(Person& p){
	struct timeval t_curr;
	int oldStatus = status;
	string outputString;

	queue_male--;
	used_total = queue_male + used_female;

	if (used_total == 0){
		status = EMPTY;
		outputString = "Empty";
	} else {
		outputString = "MenPresent";
	}
	p.complete();
	
	gettimeofday(&t_curr, NULL);
	printf("[%lu ms][Restroom] (Man) left the restroom. ", get_elasped_time(t_global_start, t_curr));
	if(status == oldStatus){
		printf("State is ");
	}
	else{
		printf("Status is changed, status is ");
	}
    print_status();
	printf(" : Total: %d (Men: %d, Women: %d)\n", used_total, queue_male, used_female);
}

void Restroom::woman_leaves(Person& p){
	struct timeval t_curr;
	int oldStatus = status;
	string outputString;

	used_female--;
	used_total = queue_male + used_female;

	if (used_total == 0){
		status = EMPTY;
		outputString = "Empty";
	} else {
		outputString = "WomenPresent";
	}
	p.complete();
	
	gettimeofday(&t_curr, NULL);
	printf("[%lu ms][Restroom] (Man) left the restroom. ", get_elasped_time(t_global_start, t_curr));
	if(status == oldStatus){
		printf("State is ");
	}
	else{
		printf("Status is changed, status is ");
	}
    print_status();
	printf(" : Total: %d (Men: %d, Women: %d)\n", used_total, queue_male, used_female);
}

bool Restroom::cleared_to_enter(Person& p){
	return (status == EMPTY) || ((p.get_gender() == 0) && (status == MENPRESENT)) || ((p.get_gender() == 1) && (status == WOMENPRESENT));
}