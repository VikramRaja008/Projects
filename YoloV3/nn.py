import pygame
import sys

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

# Maze layout (0 represents walls, 1 represents empty cells, and 2 represents the goal)
maze = [
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 1, 1, 0, 1, 1, 1, 1, 0],
    [0, 1, 0, 1, 0, 1, 0, 0, 1, 0],
    [0, 1, 0, 1, 1, 1, 1, 0, 1, 0],
    [0, 1, 0, 0, 0, 0, 1, 0, 1, 0],
    [0, 1, 1, 1, 1, 1, 1, 0, 1, 0],
    [0, 0, 0, 0, 0, 0, 1, 0, 1, 0],
    [0, 1, 1, 1, 1, 1, 1, 1, 1, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 2, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
]

# Create screen
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption('Maze Game')


# Player class
class Player:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.radius = CELL_SIZE // 2
        self.color = GREEN

    def move(self, dx, dy):
        new_x = self.x + dx
        new_y = self.y + dy

        if 0 <= new_x < MAZE_WIDTH and 0 <= new_y < MAZE_HEIGHT and maze[new_y][new_x] != 0:
            self.x = new_x
            self.y = new_y

    def draw(self):
        pygame.draw.circle(screen, self.color, (self.x * CELL_SIZE + self.radius, self.y * CELL_SIZE + self.radius), self.radius)

# Goal class
class Goal:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.radius = CELL_SIZE // 3
        self.color = RED

    def draw(self):
        pygame.draw.circle(screen, self.color, (self.x * CELL_SIZE + self.radius, self.y * CELL_SIZE + self.radius), self.radius)

# ... (previous code)

# Create Agent instance

player = Player(1, 1)
goal = Goal(8, 8)

score = 0
font = pygame.font.Font(None, 36)

clock = pygame.time.Clock()

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_UP:
                player.move(0, -1)
            elif event.key == pygame.K_DOWN:
                player.move(0, 1)
            elif event.key == pygame.K_LEFT:
                player.move(-1, 0)
            elif event.key == pygame.K_RIGHT:
                player.move(1, 0)

    if player.x == goal.x and player.y == goal.y:
        score += 1
        goal.x = (goal.x + 1) % MAZE_WIDTH

    screen.fill(WHITE)

    for y in range(MAZE_HEIGHT):
        for x in range(MAZE_WIDTH):
            if maze[y][x] == 0:
                pygame.draw.rect(screen, BLACK, (x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE))
            elif maze[y][x] == 2:
                goal.draw()

    player.draw()

    # Inside the train loop
    

    score_text = font.render(f'Score: {score}', True, BLACK)
    screen.blit(score_text, (10, 10))

    pygame.display.update()
    clock.tick(60)

