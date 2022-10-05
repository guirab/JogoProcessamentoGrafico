#include <conio.h>
#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STB_IMAGE
#include "stb_image.h"
#include "Shader.h"


// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Protótipos das funções
float nivelCalc(int nivel, int casa);
int setupGeometry();
int setupSprite(int nAnimations, int nFrames, float& dx, float& dy);

GLuint createTexture(string filePath);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

//using namespace glm;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Dinooooo!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */

	// consts
	int dificuldade = 1;
	float dinoPos = 400.0f;
	float mirror = 120.0f;

	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;

	// Compilando e buildando o programa de shader
	Shader shader("../shaders/hello.vs", "../shaders/hello.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	float dx, dy;
	GLuint VAO_Fundo = setupSprite(1, 1, dx, dy);
	GLuint VAO_Meteoro = setupSprite(1, 1, dx, dy);
	GLuint VAO_Explosao = setupSprite(1, 1, dx, dy);
	GLuint VAO_Life = setupSprite(1, 1, dx, dy);
	GLuint VAO_GameOver = setupSprite(1, 1, dx, dy);
	GLuint VAO_Numeros = setupSprite(1, 10, dx, dy);
	GLuint VAO_Personagem = setupSprite(1, 5, dx, dy);

	int iAnimation = 0;
	int iFrame = 0;
	int nFrames = 5;
	float velocidade = 8.0f;
	int nivel = 1;
	bool gameOver = false;
	int lifes = 3;
	bool met = true;

	float xMeteoro = 400.0f, yMeteoro = 700.0f;

	GLuint texID_Fundo = createTexture("../textures/desert-100.jpg");
	GLuint texID_Personagem = createTexture("../textures/dinoanda.png");
	GLuint texID_Meteoro = createTexture("../textures/flaming_meteor.png");
	GLuint texID_GameOver = createTexture("../textures/game_over.png");
	GLuint texID_Explosao = createTexture("../textures/explosion.png");
	GLuint texID_Numeros = createTexture("../textures/numeros.png");
	GLuint texID_Life = createTexture("../textures/life.png");

	//Matriz de projeção
	glm::mat4 projection = glm::mat4(1); //matriz identidade
	projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f);

	glUseProgram(shader.ID);

	shader.setMat4("projection", glm::value_ptr(projection));

	glUniform1i(glGetUniformLocation(shader.ID, "tex_buffer"), 0);

	glActiveTexture(GL_TEXTURE0);

	//Habilitando o teste de profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	//Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as funções de callback correspondentes
		glfwPollEvents();

		// Limpa o buffer de cor
		glClearColor(0.8f, 0.8f, 0.8f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		//----------------FUNDO---------------------
		//Matriz de transformação do objeto
		glm::mat4 model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(400.0f, 300.0f, 0.0f));
		//model = glm::rotate(model, /*glm::radians(90.0f)*/(float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1080.0f, 540.0f, 1.0f));
		shader.setMat4("model", glm::value_ptr(model));

		//Mandar para o shader os deslocamentos da texturas (spritesheet)
		shader.setVec2("offsets", 1, 1);
		// Chamada de desenho - drawcall
		// Poligono Preenchido - GL_TRIANGLES

		glBindVertexArray(VAO_Fundo);
		glBindTexture(GL_TEXTURE_2D, texID_Fundo);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		//---------------DINO-------------------------
		model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(dinoPos, 157.0f, 0.0f));
		model = glm::scale(model, glm::vec3(mirror, 72.0f, 1.0f));
		shader.setMat4("model", glm::value_ptr(model));

		shader.setVec2("offsets", iFrame * dx, iAnimation * dy);

		iFrame = (iFrame + 1) % nFrames;

		glBindVertexArray(VAO_Personagem);
		glBindTexture(GL_TEXTURE_2D, texID_Personagem);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//---------------LIFES---------------------------		
		if (lifes >= 1) 
		{
			model = glm::mat4(1); //matriz identidade
			model = glm::translate(model, glm::vec3(20.0f, 550.0f, 0.0f));
			model = glm::scale(model, glm::vec3(30.0f, 30.0f, 1.0f));
			shader.setMat4("model", glm::value_ptr(model));

			shader.setVec2("offsets", 1, 1);

			glBindVertexArray(VAO_Life);
			glBindTexture(GL_TEXTURE_2D, texID_Life);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		if (lifes >= 2) {
			model = glm::mat4(1); //matriz identidade
			model = glm::translate(model, glm::vec3(60.0f, 550.0f, 0.0f));
			model = glm::scale(model, glm::vec3(30.0f, 30.0f, 1.0f));
			shader.setMat4("model", glm::value_ptr(model));

			shader.setVec2("offsets", 1, 1);

			glBindVertexArray(VAO_Life);
			glBindTexture(GL_TEXTURE_2D, texID_Life);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		if (lifes == 3) {
			model = glm::mat4(1); //matriz identidade
			model = glm::translate(model, glm::vec3(100.0f, 550.0f, 0.0f));
			model = glm::scale(model, glm::vec3(30.0f, 30.0f, 1.0f));
			shader.setMat4("model", glm::value_ptr(model));

			shader.setVec2("offsets", 1, 1);

			glBindVertexArray(VAO_Life);
			glBindTexture(GL_TEXTURE_2D, texID_Life);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		//--------------LEVELS-----------------------
		model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(730.0f, 550.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 50.0f, 0.0f));
		shader.setMat4("model", glm::value_ptr(model));
		
		if (dificuldade > 99)
			shader.setVec2("offsets", nivelCalc(dificuldade, 0), 1);

		glBindVertexArray(VAO_Numeros);
		glBindTexture(GL_TEXTURE_2D, texID_Numeros);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(755.0f, 550.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 50.0f, 0.0f));
		shader.setMat4("model", glm::value_ptr(model));

		if (dificuldade > 9)
			shader.setVec2("offsets", nivelCalc(dificuldade, 0), 1);

		glBindVertexArray(VAO_Numeros);
		glBindTexture(GL_TEXTURE_2D, texID_Numeros);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(780.0f, 550.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 50.0f, 0.0f));
		shader.setMat4("model", glm::value_ptr(model));

		if (dificuldade < 10)
			shader.setVec2("offsets", nivelCalc(dificuldade, 0), 1);
		else if (dificuldade >= 10 && dificuldade < 100)
			shader.setVec2("offsets", nivelCalc(dificuldade, 1), 1);
		else
			shader.setVec2("offsets", nivelCalc(dificuldade, 2), 1);

		glBindVertexArray(VAO_Numeros);
		glBindTexture(GL_TEXTURE_2D, texID_Numeros);
		glDrawArrays(GL_TRIANGLES, 0, 6);	
		

		//--------------METEORO--------------------------
		model = glm::mat4(1); //matriz identidade
		model = glm::translate(model, glm::vec3(xMeteoro, yMeteoro, 0.0f));
		model = glm::scale(model, glm::vec3(64.0f, 64.0f, 1.0f));
		shader.setMat4("model", glm::value_ptr(model));

		shader.setVec2("offsets", 1, 1);

		glBindVertexArray(VAO_Meteoro);
		glBindTexture(GL_TEXTURE_2D, texID_Meteoro);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//--------------MOVIMENTAÇAO--------------------------
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !gameOver) {
			mirror = 120.0f;
			iFrame++;
			if(dinoPos < 750){
				dinoPos += 5.0f;
			}
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !gameOver) {
			mirror = -120.0f;
			iFrame++;
			if (dinoPos > 50) {
				dinoPos -= 5.0f;
			}
		}
		else
		{
			iFrame = 0;
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && gameOver) {	
			xMeteoro = 400.0f; 
			yMeteoro = 700.0f;
			velocidade = 8.0f;
			nivel = 1;
			gameOver = false;
			dificuldade = 1;
			dinoPos = 400.0f;
			mirror = 120.0f;
			lifes = 3;
			met = true;
		}

		yMeteoro -= velocidade;

		//--------------COLISAO--------------------------
		if ((xMeteoro <= dinoPos + 55.0f && xMeteoro >= dinoPos - 55.0f) 
			&& yMeteoro <= 160.0f && yMeteoro >= 128.0f) 
		{				
			if (met && lifes > 0)
			{
				--lifes;
				met = false;
			}
			
			glm::mat4 model = glm::mat4(1);		
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(xMeteoro, yMeteoro, 0.0f));
			model = glm::scale(model, glm::vec3(200.0f, 200.0f, 1.0f));
			shader.setMat4("model", glm::value_ptr(model));
			shader.setVec2("offsets", 1, 1);

			glBindVertexArray(VAO_Explosao);
			glBindTexture(GL_TEXTURE_2D, texID_Explosao);

			glDrawArrays(GL_TRIANGLES, 0, 6);	

			if (lifes == 0)
			{
				glm::mat4 model = glm::mat4(1);
				model = glm::translate(model, glm::vec3(400.0f, 300.0f, 0.0f));
				model = glm::scale(model, glm::vec3(410.0f, 159.0f, 1.0f));
				shader.setMat4("model", glm::value_ptr(model));
				shader.setVec2("offsets", 1, 1);

				glBindVertexArray(VAO_GameOver);
				glBindTexture(GL_TEXTURE_2D, texID_GameOver);

				glDrawArrays(GL_TRIANGLES, 0, 6);

				gameOver = true;
				velocidade = 0;
			}
		}
		//--------------NIVEL--------------------------
		if (yMeteoro < 0.0f)
		{	
			met = true;
			dificuldade++;
			yMeteoro = 700;
			xMeteoro = 64 + rand() % 736;

			if (dificuldade % 10 == 0.0f) {
				nivel++;
				velocidade += 5.0f;
			}			
		}		

		glBindVertexArray(0);
		// Troca os buffers da tela
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO_Personagem);
	glDeleteVertexArrays(1, &VAO_Fundo);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

