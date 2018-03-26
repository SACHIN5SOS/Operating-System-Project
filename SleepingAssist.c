#include <stdio.h>	
#include <pthread.h>
#include<math.h> 		
#include <semaphore.h>	
		
int count_chair = 0;
int ci = 0;
sem_t Teacher_Sem;
sem_t Student_Sem;
sem_t chairsem[3];
pthread_mutex_t AccessChair;
void *func1_teacher();          //thread for handling Teacher tasks             
void *func2_student(void *studThread_id);   //function for handling student task
void main()
{
	int students_prog;		
	int i,in,n;
	pthread_t *Student_thread;		 //thread for handling Teacher Tasks
	pthread_t Teacher_thread;		//thread for handling Stufents Tasks
	sem_init(&Teacher_Sem, 0, 0);
	sem_init(&Student_Sem, 0, 0);
	for(i = 0; i < 3; i++)			
	{	
		sem_init(&chairsem[i], 0, 1);		
	}
	printf("Enter the number of students you want: \n");
	scanf("%d",&n);
	in=rand()%10;
	pthread_mutex_init(&AccessChair, NULL);
	
	if(in<=1)
	{
		printf("Using Default values %d students \n",n);
		students_prog = n;
	}
	
	else
	{
		printf("Creating %d threads.\n",students_prog);
		students_prog = in;
	}
	
	Student_thread = (pthread_t*) malloc(sizeof(pthread_t)*students_prog);	 
	
	pthread_create(&Teacher_thread, NULL,func1_teacher, NULL);	
	
	for(i = 0; i <students_prog; i++)
	{
		pthread_create(&Student_thread[i], NULL,func2_student,(void*) (long)i);  
	}
	
	pthread_join(Teacher_thread, NULL);
	
	for(i = 0; i <students_prog; i++)
	{
		pthread_join(Student_thread[i], NULL);
	}
}

void *func2_student(void *studThread_id) 
{
	sleep(3);		

	printf("Student %ld go to Teaching Assistant \n", (long)studThread_id);
	pthread_mutex_lock(&AccessChair);
	int sum = count_chair;
	pthread_mutex_unlock(&AccessChair);
	if(sum < 3)		
	{
		if(sum == 0)		
		sem_post(&Teacher_Sem);

		else
			printf("Student %ld waiting for Teacher to finish \n", (long)studThread_id);      
		
		// lock
		pthread_mutex_lock(&AccessChair);
		int index = (ci + count_chair) % 3;
		count_chair++;
		printf("Student sat on chair. Remaining Chair %d\n", 3 - count_chair);
		pthread_mutex_unlock(&AccessChair);

		// unlock
		sem_wait(&chairsem[index]);		
		printf("Student %ld dicussing with Teacher Assistant \n", (long)studThread_id);
		sem_wait(&Student_Sem);		
		printf("Student %ld left in Room\n",(long)studThread_id);
	}
	
	else 
		printf("Student %ld will come back after some time\n", (long)studThread_id);
		
}

void *func1_teacher()
{
	sem_wait(&Teacher_Sem);		
	printf("\nTeacher Assistant was woken by student\t\n");

	while(1)
	{
		
		pthread_mutex_lock(&AccessChair);
		
		if(count_chair == 0) 
		{
			
			pthread_mutex_unlock(&AccessChair);
			break;
		}
		
		sem_post(&chairsem[ci]);
		count_chair--;
			
		// printf("Remaining Chair: %d\n", 3-count_chair);
		ci = (ci + 1) % 3;
		pthread_mutex_unlock(&AccessChair);

		printf("Teaching Assistant is helping student\n");
		sem_post(&Student_Sem);
		sleep(3);	
	}
}
