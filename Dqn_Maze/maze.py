import pygame
import sys
import random

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

def generate_random_maze(width, height):
    maze = []
    for _ in range(height):
        row = [random.choice([0, 1]) for _ in range(width)]
        maze.append(row)
    return maze

class Maze:
    def __init__(self):
        # Maze layout (0 represents walls, 1 represents empty cells, and 2 represents the goal)
      
        self.maze = [
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 1, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 1, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 1, 0, 0, 0, 0, 1, 0, 1, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 2, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
]

        # Create screen
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.player = self.Player(self.screen, self.maze, 1, 1)
        self.goal = self.Goal(self.screen, self.maze, 8, 8)
        pygame.display.set_caption('Maze Game')

        self.move_time_limit = 4000  # 4 seconds in milliseconds
        self.last_move_time = pygame.time.get_ticks()

        # Initialize player and goal
   
        self.score = 0
        self.font = pygame.font.Font(None, 36)

        self.clock = pygame.time.Clock()
            
    def reset(self):
        # Generate a new random maze layout
      
      #  self.maze = generate_random_maze(WIDTH,HEIGHT)  # Implement a function to generate a random maze layout
        
        # Reset player and goal positions
        self.player.x = 1  # Reset player's starting position
        self.player.y = 1
        self.goal.x = 8  # Reset goal's position
        self.goal.y = 8
        self.score = 0  # Reset score

    
    def move_player(self, action):
        current_time = pygame.time.get_ticks()
        
        
        for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()


        if action[0] == 1:  # Move Up
            self.player.move(0, -1)
           # print("moving up")
        elif action[1] == 1:  # Move Down
            self.player.move(0, 1)
           # print("moving down")
        elif action[2] == 1:  # Move Left
            self.player.move(-1, 0)
           # print("moving left")
        elif action[3] == 1:  # Move Right
            self.player.move(1, 0)
           # print("moving right")
       
        self.screen.fill(WHITE)

        for y in range(MAZE_HEIGHT):
            for x in range(MAZE_WIDTH):
                if self.maze[y][x] == 0:
                     pygame.draw.rect(self.screen, BLACK, (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE))
                elif self.maze[y][x] == 2:
                        self.goal.draw()

        self.player.draw()

        score_text = self.font.render(f'Score: {self.score}', True, BLACK)
        self.screen.blit(score_text, (10, 10))

        pygame.display.update()
        self.clock.tick(60)

        new_state = (self.player.x, self.player.y)
        done=False
        if new_state == (self.goal.x, self.goal.y):
            reward = 10  # Reward for reaching the goal
            done=True
        elif self.maze[new_state[1]][new_state[0]] == 0:
            reward = -1  # Penalty for hitting a wall
        else:
            reward = -0.1  # Small negative reward for normal steps


        return reward, done

    class Player:
        def __init__(self, screen, maze, x, y):
            self.maze = maze
            self.screen = screen 
            self.x = x
            self.y = y
            self.radius = CELL_SIZE // 2
            self.color = GREEN

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

