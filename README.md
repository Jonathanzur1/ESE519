# lab2-jzur
Hello,

A few notes:

When doing this assignment, I created a project for each of the parts, not just a new file. As a result, each of the files was originally named main.c. I have renamed all of the files in accordance with the assignment, but if this causes any issues, please let me know and I can remedy the situation.

When I try to open my video in an incognito tab, it seems to be taking a very long time to load. The video works when I play it on my alternate google account. If you have issues playing the video, please let me know. The link is https://drive.google.com/file/d/1rIlL3DPBWwJOWD8el1TP-N1IcnqJ2FyB/view?usp=sharing

My approach to part E is likely not the simplest method and needs some explanation. I initialized an array called tracker which has five integers. I then go about my usual timer setup using a prescaler of 256 so that I overflow approximately once each second. I set up an interrupt every time an overflow occurs which adds one to an overflow counter. I then set up an interrupt for calculating the period using values of ICR1 captured at button press and button release. In my while(1) loop, I create an if statement for every possibility of dot and dash combinations which would translate to a letter or number in Morse Code. If a dot is detected, I store the number three in my array Tracker. If a dash is detected, I store the number four. The if statements are written so that the value 3 or 4 is populated in the first empty (zero) slot available in the array. For instance, since the letter A is a dot followed by a dash, the array Tracker for A would be (3,4,0,0,0). To print the letter, I wait until the overflow counter has surpassed five. I then pass through a series of if statements which reads the array Tracker, prints the corresponding letter on the screen, and then resets the tracker. 
In the video, it takes a bit of time for the letter to appear on screen. This is because of the delay caused by waiting for the overflow to pass five. I did this on purpose because I am a slow button presser and wanted time to issue dots and dashes properly. This program then awaits the next entry. This all amounts to nearly 700 lines of code which seems extremely high for the task being performed. If ther are ways to simplify this, I would be interested in learning how. 

As of September 30th, I have tested all code submitted and it all works. If for whatever reason code is run and does not perform as it is supposed to, please let me know. 

Thank you,

-Jonathan Zur
