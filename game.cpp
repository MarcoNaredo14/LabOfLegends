// Game.hpp
#include <SFML/Graphics.hpp>
#include <memory>
#include<iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include "player.cpp"
#include "bullet.cpp"
#include "enemy.cpp"
#include "scoreboard.cpp"
#include "lives.cpp"
#include "golditems.cpp"
#include "bgmusic.cpp"
#include <SFML/Audio.hpp>

// Add forward declaration at the top of the file
class GameState;
class GamePlayState;
class MenuState;






template<typename T>
std::unique_ptr<GameState> make_state() {
    return std::unique_ptr<GameState>(new T());
}

class Maze {
private:
    static const int MAZE_WIDTH = 28;
    static const int MAZE_HEIGHT = 21;
    std::vector<std::vector<char>> tileMap;  // Changed to char for symbols

public:
    Maze() : tileMap(MAZE_HEIGHT, std::vector<char>(MAZE_WIDTH, '#')) {
        // Initialize with predefined level layout
                const std::vector<std::string> layout = {
            "###########################",
            "#S------------------------#",
            "###-####-#########-########",
            "#---#--#-#-------#--------#",
            "#-###--#-#-#####-#-######-#",
            "#------#-#-------#------#-#",
            "######-#-###-#-###-####-#-#",
            "#------#-----#-#------#-#-#",
            "#-#########-##-#-####-#-#-#",
            "#---------#----#----#-#-#-#",
            "#########-######-##-#-#-#-#",
            "#-------#-----------#-#-#-#",
            "#-#####-#########-###-#-#-#",
            "#-#---------------#---#-#-#",
            "#-#-#################-#-#-#",
            "#-#-----------------#-#-#-#",
            "#-###############-#-#-#-#-#",
            "#-----------------#-------#",
            "#-#########################",
            "#-------------------------E",
			"###########################"
        };

        // Convert layout to tilemap
        for (int i = 0; i < MAZE_HEIGHT; i++) {
            for (int j = 0; j < MAZE_WIDTH; j++) {
                tileMap[i][j] = layout[i][j];
            }
        }
    }

    const std::vector<std::vector<char>>& getTileMap() const {
        return tileMap;
    }

    int getWidth() const { return MAZE_WIDTH; }
    int getHeight() const { return MAZE_HEIGHT; }
};


class GameState {
public:
    virtual ~GameState() = default;
    virtual void handleEvent(const sf::Event& event, sf::RenderWindow& window) {}
    virtual void handleInput(sf::RenderWindow& window) = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual std::unique_ptr<GameState> nextState() { return nullptr; }
};


class GamePlayState : public GameState {
private:

    Scoreboard scoreboard;
    int currentScore = 0;
    bool gameOver = false;
    int finalScore = 0;
	//wall texture
	sf::Texture wallTexture;
	sf::Sprite wallSprite;
    sf::Font font;
    sf::Text scoreText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    
    // Add maze members
    Maze maze;
    static const int CELL_SIZE = 35;
    sf::RectangleShape wallShape;
    sf::RectangleShape pathShape;
    sf::RectangleShape startShape;
    sf::RectangleShape endShape;

    // add enemy bullet container
    std::vector<std::unique_ptr<Bullet>> enemyBullets;

    std::string bulletTexture = "bullet.png"; // Your bullet texture path

    // player member
    std::unique_ptr<Player> player;

    // view member for camera
    sf::View gameView;
    static const float ZOOM_LEVEL;

	// bullet member
	std::vector<std::unique_ptr<Bullet>> bullets;

	// cooldown for shooting
	float cooldown = 2.5f;
	float shootTimer = 1.0f;

    // level completed
    bool levelCompleted = false;

    // Add collision padding constant
    static const int COLLISION_PADDING = 2; // Adjust this value as needed

    // Add new method to calculate view size
    sf::Vector2f calculateViewSize() const {
        float mazePixelWidth = maze.getWidth() * CELL_SIZE;
        float mazePixelHeight = maze.getHeight() * CELL_SIZE;
        return sf::Vector2f(mazePixelWidth, mazePixelHeight);
    }

    // add enemy 
    std::vector<std::unique_ptr<Enemy>> enemies;

