# shooting_alg_test.py

# Created to test a shooting algorithm for the space invader enemies
# Jamie Williams - 2019

# User controls: F to fire A to quit



import random
import cv2
import numpy as np
import math

# Window used for keyboard input
cv2.namedWindow('user input')
img = np.zeros((300,300,3), np.uint8)

# Numbers used to change the rate of fire:

# prev #s: 
#rand_max = 39
#fire_rate = .15
#fire_rate_per = .9
#shoot_compare = 5

rand_max = 29
fire_rate = .10
fire_rate_per = .6
shoot_compare = 2.5


# Alien class:
class Alien:
    def __init__(self, x_cor, y_cor):
        self.x = x_cor
        self.y = y_cor
        self.is_in_front = False
        self.disp_coor()
        print()
    def set_front(self):
        self.is_in_front = True
    def is_front(self):
        return self.is_in_front
    def disp_coor(self):
        print ("Alien at: ", self.x, ",", self.y, end='')
    def disp_front(self):
        add_string = " NOT "
        if self.is_in_front:
            add_string = " "
        self.disp_coor()
        print ("  is", add_string, "in front.")
    def disp_shot(self):
        self.disp_coor()
        print("  has been destroyed!  ***")
        
    def fire_weapon(self, num_col_left):
        if  not self.is_in_front:
            should_fire = 0
            #print("Current alien not in the front row!")
        else:
            #should_fire = fire_rate * random.randint(0,rand_max)
            if num_col_left < 2:
                num_col_left = 2
            should_fire = (fire_rate_per/num_col_left) * random.randint(0,rand_max)
            #print("Firing value is: ", should_fire)
        if should_fire >= shoot_compare:
            will_fire = True
            self.disp_coor()
            print("  fired! ---")
        else:
            will_fire = False
        return will_fire

# Class for the 2D array ("swarm") of aliens
class Alien_Swarm:
    def __init__(self, x_total, y_total):
        self.x_max = (x_total-1)
        self.y_max = (y_total-1)
        self.x_total = x_total
        self.y_total = y_total
        self.swarm = [[Alien(x,y) for y in range(self.y_total)] for x in range(self.x_total)]
        self.determine_front
        self.num_aliens = x_total * y_total
        self.active_cols = [x for x in range(self.x_total)]
    def print_stats(self):
        print("Alien swarm with values: ")
        print("X Max: ", self.x_max, "  Y Max: ", self.y_max)
        print("Total number of aliens: ", self.num_aliens)
    def determine_front(self):
        for x in range(self.x_total):
            found_front = False
            for y in range(self.y_max, -1, -1):
                # Determine if in front, then set value if so
                # So, is 0,0 upper left? Then might want to count from y_max down
                if found_front == False:
                    if  not (self.swarm[x][y] is None):
                        self.swarm[x][y].set_front()
                        #self.swarm[x][y].disp_front()
                        found_front = True
    def print_swarm(self):
        print("SWARM VALUES: ")
        for y in range(self.y_total):
            for x in range(self.x_total):
                print("Parent location: ", x,",",y," and: ", end='')
                self.swarm[x][y].disp_coor()
                print()
        print("END SWARM")
                
    def shoot_alien(self, x_loc):
        #delete the alien in front
        found_target = False
        for y in range(self.y_max, -1, -1):
            # So, is 0,0 upper left? Then might want to count from y_max down
            #print("Looking for target at: ", x_loc, ",",y)
            if found_target == False:
                if  not (self.swarm[x_loc][y] is None):
                    self.swarm[x_loc][y].disp_shot()
                    self.swarm[x_loc][y] = None
                    found_target = True
        if found_target == True:
            self.determine_front()
            self.num_aliens = self.num_aliens - 1
        else:
            if x_loc in self.active_cols:
                self.active_cols.remove(x_loc)
            print("Oh, no! Shot to column: ", x_loc, " missed for lack of targets!")
        return found_target
        #run determine front, again (just do it for row or do I call function?)
        #Decrement num_aliens
        #Return False if no aliens left (after shooting)
    
    def aliens_fire(self):
        #print("... charging lasers")
        #All aliens get a chance to fire
        for x in range(self.x_total):
            for y in range(self.y_total):
                #print("Shooter location: ", x,",",y, end='')
                if not (self.swarm[x][y] is None):
                    #print("  - valid shooter!")
                    self.swarm[x][y].fire_weapon(len(self.active_cols))
    def get_num_aliens(self):
        return self.num_aliens

def fire_at_aliens(this_swarm):
     # Call shoot_alien on random x location
     shoot_at_col = random.randint(0,this_swarm.x_max)
     result = this_swarm.shoot_alien(shoot_at_col)
     return result
     # Return false if no aliens left (after shooting


 


                
# Here goes the main code


# Create instance of swarm
bad_guys = Alien_Swarm(6, 3)
bad_guys.print_stats()
#bad_guys.print_swarm()


# Create a counter, initialize
timer_base = 70
timer_max = timer_base * ((bad_guys.get_num_aliens()))
#timer_max = 15
timer_current = 0
running_game = True

# Header for the simulation "game"
print ("STARTING SPACE INVADERS SHOOTING TIMING SIM ...")
print()
print ("Press the 'f' key to shoot, and press the 'a' key to exit")
print ()
print()
print ("GAME START")
print ("_______________________________________________________")


# Main loop
while(running_game):
    cv2.imshow('user input',img)
    #   If current_timer == timer_max: aliens fire
    if timer_current >= timer_max:
        timer_current = 0
        # Aliens attempt to shoot
        print("THE ALIENS ARE SHOOTING AT YOU!")
        bad_guys.aliens_fire()

    # Get any key press information
    key_press = cv2.waitKey(1) & 0xFF
    #time.sleep(.01)

    # User controls: F to fire A to quit
    if key_press == ord('f'):
        #randomly destroy an alien in front
        sucess = fire_at_aliens(bad_guys)
        #print("Noted f key press")
    elif key_press == ord('a'):
        running_game = False
        #print("Noted a key press")
        
    #   Update timer_max based on # of aliens left (this is a hack, but eh)
    aliens_left = bad_guys.get_num_aliens()
    #   If num_aliens == 0, running_game = false
    if aliens_left == 0:
        running_game = False
    timer_max = timer_base * (aliens_left)

    #   Increment timer
    timer_current = timer_current + 1

# Closing display information
cv2.destroyAllWindows()
print ()
print ()
print ("**********************************************************")
print ("THE GAME IS OVER!")
print ()
print ("THE NUMBER OF ALIENS LEFT IS: ", aliens_left)
print ()
    

        
    

