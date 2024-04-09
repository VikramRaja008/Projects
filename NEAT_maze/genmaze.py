import numpy as np
import pygame
import random 
import torch 
import os
import neat
import sys
import pickle
import visualize

# Initialize Pygame
pygame.init()

# Game constants
WIDTH, HEIGHT = 400, 400
CELL_SIZE = 40
MAZE_WIDTH = WIDTH // CELL_SIZE
MAZE_HEIGHT = HEIGHT // CELL_SIZE

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GREEN = (0, 255, 0)
RED = (255, 0, 0)
gen = 0

class Agent:

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

class Maze:
    def __init__(self):
        # Maze layout (0 represents walls, 1 represents empty cells, and 2 represents the goal)
        self.maze = [
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 1, 1, 1, 0, 1, 1, 1, 0],
    [0, 0, 0, 0, 1, 0, 1, 0, 1, 0],
    [0, 1, 1, 1, 1, 0, 1, 0, 1, 0],
    [0, 0, 0, 0, 1, 0, 1, 0, 1, 0],
    [0, 1, 1, 1, 1, 1, 1, 0, 1, 0],
    [0, 1, 0, 0, 0, 0, 1, 0, 1, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 2, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
]

        # Create screen
        self.visited_cells = [[False for _ in range(MAZE_WIDTH)] for _ in range(MAZE_HEIGHT)]

        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.player = self.Player(self.screen, self.maze, 1, 1)
        self.goal = self.Goal(self.screen, self.maze, 8, 8)
        pygame.display.set_caption('Maze Game')

        self.move_time_limit = 4000  # 4 seconds in milliseconds
        self.max_step_limit = 20
        
        self.last_move_time = pygame.time.get_ticks()

        # Initialize player and goal
        #print("Number of players:", len(self.players))

        self.score = 0
        self.font = pygame.font.Font(None, 36)

        self.clock = pygame.time.Clock()
        self.start_time = pygame.time.get_ticks() 

                
    def reset(self):
        # Generate a new random maze layout
      
      #  self.maze = generate_random_maze(WIDTH,HEIGHT)  # Implement a function to generate a random maze layout
        
        # Reset player and goal positions
        self.player.x = 1  # Reset player's starting position
        self.player.y = 1
        self.goal.x = 8  # Reset goal's position
        self.goal.y = 8
        self.score = 0  # Reset score

    def move_player(self, player_index, action,player):
        current_time = pygame.time.get_ticks()

 # Get the specific player

        player.step_count += 1
        #print(action)
        if action[0] == 1:  # Move Up
            player.move(0, -1)
        elif action[1] == 1:  # Move Down
            player.move(0, 1)
        elif action[2] == 1:  # Move Left
            player.move(-1, 0)
        elif action[3] == 1:  # Move Right
            player.move(1, 0)

        new_state = (player.x, player.y)
      
        done = False

        current_time = pygame.time.get_ticks()

        if player.step_count > self.max_step_limit:
            # Player has exceeded the maximum step limit, mark as "dead"
            return 0, True
        if action[0] == 1 or action[1] == 1 or action[2] == 1 or action[3] == 1:
            reward = 0
            reward+=1 # Positive reward for moving
        if new_state == (self.goal.x, self.goal.y):
            reward = 10  # Reward for reaching the goal
            done = True
        elif self.maze[new_state[1]][new_state[0]] == 0:
            reward = -1  # Penalty for hitting a wall
        # Check if the player is staying at the same cell
        elif new_state == (player.x, player.y):
            reward = -0.001  # Negative reward for staying at the same cell
        else:
            # Calculate Euclidean distance to the goal
            distance_to_goal = np.sqrt((player.x - self.goal.x) ** 2 + (player.y - self.goal.y) ** 2)
            
             # Exploration bonus: Reward for visiting new cells
              # Exploration bonus: Reward for visiting new cells
            if not self.visited_cells[new_state[1]][new_state[0]]:
                reward = 1.0  # Small bonus for exploration
                self.visited_cells[new_state[1]][new_state[0]] = True  # Mark the cell as visited
            else:
                reward = 0.0  # No additional reward for revisiting a cell


            # Adjust the reward based on the distance
            reward += 1 * distance_to_goal


        return reward, done

    # ... (other code) ...

    class Player:
        def __init__(self, screen, maze, x, y):
            self.maze = maze
            self.screen = screen 
            self.step_count=0
            self.x = x
            self.y = y
            self.radius = CELL_SIZE // 2
            self.color = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))

        def move(self, dx, dy):
            new_x = self.x + dx
            new_y = self.y + dy

            if 0 <= new_x < MAZE_WIDTH and 0 <= new_y < MAZE_HEIGHT and self.maze[new_y][new_x] != 0:
                self.x = new_x
                self.y = new_y

        def draw(self):
            pygame.draw.circle(self.screen, self.color, (self.x * CELL_SIZE + self.radius, self.y * CELL_SIZE + self.radius), self.radius)


    class Goal:
        def __init__(self, screen, maze, x, y):
            self.maze = maze
            self.screen = screen 
            self.x = x
            self.y = y
            self.radius = CELL_SIZE // 3
            self.color = RED

        def draw(self):
            pygame.draw.circle(self.screen, self.color, (self.x * CELL_SIZE + self.radius, self.y * CELL_SIZE + self.radius), self.radius)