//--------------TEXTO NIVEL--------------------------
float nivelCalc(int nivel, int casa) {
	string nivelToString = to_string(nivel);
	char nivelSplit = nivelToString.at(casa);	

	switch (nivelSplit)
	{
		case '1':
			return 0.1f;
		case '2':
			return 0.2f;
		case '3':
			return 0.3f;
		case '4':
			return 0.4f;
		case '5':
			return 0.5f;
		case '6':
			return 0.6f;
		case '7':
			return 0.7f;
		case '8':
			return 0.8f;
		case '9':
			return 0.9f;
		default:
			return 0.0f;
	}	
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do triângulo e as armazenamos de forma
	// sequencial, já visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do vértice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO único ou em VBOs separados
	GLfloat vertices[] = {
		//x   y     z    s    t
		-0.5, -0.5, 0.0, 0.0, 0.0,
		 0.5, -0.5, 0.0, 1.0, 0.0,
		 0.0,  0.5, 0.0, 0.5, 1.0
	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo coord de textura
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

GLuint createTexture(string filePath)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregando a imagem do filePath e armazenando em data
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);



	return texID;
}

int setupSprite(int nAnimations, int nFrames, float& dx, float& dy)
{
	dx = 1.0 / nFrames;
	dy = 1.0 / nAnimations;

	GLfloat vertices[] = {
		//x   y     z    s    t
		//Primeiro triângulo
		-0.5, -0.5, 0.0, 0.0, 0.0, //v0
		 0.5, -0.5, 0.0, dx, 0.0, //v1
		 0.5,  0.5, 0.0, dx, dy,  //v2
		 //outro triangulo vai aqui
		-0.5, -0.5, 0.0, 0.0, 0.0, //v0
		 0.5,  0.5, 0.0, dx, dy,  //v2
		-0.5,  0.5, 0.0, 0.0, dy, //v3
	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 

	//Atributo posição
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//Atributo coord de textura
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}