////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <windows.h>

////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
	setlocale(LC_ALL, "Russian");
	std::srand(static_cast<unsigned int>(std::time(NULL))); 

	// Define some constants
	const float pi = 3.14159f;
	const int gameWidth = 400;
	const int gameHeight = 600;
	const float paddleWidth = 25;
	float ballRadius = 15.f;
	float multiplier = 0.016f;
	
	// Create the window of the application
	sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight, 32), "SFML Pong",
		sf::Style::Titlebar | sf::Style::Close);
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);

	// Load the sounds used in the game
	sf::SoundBuffer ballSoundBuffer;
	if (!ballSoundBuffer.loadFromFile("resources/sfx_wing.wav"))
		return EXIT_FAILURE;
	sf::Sound ballSound(ballSoundBuffer);

	sf::SoundBuffer ballSoundBuffer2;
	if (!ballSoundBuffer2.loadFromFile("resources/ball.wav"))
		return EXIT_FAILURE;
	sf::Sound collisionSound(ballSoundBuffer2);

	// Load the png texture of burb
	sf::Texture texture;
	if (!texture.loadFromFile("resources/emoji.png"))
	{
		std::cout << "Cannot load the image!\n";
		return 0;
	}

	// Set distances for gap between paddles and paddles
	const float gap = 170;
	const float minPaddleHeight = 50;

	// Randomize the paddle size
	float randPaddleHeight1 = minPaddleHeight + std::rand() % int(gameHeight - gap - 2 * minPaddleHeight);
	float randPaddleHeight2 = minPaddleHeight + std::rand() % int(gameHeight - gap - 2 * minPaddleHeight);
	std::cout << randPaddleHeight1 << "; " << randPaddleHeight2 << " random height\n";
	// Set sizes of paddles into Vector2f
	sf::Vector2f paddleSize1(paddleWidth, randPaddleHeight1);
	sf::Vector2f paddleSize2(paddleWidth, gameHeight - gap - randPaddleHeight1);
	sf::Vector2f paddleSize3(paddleWidth, randPaddleHeight2);
	sf::Vector2f paddleSize4(paddleWidth, gameHeight - gap - randPaddleHeight2);
	std::cout << paddleSize1.x << "; " << paddleSize1.y << " paddle size 1\n";
	std::cout << paddleSize2.x << "; " << paddleSize2.y << " paddle size 2\n";
	std::cout << paddleSize3.x << "; " << paddleSize3.y << " paddle size 3\n";
	std::cout << paddleSize4.x << "; " << paddleSize4.y << " paddle size 4\n";
	// Creating paddles
	sf::RectangleShape topPaddle;
	topPaddle.setSize(paddleSize1);
	topPaddle.setOutlineThickness(3);
	topPaddle.setOutlineColor(sf::Color::Black);
	topPaddle.setFillColor(sf::Color(100, 100, 200));

	sf::RectangleShape bottomPaddle;
	bottomPaddle.setSize(paddleSize2);
	bottomPaddle.setOutlineThickness(3);
	bottomPaddle.setOutlineColor(sf::Color::Black);
	bottomPaddle.setFillColor(sf::Color(100, 100, 200));

	sf::RectangleShape topPaddle2;
	topPaddle2.setSize(paddleSize3);
	topPaddle2.setOutlineThickness(3);
	topPaddle2.setOutlineColor(sf::Color::Black);
	topPaddle2.setFillColor(sf::Color(100, 100, 200));

	sf::RectangleShape bottomPaddle2;
	bottomPaddle2.setSize(paddleSize4);
	bottomPaddle2.setOutlineThickness(3);
	bottomPaddle2.setOutlineColor(sf::Color::Black);
	bottomPaddle2.setFillColor(sf::Color(100, 100, 200));

	sf::RectangleShape paddles[4];
	paddles[0] = topPaddle;
	paddles[1] = bottomPaddle;
	paddles[2] = topPaddle2;
	paddles[3] = bottomPaddle2;
	std::cout << paddles[0].getSize().x << "; " << paddles[0].getSize().y << " p1\n";
	std::cout << paddles[1].getSize().x << "; " << paddles[1].getSize().y << " p2\n";
	std::cout << paddles[2].getSize().x << "; " << paddles[2].getSize().y << " p3\n";
	std::cout << paddles[3].getSize().x << "; " << paddles[3].getSize().y << " p4\n";
	std::cout << _countof(paddles) << " sizeof(paddles)\n";

	// Create the ball
	sf::CircleShape ball;
	ball.setRadius(ballRadius);
	ball.setFillColor(sf::Color::White);
	ball.setTexture(&texture);
	ball.setOrigin(ballRadius, ballRadius);
	
	// Load the text font
	sf::Font font;
	if (!font.loadFromFile("resources/seasrn.ttf"))
		return EXIT_FAILURE;

	// Initialize the pause message
	sf::Text pauseMessage;
	pauseMessage.setFont(font);
	pauseMessage.setCharacterSize(40);
	pauseMessage.setPosition(20.f, 200.f);
	pauseMessage.setFillColor(sf::Color::White);
	pauseMessage.setString("Welcome Floppy Burb!\nPress space to start the game");
	pauseMessage.setCharacterSize(20);

	// Define the paddles properties
	sf::Clock AITimer;
	const sf::Time AITime = sf::seconds(0.1f);
	float paddleSpeed = 2.f;
	float rightPaddleSpeed = 0.f;
	float ballSpeed = 0.f;
	float ballAngle = 0.f; // to be changed later
	bool ballCollided = false;

	sf::Clock clock;
	bool isPlaying = false;
	while (window.isOpen())
	{
		// Handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
			{
				window.close();
				break;
			}
			
			// P key pressed: pause
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::P)) 
			{
				isPlaying = !isPlaying;
				pauseMessage.setString("Pause");
				break;
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space))
			{
				if (!isPlaying)
				{
					// (re)start the game
					isPlaying = true;
					clock.restart();
					ballSpeed = 0.0f;

					// Reset the position of the paddles and ball
					paddles[0].setPosition(gameWidth, 0);
					paddles[1].setPosition(gameWidth, gameHeight - paddles[1].getSize().y);
					paddles[2].setPosition(gameWidth + gameWidth/2, 0);
					paddles[3].setPosition(gameWidth + gameWidth/2, gameHeight - paddles[3].getSize().y);
					ball.setPosition(gameWidth / 3, gameHeight / 2);

					// Reset the ball angle
					//do
					//{
					//	// Make sure the ball initial angle is not too much vertical
					//	random = std::rand();
					//	ballAngle = (std::rand() % 360) *2 * pi / 360;
					//} while (std::abs(std::cos(ballAngle)) < 0.7f);
				}
			}
		}

		if (isPlaying)
		{
			
			// Fly the ball
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
			{
				ballSpeed = -8.f;
				ballSound.play();
				ball.setRotation(-60);
			}

			// Ball goes down all the time
			ball.move(0.f, ballSpeed);
			ballSpeed += 0.5f;
			float rotation = abs(ballSpeed);
			ball.rotate(rotation);
			if (ballSpeed > 8) {
				ball.setRotation(75);
			}

			// Check collision with top and bottom
			if (ball.getPosition().y < 0.f)
			{
				ballSpeed = 0;
				ball.setPosition(ball.getPosition().x, ballRadius + 0.1f);
			}
			if (ball.getPosition().y + ballRadius > gameHeight)
			{
				isPlaying = false;
				pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
			}

			// Move paddles to the left
			for (int i = 0; i < 4; i++)
			{
				paddles[i].move(-paddleSpeed, 0);
			}

			// Teleport paddles to the beginning when they get out of window
			for (int i = 0; i < 4; i += 2)
			{
				if (paddles[i].getPosition().x < -paddleWidth)
				{

					// Randomize the paddle size
					randPaddleHeight1 = minPaddleHeight + std::rand() % int(gameHeight - gap - 2 * minPaddleHeight);

					// Set sizes of paddles into Vector2f
					sf::Vector2f paddleSize1(paddleWidth, randPaddleHeight1);
					sf::Vector2f paddleSize2(paddleWidth, gameHeight - gap - randPaddleHeight1);

					paddles[i].setSize(paddleSize1);
					paddles[i + 1].setSize(paddleSize2);

					// Move paddles to the beginning
					paddles[i].setPosition(gameWidth, 0);
					paddles[i + 1].setPosition(gameWidth, gameHeight - paddles[i + 1].getSize().y);
				}
			}

			// Check collision between the ball and the paddles
			for (int i = 0; i < 4; i++)
			{
				ballCollided = ball.getGlobalBounds().intersects(paddles[i].getGlobalBounds());
				//ballCollided = ball.getPosition().x + ballRadius >= paddles[i].getPosition().x &&
				//	ball.getPosition().x - ballRadius <= paddles[i].getPosition().x + paddleWidth &&
				//	(ball.getPosition().y + ballRadius <= paddles[i].getSize().y || ball.getPosition().y + ballRadius >= paddles[i + 1].getPosition().y);
				if (ballCollided)
				{
					collisionSound.play();
					isPlaying = false;
					pauseMessage.setString("You lost!\nPress space to restart or\nescape to exit");
				}
			}
		}
			

		// Clear the window
		window.clear(sf::Color(50, 200, 50));

		if (isPlaying)
		{
			// Draw the paddles and the ball
			window.draw(paddles[0]);
			window.draw(paddles[1]);
			window.draw(paddles[2]);
			window.draw(paddles[3]);
			window.draw(ball);
		}
		else
		{
			// Draw the pause message
			window.draw(pauseMessage);
		}

		// Display things on screen
		window.display();
	}

	return EXIT_SUCCESS;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