    void updateBackgroundScale() {
        // Get the current view size and center
        sf::Vector2f viewSize = gameView.getSize();
        sf::Vector2f viewCenter = gameView.getCenter();
        
        // Add padding to ensure no black edges (20% extra)
        float padding = 1.2f;
        
        // Calculate the required scale to cover the visible area plus padding
        float scaleX = (viewSize.x * padding) / backgroundSprite.getLocalBounds().width;
        float scaleY = (viewSize.y * padding) / backgroundSprite.getLocalBounds().height;
        
        // Use the larger scale to ensure full coverage
        float scale = std::max(scaleX, scaleY);
        
        // Apply the new scale
        backgroundSprite.setScale(scale, scale);
        
        // Center the background on the view
        float bgWidth = backgroundSprite.getLocalBounds().width * scale;
        float bgHeight = backgroundSprite.getLocalBounds().height * scale;
        
        backgroundSprite.setPosition(
            viewCenter.x - bgWidth/2,
            viewCenter.y - bgHeight/2
        );
    }

    void findPathBoundaries(int row, int col, bool isHorizontal, float& pathStart, float& pathEnd) {
        const auto& tileMap = maze.getTileMap();
        float offsetX = (1080 - maze.getWidth() * CELL_SIZE) / 2.0f;
        float offsetY = (800 - maze.getHeight() * CELL_SIZE) / 2.0f;

        if (isHorizontal) {
            // Find horizontal path boundaries
            int startCol = col;
            int endCol = col;

            // Search left
            while (startCol > 0 && tileMap[row][startCol - 1] == '-') {
                startCol--;
            }

            // Search right
            while (endCol < maze.getWidth() - 1 && tileMap[row][endCol + 1] == '-') {
                endCol++;
            }

            pathStart = offsetX + startCol * CELL_SIZE;
            pathEnd = offsetX + endCol * CELL_SIZE;
        } else {
            // Find vertical path boundaries
            int startRow = row;
            int endRow = row;

            // Search up
            while (startRow > 0 && tileMap[startRow - 1][col] == '-') {
                startRow--;
            }

            // Search down
            while (endRow < maze.getHeight() - 1 && tileMap[endRow + 1][col] == '-') {
                endRow++;
            }

            pathStart = offsetY + startRow * CELL_SIZE;
            pathEnd = offsetY + endRow * CELL_SIZE;
        }
    }

    // Add initial position storage for respawn
    sf::Vector2f playerInitialPosition;

    // Add lives member
    Lives lives;

    // Add gold items container
    std::vector<std::unique_ptr<GoldItem>> goldItems;
    int totalGold = 0;
    int collectedGold = 0;

    // Add new members for sound
    sf::SoundBuffer gunshotBuffer;
    sf::Sound gunshotSound;

    bool shouldReturnToMenu = false;  // Add this new member variable
    bool moveRightHeld = false;
    bool moveLeftHeld = false;
    bool moveUpHeld = false;
    bool moveDownHeld = false;
    bool shootHeld = false;

public:
    GamePlayState() : maze(), scoreboard(), lives(5) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        // Replace the old view initialization with this:
        sf::Vector2f viewSize = calculateViewSize();
        gameView.setSize(viewSize);
        gameView.zoom(ZOOM_LEVEL);
        // score
        scoreboard.updateScore(currentScore);

        const auto& tileMap = maze.getTileMap();
        for (int row = 0; row < maze.getHeight(); row++) {
            for (int col = 0; col < maze.getWidth(); col++) {
                if (tileMap[row][col] == 'S') {
                    float startX = (1080 - maze.getWidth() * CELL_SIZE) / 2.0f + col * CELL_SIZE;
                    float startY = (800 - maze.getHeight() * CELL_SIZE) / 2.0f + row * CELL_SIZE;
                    playerInitialPosition = sf::Vector2f(startX, startY);  // Store initial position
                    player = std::make_unique<Player>(startX, startY);
                    break;
                }
            }
        }


        
        for (int row = 0; row < maze.getHeight(); row++) {
            for (int col = 0; col < maze.getWidth(); col++) {
                if (tileMap[row][col] == '-') {
                    // Check if this is part of a horizontal or vertical path
                    bool isHorizontalPath = false;
                    if (col > 0 && col < maze.getWidth() - 1) {
                        isHorizontalPath = tileMap[row][col-1] == '-' && tileMap[row][col+1] == '-';
                    }
                    
                    if (rand() % 20 == 0) {
                        float enemyX = (1080.0f - static_cast<float>(maze.getWidth() * CELL_SIZE)) / 2.0f + 
                            static_cast<float>(col * CELL_SIZE);
                        float enemyY = (800.0f - static_cast<float>(maze.getHeight() * CELL_SIZE)) / 2.0f + 
                            static_cast<float>(row * CELL_SIZE);
                        
                        float pathStart, pathEnd;
                        findPathBoundaries(row, col, isHorizontalPath, pathStart, pathEnd);
                        
                        enemies.push_back(std::make_unique<Enemy>(
                            enemyX, enemyY, isHorizontalPath, pathStart, pathEnd));
                    }
                }
            }
        }


