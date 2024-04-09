import pygame
import math

# Initialize Pygame
pygame.init()

# Screen dimensions
WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Neural Network Architecture Visualization")

# Neural network architecture (number of neurons in each layer)
architecture = [13, 256, 4]

# Circle properties
circle_radius = 30
circle_spacing = 100

# Colors
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

# Clear the screen
screen.fill(WHITE)

# Draw circles for each layer
for layer_idx, num_neurons in enumerate(architecture):
    x = (WIDTH // 2) - ((num_neurons - 1) * circle_spacing) // 2
    y = (layer_idx + 1) * circle_spacing
    for _ in range(num_neurons):
        pygame.draw.circle(screen, BLACK, (x, y), circle_radius, 2)
        x += circle_spacing

# Draw lines connecting circles
for layer_idx in range(len(architecture) - 1):
    for i in range(architecture[layer_idx]):
        x1 = (WIDTH // 2) - ((architecture[layer_idx] - 1) * circle_spacing) // 2 + i * circle_spacing
        y1 = (layer_idx + 1) * circle_spacing + circle_radius
        for j in range(architecture[layer_idx + 1]):
            x2 = (WIDTH // 2) - ((architecture[layer_idx + 1] - 1) * circle_spacing) // 2 + j * circle_spacing
            y2 = (layer_idx + 2) * circle_spacing - circle_radius
            pygame.draw.line(screen, BLACK, (x1, y1), (x2, y2))

# Main loop
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    pygame.display.update()

# Quit Pygame
pygame.quit()
