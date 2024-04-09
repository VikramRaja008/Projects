import numpy as np
import pygame
import random 
import torch 
import os
import matplotlib.pyplot as plt
import seaborn as sns
from model import Linear_QNet,QTrainer
from maze import Maze
from collections import deque
MAX_MEMORY = 100_000
BATCH_SIZE = 1000
LR = 0.001


class Agent:

    def __init__(self):
        self.n_game = 0
        self.epsilon = 0 # Randomness
        self.gamma = 0.9 # discount rate
        self.memory = deque(maxlen=MAX_MEMORY) # popleft()
        self.model = Linear_QNet(13,256,4) 
        if os.path.exists('model.pth'):
            self.model.load_state_dict(torch.load('model.pth'))
            self.model.eval()
        self.trainer = QTrainer(self.model,lr=LR,gamma=self.gamma)
    # ... (other methods and initialization)

    def get_state(self, maze, player_x, player_y, goal_x, goal_y):
        # Check for walls around the player

        wall_ahead = maze[player_y - 1][player_x] == 0
        wall_to_right = maze[player_y][player_x + 1] == 0
        wall_to_left = maze[player_y][player_x - 1] == 0
        wall_above = maze[goal_y - 1][goal_x] == 0
        wall_below = maze[goal_y + 1][goal_x] == 0

        # Determine goal direction
        goal_direction = np.zeros(4)
        if goal_x < player_x:
            goal_direction[2] = 1  # Left
        elif goal_x > player_x:
            goal_direction[3] = 1  # Right
        if goal_y < player_y:
            goal_direction[0] = 1  # Up
        elif goal_y > player_y:
            goal_direction[1] = 1  # Down

        # Calculate distance to goal
        distance_to_goal = np.sqrt((player_x - goal_x) ** 2 + (player_y - goal_y) ** 2)

        # Encode player direction based on player's movement
        dx = goal_x - player_x
        dy = goal_y - player_y
        if abs(dx) > abs(dy):
            player_direction = 2 if dx < 0 else 3
        else:
            player_direction = 0 if dy < 0 else 1

        # Combine all components into the state
        state = np.array([
            wall_ahead, wall_to_right, wall_to_left, wall_above, wall_below,
            *goal_direction, distance_to_goal,
            player_direction,
            player_x, player_y
        ], dtype=int)

        # ... (print the state components as before)

        return state

    def remember(self,state,action,reward,next_state,done):
        self.memory.append((state,action,reward,next_state,done)) # popleft if memory exceed

    def train_long_memory(self):
        if (len(self.memory) > BATCH_SIZE):
            mini_sample = random.sample(self.memory,BATCH_SIZE)
        else:
            mini_sample = self.memory
        states,actions,rewards,next_states,dones = zip(*mini_sample)
        self.trainer.train_step(states,actions,rewards,next_states,dones)

    def train_short_memory(self,state,action,reward,next_state,done):
        self.trainer.train_step(state,action,reward,next_state,done)

    def get_action(self, state):
        self.epsilon = 80 - self.n_game
        final_move = [0, 0, 0, 0]  # Assuming 4 possible actions: [up, down, left, right]

        if random.randint(0, 200) < self.epsilon:
            move = random.randint(0, 3)  # Randomly choose an action index
            final_move[move] = 1
        else:
            state_tensor = torch.tensor(state, dtype=torch.float)
            prediction = self.model(state_tensor)
            move = torch.argmax(prediction).item()
            final_move[move] = 1

        return final_move

def train():
    agent = Agent()
    game = Maze()
    
    # Run the game loop
    def run():
        while True:
            current_state = agent.get_state(game.maze, game.player.x, game.player.y, game.goal.x, game.goal.y)
            action = agent.get_action(current_state)  # Replace 'current_state' with the actual state
            reward, done= game.move_player(action) 
            state_new = agent.get_state(game.maze, game.player.x, game.player.y, game.goal.x, game.goal.y)
            agent.train_short_memory(current_state,action,reward,state_new,done)
            #remember
            agent.remember(current_state,action,reward,state_new,done)

            if done:
                # Train long memory, plot result
                game.reset()
                agent.n_game += 1
                agent.train_long_memory()
                agent.model.save()


    # Run the game loop
    run()
  

if(__name__=="__main__"):
    train()