		// loading wall texture
		if (!wallTexture.loadFromFile("wall.png")) {
			
			std::cout << "wall.png image not loaded" << std::endl;
		}
		wallSprite.setTexture(wallTexture);
		wallShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
		// scale the wall sprite to fit the cell size
		wallSprite.setScale(CELL_SIZE / wallSprite.getLocalBounds().width, CELL_SIZE / wallSprite.getLocalBounds().height);
        

        
        wallTexture.setRepeated(true);  
        wallShape.setTextureRect(sf::IntRect(0, 0, CELL_SIZE, CELL_SIZE));  

        if (!backgroundTexture.loadFromFile("background.png")) {
            
            std::cout << "background.png image not loaded" << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);
        // Initial background update
        updateBackgroundScale();

        if (!font.loadFromFile("arial.ttf")) {
            // Handle font loading error
            std::cout << "arial.ttf font not loaded" << std::endl;
        }

        scoreText.setFont(font);
        scoreText.setString("Game State ");
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        // Setup shapes for different tiles
        wallShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        wallShape.setFillColor(sf::Color(50, 50, 50));  // Dark gray for walls

        pathShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        pathShape.setFillColor(sf::Color(200, 200, 200));  // Light gray for paths

        startShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        startShape.setFillColor(sf::Color::Green);  // Green for start

        endShape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        endShape.setFillColor(sf::Color::Red);  // Red for end

        // Initialize gold items
        for (int row = 0; row < maze.getHeight(); row++) {
            for (int col = 0; col < maze.getWidth(); col++) {
                if (tileMap[row][col] == '-') {  // Place gold on paths
                    if (rand() % 10 == 0) {  // 10% chance for gold on each path tile
                        float goldX = (1080.0f - maze.getWidth() * CELL_SIZE) / 2.0f + col * CELL_SIZE;
                        float goldY = (800.0f - maze.getHeight() * CELL_SIZE) / 2.0f + row * CELL_SIZE;
                        goldItems.push_back(std::make_unique<GoldItem>(goldX, goldY));
                        totalGold++;
                    }
                }
            }
        }

        //const auto& tileMap = maze.getTileMap();
        std::vector<std::pair<int, int>> pathTiles;

        // Collect all valid path positions
        for (int row = 0; row < maze.getHeight(); row++) {
            for (int col = 0; col < maze.getWidth(); col++) {
                if (tileMap[row][col] == '-') {
                    pathTiles.push_back({row, col});
                }
            }
        }

        // Randomly shuffle the path positions
        std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
        std::shuffle(pathTiles.begin(), pathTiles.end(), rng);

        // Place exactly 16 gold items in random positions
        const int TOTAL_GOLD_ITEMS = 16;
        for (int i = 0; i < TOTAL_GOLD_ITEMS && i < pathTiles.size(); i++) {
            float goldX = (1080.0f - maze.getWidth() * CELL_SIZE) / 2.0f + pathTiles[i].second * CELL_SIZE;
            float goldY = (800.0f - maze.getHeight() * CELL_SIZE) / 2.0f + pathTiles[i].first * CELL_SIZE;
            goldItems.push_back(std::make_unique<GoldItem>(goldX, goldY));
            totalGold++;
        }