def eval_genomes(genomes, config):

    global gen
   
    gen += 1


    nets = []
    players = []
    ge = []
    game = Maze()

    for genome_id, genome in genomes:
        genome.fitness = 0  # start with fitness level of 0
        net = neat.nn.FeedForwardNetwork.create(genome, config)
        nets.append(net)
        x = 1  # Avoid spawning on the walls
        y = 1
        player = game.Player(game.screen, game.maze, x, y)
        players.append(player)  # Add the player to the maze
        ge.append(genome)
        


    score = 0

    clock = pygame.time.Clock()

    run = True
    index=0
    
    while run and len(players) != 0:
        
        clock.tick(30)
    
        for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    run = False
                    pygame.quit()
                    quit()
                    break

        index=0
        for index, player in enumerate(players):
            #print("Index: ")
            #print(index)
            
            agent = Agent()
            done = False
            current_state = agent.get_state(game.maze, players[index].x, players[index].y, game.goal.x, game.goal.y)

            state_list = current_state.tolist()
            
            # Get the network's output
            output = nets[index].activate(state_list)  # Use nets[index] for the current player

            # Determine the action based on the network's output
            action = output.index(max(output))  # Choose the action with the highest output
            # Choose the action with the highest output
        
           
            final_move = [0, 0, 0, 0]

            if action == 0:
                final_move[action] = 1
            elif action == 1:
                final_move[action] = 1
            elif action == 2:
                final_move[action] = 1
            elif action==3:
                final_move[action] = 1
            

            reward, done= game.move_player(index, final_move,players[index])
            
               # After updating all players' positions, draw them on the screen
            game.screen.fill(WHITE)
            for y in range(MAZE_HEIGHT):
                for x in range(MAZE_WIDTH):
                    if game.maze[y][x] == 0:
                        pygame.draw.rect(game.screen, BLACK, (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE))
                    elif game.maze[y][x] == 2:
                        game.goal.draw()
            
            for player in players:
                player.draw()

            score_text = game.font.render(f'Score: {game.score}', True, BLACK)
            game.screen.blit(score_text, (10, 10))
            
            pygame.display.update()
            game.clock.tick(30)

            ge[index].fitness += reward
            if done:
                # If the player is done (reached goal or exceeded time), remove it
                players.pop(index)
                nets.pop(index)
                ge.pop(index)
           
            
            # Update genome's fitness based on the obtained reward
            

        # break if score gets large enough
        '''if score > 20:
            pickle.dump(nets[0],open("best.pickle", "wb"))
            break'''

def play_game(neural_network, config_path):
    # Initialize Pygame
    pygame.init()

    # Load the NEAT configuration
    config = neat.config.Config(neat.DefaultGenome, neat.DefaultReproduction,
                                 neat.DefaultSpeciesSet, neat.DefaultStagnation,
                                 config_path)

    # Create a population with a single genome (the trained neural network)
    p = neat.Population(config)
    p.add_reporter(neat.Checkpointer(1))  # Load the best genome from the last generation
    if os.path.exists("best.pickle"):
            with open("best.pickle", "rb") as f:
                winner = pickle.load(f)

    # Get the neural network from the best genome
    neural_network = neat.nn.FeedForwardNetwork.create(winner, config)

    agent = Agent()
    # Initialize the game
    game = Maze()

    run = True
    while run:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
                pygame.quit()
                break

        current_state = agent.get_state(game.maze, game.player.x, game.player.y, game.goal.x, game.goal.y)
        state_list = current_state.tolist()

        # Get the network's output
        output = neural_network.activate(state_list)

        # Determine the action based on the network's output
        action = output.index(max(output))

        final_move = [0, 0, 0, 0]
        final_move[action] = 1

        reward, done = game.move_player(0, final_move, game.player)  # Use index 0 for the single player

        game.screen.fill(WHITE)
        for y in range(MAZE_HEIGHT):
            for x in range(MAZE_WIDTH):
                if game.maze[y][x] == 0:
                    pygame.draw.rect(game.screen, BLACK, (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE))
                elif game.maze[y][x] == 2:
                    game.goal.draw()
        game.player.draw()

        pygame.display.update()
        game.clock.tick(30)

        if done:
            print("NN solved the Maze")
            run = False

    pygame.quit()


def run(config_file):

    screen = pygame.display.set_mode((MAZE_WIDTH * CELL_SIZE, MAZE_HEIGHT * CELL_SIZE))
    pygame.display.set_caption('Maze Game')

    """
    runs the NEAT algorithm to train a neural network to play flappy bird.
    :param config_file: location of config file
    :return: None
    """
    config = neat.config.Config(neat.DefaultGenome, neat.DefaultReproduction,
                         neat.DefaultSpeciesSet, neat.DefaultStagnation,
                         config_file)

    # Create the population, which is the top-level object for a NEAT run.
    p = neat.Population(config)

    # Add a stdout reporter to show progress in the terminal.
    p.add_reporter(neat.StdOutReporter(True))
    stats = neat.StatisticsReporter()
    p.add_reporter(stats)
    #p.add_reporter(neat.Checkpointer(5))

    # Run for up to 50 generations.
    winner = p.run(eval_genomes, 10)
    visualize.draw_net(config, winner, view=True, node_names=None, filename="neural_network.png")

    # Save the winning neural network using pickle
    with open("best.pickle", "wb") as pickle_file:
        pickle.dump(winner, pickle_file)
   
    # show final stats
    print('\nBest genome:\n{!s}'.format(winner))


if __name__ == "__main__":
    local_dir = os.path.dirname(__file__)
    config_path = os.path.join(local_dir, 'config-feedforward.txt')
    #run(config_path)
    play_game(None, config_path)
