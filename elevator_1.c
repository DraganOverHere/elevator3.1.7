//VARIABLES
int currentFloor = 1; //floor elevator is currently in
int queue[3];	//create an array "queue" of size 3
int target;	//represents the floor elevator is headed to
bool vbtn1;	//variable checking if first button is pressed (to "debounce")
bool vbtn2;	//variable checking if second button is pressed
bool vbtn3;	//variable checking if third button is pressed

//QUEUE FUNCTIONS AND PROCESS
bool queueContains(int n)	//checks if the queue contains a floor
{
	for (int i = 0; i < sizeof(queue)/sizeof(int); i++) if (queue[i]==n) return true;	//if floor is in the queue, return true
	return false;	//else return false
}

bool addToQueue(int flr)	//function that adds floors to queue
{
	if (currentFloor != flr && !queueContains(flr)) //if the elevator isn't already on that floor or queued to go to that floor
	{
		for (int i = 0; i < sizeof(queue)/sizeof(int); i++) //check each value in the queue
		{
			if (queue[i]==0)	//if index i of array queue is 0
			{
				queue[i]=flr; //add the floor to that location in the array
				return true; //don't check any other locations, return that the floor was added
			}
		}
	}
	return false; //return that the floor wasn't added
}

task changeLEDs()	//task that changes LEDs according to the height
{
	while(1)	//change LEDs when elevator changes floors, by height
	{
		int height = SensorValue(sonar);
		if(height > 0 && height < 90)	//first floor
		{
			SensorValue[led1] = 1;	//LED for first floor is on
			SensorValue[led2] = 0;	//LED for second floor is off
			SensorValue[led3] = 0;	//LED for third floor is off
		}

		else if(height > 90 && height <150)	//second floor
		{
			SensorValue[led1] = 0;	//LED for first floor is off
			SensorValue[led2] = 1;	//LED for second floor is on
			SensorValue[led3] = 0;	//LED for third floor is off
		}

		else if(height > 150 && height < 210)	//third floor
		{
			SensorValue[led1] = 0;	//LED for first floor is off
			SensorValue[led2] = 0;	//LED for second floor is off
			SensorValue[led3] = 1;	//LED for third floor is on
		}
	}
}

task queueManager()	//task that manages queue array
{
	while(1) //poll the states of the 3 buttons, and if one is pressed add its floor to the queue
	{
		if(SensorValue(btn1) && !vbtn1) //on the rising edge of button 1
		{
			vbtn1 = true;	//make vbtn1 true, indicating that button 1 is pressed (for edge detection)
			addToQueue(1);	//add floor 1 to the queue
			clearTimer(T1); //clears timer, used for safety mechanism
		}
		else if(!SensorValue(btn1)) vbtn1 = false;	//if button 1 is not pressed, vbtn1 = false

		if(SensorValue(btn2) && !vbtn2) //on the rising edge of button 2
		{
			vbtn2 = true;	//make vbtn2 true, indicating that button 2 is pressed (for edge detection)
			addToQueue(2);	//add floor 2 to the queue
			clearTimer(T1);	//clears timer, used for safety mechanism
		}
		else if(!SensorValue(btn2)) vbtn2 = false;	//if button 2 is not pressed, vbtn1 = false

		if(SensorValue(btn3) && !vbtn3) //on the rising edge of button 3
		{
			vbtn3 = true;	//make vbtn3 true, indicating that button 3 is pressed (for edge detection)
			addToQueue(3);	//add floor 3 to the queue
			clearTimer(T1);	//clears timer, used for safety mechanism
		}
		else if(!SensorValue(btn3)) vbtn3 = false;	//if button 3 is not pressed, vbtn1 = false

		wait1Msec(100); //delay to fix logic issues
	}
}

//MAIN TASK AND PROCESS
task main()	//main task
{
	startTask(queueManager);	//start task "queueManager"
	startTask(changeLEDs);	//start task "changeLEDs"
	currentFloor = 0;	//set currentFloor to 0
	addToQueue(1);	//add floor 1 to queue, making the elevator start at floor 1
	while(1) //add elevator control code here
	{
		if(time1(T1)>10000) addToQueue(1);	//if timer reaches 10 seconds go to first floor
		if(queue[0] != 0)	//if there is a floor in the queue
		{
			currentFloor = queue[0];	//make currentFloor equal to the queued floor
			target = queue[0]*60;	//target floor is at height of desired floor times 60 mm
			if (SensorValue(sonar) < target)	//if current floor is lower than desired floor, go up
			{
				motor[liftMotor] = 127;	//goes up
				while(SensorValue(sonar) < target){}	//while the sonar value is less than the target height
				motor[liftMotor] = -10;	//goes the other way for .1 seconds to exactly stop
				wait1Msec(100);	//for .1 seconds
				motor[liftMotor] = 0;	//stop
			}
			else if (SensorValue(sonar) > target)	//if current floor is higher than desired floor, go down
			{
				motor[liftMotor] = -127;	//go down
				while(SensorValue(sonar) > target){}	//while the sonar value is greater than the target height
				motor[liftMotor] = 10;	//goes the other way for .1 seconds to exactly stop
				wait1Msec(100);	//for .1 seconds
				motor[liftMotor] = 0;	//stop
			}
			queue[0] = queue[1];	//removes first queue as it is used, moves the next queue to the first queue
			queue[1] = queue[2];	//moves the third queue to the second queue
			queue[2] = 0;	//third place of the queue is empty(0)
		}
	}
}