        // Add sound loading in constructor
        if (!gunshotBuffer.loadFromFile("items/gunshot.wav")) {
            std::cout << "Error loading gunshot sound" << std::endl;
        }
        gunshotSound.setBuffer(gunshotBuffer);
        gunshotSound.setVolume(80.0); // Adjust volume (0-100)
    }

    void incrementScore(int points) {
        currentScore += points;
        scoreboard.updateScore(currentScore);
    }

    bool checkCollision(float x, float y) {
        // Convert world coordinates to maze grid coordinates
        int col = (x - (1080 - maze.getWidth() * CELL_SIZE) / 2.0f) / CELL_SIZE;
        int row = (y - (800 - maze.getHeight() * CELL_SIZE) / 2.0f) / CELL_SIZE;
        
        // Check if position is within maze bounds
        if (row >= 0 && row < maze.getHeight() && col >= 0 && col < maze.getWidth()) {
            return maze.getTileMap()[row][col] == '#' ;
        }
        return true;
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override {
        if (event.type == sf::Event::LostFocus) {
            moveRightHeld = false;
            moveLeftHeld = false;
            moveUpHeld = false;
            moveDownHeld = false;
            shootHeld = false;
            return;
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Right:
                case sf::Keyboard::D:
                    moveRightHeld = true;
                    break;
                case sf::Keyboard::Left:
                case sf::Keyboard::A:
                    moveLeftHeld = true;
                    break;
                case sf::Keyboard::Up:
                case sf::Keyboard::W:
                    moveUpHeld = true;
                    break;
                case sf::Keyboard::Down:
                case sf::Keyboard::S:
                    moveDownHeld = true;
                    break;
                case sf::Keyboard::Space:
                    shootHeld = true;
                    break;
                default:
                    break;
            }
        } else if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
                case sf::Keyboard::Right:
                case sf::Keyboard::D:
                    moveRightHeld = false;
                    break;
                case sf::Keyboard::Left:
                case sf::Keyboard::A:
                    moveLeftHeld = false;
                    break;
                case sf::Keyboard::Up:
                case sf::Keyboard::W:
                    moveUpHeld = false;
                    break;
                case sf::Keyboard::Down:
                case sf::Keyboard::S:
                    moveDownHeld = false;
                    break;
                case sf::Keyboard::Space:
                    shootHeld = false;
                    break;
                default:
                    break;
            }
        }
    }

    void handleInput(sf::RenderWindow& window) override {
        float moveSpeed = 0.7f;
        float dx = 0, dy = 0;

        if (!window.hasFocus()) {
            return;
        }

        if (moveRightHeld && !moveLeftHeld) dx = moveSpeed;
        if (moveLeftHeld && !moveRightHeld) dx = -moveSpeed;
        if (moveUpHeld && !moveDownHeld) dy = -moveSpeed;
        if (moveDownHeld && !moveUpHeld) dy = moveSpeed;

        if (player) {
            sf::Vector2f pos = player->getPosition();
            
            // Check horizontal movement with padding
            if (dx != 0) {
                if (!checkCollision(pos.x + dx + (dx > 0 ? CELL_SIZE - COLLISION_PADDING : COLLISION_PADDING), pos.y + COLLISION_PADDING) && 
                    !checkCollision(pos.x + dx + (dx > 0 ? CELL_SIZE - COLLISION_PADDING : COLLISION_PADDING), pos.y + CELL_SIZE - COLLISION_PADDING)) {
                    player->move(dx, 0, 1.0f/60.0f);
                }
            }
            
            // Check vertical movement with padding
            if (dy != 0) {
                if (!checkCollision(pos.x + COLLISION_PADDING, pos.y + dy + (dy > 0 ? CELL_SIZE - COLLISION_PADDING : COLLISION_PADDING)) && 
                    !checkCollision(pos.x + CELL_SIZE - COLLISION_PADDING, pos.y + dy + (dy > 0 ? CELL_SIZE - COLLISION_PADDING : COLLISION_PADDING))) {
                    player->move(0, dy, 1.0f/60.0f);
                }
            }
        }

        if (player && shootHeld && shootTimer <= 0) {
            // Play gunshot sound
            gunshotSound.play();

            sf::Vector2f pos = player->getPosition();
            sf::Vector2f direction(0.f, 0.f);
            std::string bulletTexture = "bullet.png"; // Your bullet texture path
            
            // Set direction based on player's current direction
            switch(player->getDirection()) {
                case Player::Right:
                    direction = sf::Vector2f(1.f, 0.f);
                    break;
                case Player::Left:
                    direction = sf::Vector2f(-1.f, 0.f);
                    break;
                case Player::Up:
                    direction = sf::Vector2f(0.f, -1.f);
                    break;
                case Player::Down:
                    direction = sf::Vector2f(0.f, 1.f);
                    break;
            }
            
            // Create new bullet at player's position
            bullets.push_back(std::make_unique<Bullet>(
                pos.x + static_cast<float>(CELL_SIZE)/2.0f,  // Explicit float conversion
                pos.y + static_cast<float>(CELL_SIZE)/2.0f,
                direction,
                bulletTexture
            ));
            
            shootTimer = cooldown;
        }
        
        // Update shoot timer
        if (shootTimer > 0) {
            shootTimer -= 1.0f/60.0f;
        }
    }

    void respawnPlayer() {
        lives.loseLife();
        if (!lives.isGameOver()) {
            player = std::make_unique<Player>(playerInitialPosition.x, playerInitialPosition.y);
            incrementScore(-10);  // Optional: Penalty for dying
        } else {
            shouldReturnToMenu = true;  // Set flag to return to menu
        }
    }

    

    void update() override {
        if (lives.isGameOver()) {
            shouldReturnToMenu = true;
            return;  // Skip other updates if game is over
        }
        // Update camera and player
        if (player) {
            sf::Vector2f playerPos = player->getPosition();
            gameView.setCenter(playerPos);
            updateBackgroundScale();
        }

        // Handle bullet updates and collisions
        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
            (*bulletIt)->update(1.0f/60.0f);
            bool bulletHit = false;

            // Check collisions with enemies
            for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
                if ((*bulletIt)->getGlobalBounds().intersects((*enemyIt)->getBounds())) {
                    // Enemy was hit
                    incrementScore(10);  // Award points
                    enemyIt = enemies.erase(enemyIt);  // Remove enemy
                    bulletHit = true;
                    break;  // Exit enemy loop after hit
                } else {
                    ++enemyIt;
                }
            }

            // Remove bullet if it hit something or hit a wall
            if (bulletHit || checkCollision((*bulletIt)->getPosition().x, (*bulletIt)->getPosition().y)) {
                bulletIt = bullets.erase(bulletIt);
            } else {
                ++bulletIt;
            }
        }

        for (auto& gold : goldItems) {
            if (!gold->isCollected()) {
                gold->update(1.0f/60.0f);
                if (player && gold->getBounds().intersects(player->getBounds())) {
                    gold->collect();
                    collectedGold++;
                    incrementScore(50);  // Award points for collecting gold
                    
                    // Check if all gold is collected
                    if (collectedGold >= totalGold) {
                        levelCompleted = true;
                        // Optional: Add bonus points for completing level
                        incrementScore(1000);
                    }
                }
            }
        }

        // Update enemy positions
        for (auto& enemy : enemies) {
            sf::Vector2f oldPos = enemy->getPosition();
            enemy->update(1.0f/60.0f, player->getPosition());

             enemy->updateShootTimer(1.0f/60.0f);
            if (enemy->canShoot()) {
                sf::Vector2f enemyPos = enemy->getPosition();
                sf::Vector2f shootDir = enemy->getShootDirection();
                
                // Create new enemy bullet
                enemyBullets.push_back(std::make_unique<Bullet>(
                    enemyPos.x + static_cast<float>(CELL_SIZE)/2.0f,
                    enemyPos.y + static_cast<float>(CELL_SIZE)/2.0f,
                    shootDir,
                    bulletTexture
                ));
                
                enemy->resetShootTimer();
            }
            
            // Check for wall collisions and revert position if needed
            sf::Vector2f newPos = enemy->getPosition();
            if (checkCollision(newPos.x + COLLISION_PADDING, newPos.y + COLLISION_PADDING) ||
                checkCollision(newPos.x + CELL_SIZE - COLLISION_PADDING, newPos.y + COLLISION_PADDING) ||
                checkCollision(newPos.x + COLLISION_PADDING, newPos.y + CELL_SIZE - COLLISION_PADDING) ||
                checkCollision(newPos.x + CELL_SIZE - COLLISION_PADDING, newPos.y + CELL_SIZE - COLLISION_PADDING)) {
                enemy->setPosition(oldPos);
            }


        }

        // Check enemy bullet collisions with player
        if (player) {
            for (auto it = enemyBullets.begin(); it != enemyBullets.end();) {
                if ((*it)->getGlobalBounds().intersects(player->getBounds())) {
                    // Player hit by enemy bullet
                    respawnPlayer();
                    it = enemyBullets.erase(it);
                } else {
                    // Update bullet position
                    (*it)->update(1.0f/60.0f);
                    
                    // Check if bullet hit a wall
                    sf::Vector2f bulletPos = (*it)->getPosition();
                    if (checkCollision(bulletPos.x, bulletPos.y)) {
                        it = enemyBullets.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }

        if (levelCompleted) {
            sf::Vector2f playerPos = player->getPosition();
            int playerRow = (playerPos.y - (800.0f - maze.getHeight() * CELL_SIZE) / 2.0f) / CELL_SIZE;
            int playerCol = (playerPos.x - (1080.0f - maze.getWidth() * CELL_SIZE) / 2.0f) / CELL_SIZE;
            
            // Check if player is at exit tile ('E')
            if (maze.getTileMap()[playerRow][playerCol] == 'E') {
                shouldReturnToMenu = true;  // Use same mechanism as game over
                return;
            }
        }

        
        
        
    }

    void render(sf::RenderWindow& window) override {
        sf::View defaultView = window.getDefaultView();
        window.setView(gameView);
        window.draw(backgroundSprite);
        
        // Draw maze
        float offsetX = (1080 - maze.getWidth() * CELL_SIZE) / 2.0f;
        float offsetY = (800 - maze.getHeight() * CELL_SIZE) / 2.0f;
        
        const auto& tileMap = maze.getTileMap();
        for (int row = 0; row < maze.getHeight(); row++) {
            for (int col = 0; col < maze.getWidth(); col++) {
                sf::Vector2f position(offsetX + col * CELL_SIZE, 
                                    offsetY + row * CELL_SIZE);
                
                switch(tileMap[row][col]) {
                    case '#':  // Wall
						wallSprite.setPosition(position);
                        
                       // wallShape.setPosition(position);
						window.draw(wallSprite);
                       // window.draw(wallShape);
                        break;
                    //case '-':  // Path
                   
                    case 'S':  // Start
                        startShape.setPosition(position);
                        window.draw(startShape);
                        break;
                    case 'E':  // End
                        endShape.setPosition(position);
                        window.draw(endShape);
                        break;
                }
            }
        }

        // Draw gold items
        for (const auto& gold : goldItems) {
            gold->draw(window);
        }

        if (player){
            player->draw(window);
        }

        // Draw bullets
        for (const auto& bullet : bullets) {
            bullet->draw(window);
        }

        // draw enemies
        for (const auto& enemy : enemies) {
            enemy->draw(window);
        }

        // draw enemy bullets
        for (const auto& bullet : enemyBullets) {
            bullet->draw(window);
        }

        if (levelCompleted) {
            sf::Text completionText;
            completionText.setFont(font);
            completionText.setString("All gold collected! Head to the exit!");
            completionText.setCharacterSize(24);
            completionText.setFillColor(sf::Color::Yellow);
            completionText.setPosition(
                window.getSize().x/2 - completionText.getGlobalBounds().width/2,
                50
            );
            window.draw(completionText);
        }

       

        
        window.setView(defaultView);
        scoreboard.draw(window);
        lives.draw(window);  // Draw lives counter
    }

    std::unique_ptr<GameState> nextState() override {
        if (shouldReturnToMenu) {
            return make_state<MenuState>();  
        }
        return nullptr;
    }
};

const float GamePlayState::ZOOM_LEVEL = 1.0f;  // Adjust this value as needed

class MenuState : public GameState {
private:
    sf::Font font;
    sf::Text titleText;
    sf::Text playText;
    sf::Text exitText;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    bool switchToPlay = false;
    int selectedIndex = 0;  // 0: Play, 1: Exit

    void updateSelectionFromPosition(float x, float y) {
        if (playText.getGlobalBounds().contains(x, y)) {
            selectedIndex = 0;
        } else if (exitText.getGlobalBounds().contains(x, y)) {
            selectedIndex = 1;
        }
    }

    void activateSelection(sf::RenderWindow& window) {
        if (selectedIndex == 0) {
            switchToPlay = true;
        } else {
            window.close();
        }
    }

public:
    MenuState() {
        if (!font.loadFromFile("menu/menu.otf")) {
            // Handle font loading error
            std::cout << "menu.otf font not loaded" << std::endl;
        }
        if (!backgroundTexture.loadFromFile("menu/backgroundmenu.png")) {
            // Handle background loading error
            std::cout << "background_menu.png image not loaded" << std::endl;
        }
        backgroundSprite.setTexture(backgroundTexture);

        backgroundSprite.setScale(1080.f / backgroundSprite.getLocalBounds().width, 800.f / backgroundSprite.getLocalBounds().height); // Use .f for float division

        titleText.setFont(font);
        titleText.setString("Game Title");
        titleText.setCharacterSize(60);
        titleText.setFillColor(sf::Color::White);
        float titleX = (1200.f - titleText.getLocalBounds().width) / 2.f; // Use .f for float division
        titleText.setPosition(titleX, 500.f); // Use .f for float positions

        playText.setFont(font);
        playText.setString("Play");
        playText.setCharacterSize(40);
        playText.setFillColor(sf::Color::White);
        float playX = (1200.f - playText.getLocalBounds().width) / 2.f; // Use .f for float division
        playText.setPosition(playX, 600.f); // Use .f for float positions

        exitText.setFont(font);
        exitText.setString("Exit");
        exitText.setCharacterSize(40);
        exitText.setFillColor(sf::Color::White);
        float exitX = (1200.f - exitText.getLocalBounds().width) / 2.f; // Use .f for float division
        exitText.setPosition(exitX, 700.f); // Use .f for float positions
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window) override {
        if (event.type == sf::Event::MouseMoved) {
            updateSelectionFromPosition(
                static_cast<float>(event.mouseMove.x),
                static_cast<float>(event.mouseMove.y)
            );
        } else if (event.type == sf::Event::MouseButtonPressed &&
                   event.mouseButton.button == sf::Mouse::Left) {
            updateSelectionFromPosition(
                static_cast<float>(event.mouseButton.x),
                static_cast<float>(event.mouseButton.y)
            );
            activateSelection(window);
        } else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Up:
                case sf::Keyboard::W:
                    selectedIndex = 0;
                    break;
                case sf::Keyboard::Down:
                case sf::Keyboard::S:
                    selectedIndex = 1;
                    break;
                case sf::Keyboard::Return:
                case sf::Keyboard::Space:
                    activateSelection(window);
                    break;
                default:
                    break;
            }
        }
    }

    void handleInput(sf::RenderWindow& window) override {
        playText.setFillColor(selectedIndex == 0 ? sf::Color::Red : sf::Color::White);
        exitText.setFillColor(selectedIndex == 1 ? sf::Color::Red : sf::Color::White);
    }

    void update() override {
        // Update menu state
    }

    void render(sf::RenderWindow& window) override {
        window.draw(backgroundSprite);
        window.draw(titleText);
        window.draw(playText);
        window.draw(exitText);
    }

   std::unique_ptr<GameState> nextState() override {
    if (switchToPlay) {
        return make_state<GamePlayState>();  
    }
    return nullptr;
}
};

class Game {
private:
    sf::RenderWindow window;
    std::unique_ptr<GameState> currentState;
    BackgroundMusic bgMusic;

public:
    Game() : window(sf::VideoMode(1080, 800), "Game") {
        window.setFramerateLimit(60);
        currentState = std::make_unique<MenuState>();
		bgMusic.setVolume(30.0f);
        bgMusic.play();
    }

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                currentState->handleEvent(event, window);

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::M) {  // M to mute/unmute
                        if (bgMusic.getIsPlaying()) {
                            bgMusic.pause();
                        } else {
                            bgMusic.play();
                        }
                    }
                }


            }

            currentState->handleInput(window);
            currentState->update();

            // Check for state transition
            if (auto nextState = currentState->nextState()) {
                currentState = std::move(nextState);
            }

            window.clear(sf::Color::Black);
            currentState->render(window);
            window.display();
        }
    }
};
// For coordinate calculations
