import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.Random;

public class SnakeGame extends JFrame {
    private static final int WIDTH = 800;
    private static final int HEIGHT = 600;
    private static final int UNIT_SIZE = 20;
    private static final int GAME_UNITS = (WIDTH * HEIGHT) / (UNIT_SIZE * UNIT_SIZE);
    private static final int DELAY = 100;

    private ArrayList<Point> snake;
    private Point food;
    private int score;
    private char direction;
    private boolean running;

    private Timer timer;
    private JPanel gamePanel;
    private JLabel scoreLabel;

    public SnakeGame() {
        snake = new ArrayList<>();
        snake.add(new Point(WIDTH / 2, HEIGHT / 2));
        food = getRandomFoodPosition();
        direction = 'R';
        score = 0;
        running = true;

        this.setTitle("Snake Game");
        this.setSize(WIDTH, HEIGHT);
        this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        this.setResizable(false);
        this.setLayout(new BorderLayout());

        gamePanel = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);

                g.setColor(new Color(19, 161, 186));
                g.fillRect(0, 0, getWidth(), getHeight());

                draw(g);
            }
        };
        gamePanel.setPreferredSize(new Dimension(WIDTH - (WIDTH % UNIT_SIZE), HEIGHT - (HEIGHT % UNIT_SIZE)));
        this.add(gamePanel, BorderLayout.CENTER);

        scoreLabel = new JLabel("Score: 0");
        this.add(scoreLabel, BorderLayout.SOUTH);

        timer = new Timer(DELAY, new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (running) {
                    move();
                    checkCollision();
                    checkFoodCollision();
                }
                gamePanel.repaint();
            }
        });

        gamePanel.addKeyListener(new KeyListener() {
            @Override
            public void keyPressed(KeyEvent e) {
                handleInput(e.getKeyCode());
            }

            @Override
            public void keyReleased(KeyEvent e) {
            }

            @Override
            public void keyTyped(KeyEvent e) {
            }
        });

        gamePanel.setFocusable(true);
        gamePanel.requestFocusInWindow();

        timer.start();
        this.pack();
        this.setLocationRelativeTo(null);
        this.setVisible(true);
    }

    private Point getRandomFoodPosition() {
        Random random = new Random();
        int x, y;
        do {
            x = random.nextInt(WIDTH / UNIT_SIZE) * UNIT_SIZE;
            y = random.nextInt(HEIGHT / UNIT_SIZE) * UNIT_SIZE;
        } while (snake.contains(new Point(x, y)));
        return new Point(x, y);
    }

    private void handleInput(int keyCode) {
        switch (keyCode) {
            case KeyEvent.VK_UP:
                if (direction != 'D')
                    direction = 'U';
                break;
            case KeyEvent.VK_DOWN:
                if (direction != 'U')
                    direction = 'D';
                break;
            case KeyEvent.VK_LEFT:
                if (direction != 'R')
                    direction = 'L';
                break;
            case KeyEvent.VK_RIGHT:
                if (direction != 'L')
                    direction = 'R';
                break;
            case KeyEvent.VK_X:
                running = false;
                gameOver();
                break;
        }
    }

    private void move() {
        Point head = snake.get(0);
        int x = head.x;
        int y = head.y;

        switch (direction) {
            case 'U':
                y -= UNIT_SIZE;
                break;
            case 'D':
                y += UNIT_SIZE;
                break;
            case 'L':
                x -= UNIT_SIZE;
                break;
            case 'R':
                x += UNIT_SIZE;
                break;
        }

        snake.add(0, new Point(x, y));
        snake.remove(snake.size() - 1);
    }

    private void checkCollision() {
        Point head = snake.get(0);
        int x = head.x;
        int y = head.y;

        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
            running = false;
            gameOver();
        }

        for (int i = 1; i < snake.size(); i++) {
            if (head.equals(snake.get(i))) {
                running = false;
                gameOver();
                break;
            }
        }
    }

    private void checkFoodCollision() {
        Point head = snake.get(0);
        if (head.equals(food)) {
            score++;
            snake.add(new Point(-1, -1));
            food = getRandomFoodPosition();
            scoreLabel.setText("Score: " + score);
        }
    }

    private void draw(Graphics g) {
        for (Point p : snake) {
            g.setColor(Color.black);
            g.fillRect(p.x, p.y, UNIT_SIZE, UNIT_SIZE);
        }

        g.setColor(Color.orange);
        g.fillRect(food.x, food.y, UNIT_SIZE, UNIT_SIZE);

        Toolkit.getDefaultToolkit().sync();
    }

    private void gameOver() {
        timer.stop();
        JOptionPane.showMessageDialog(this, "Game Over! Your Score: " + score, "Game Over", JOptionPane.INFORMATION_MESSAGE);
        this.dispose();
    }

    private static class Point {
        int x;
        int y;

        Point(int x, int y) {
            this.x = x;
            this.y = y;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj) return true;
            if (obj == null || getClass() != obj.getClass()) return false;
            Point point = (Point) obj;
            return x == point.x && y == point.y;
        }

        @Override
        public int hashCode() {
            return 31 * x + y;
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                new SnakeGame();
            }
        });
    }
}
