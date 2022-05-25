#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vertexShaderFilename, const char* fragmentShaderFilename) : m_ID()
{
	int success;
	char infoLog[512];

	unsigned int vertexShaderID = createShader(vertexShaderFilename, GL_VERTEX_SHADER);
	unsigned int fragmentShaderID = createShader(fragmentShaderFilename, GL_FRAGMENT_SHADER);

	m_ID = glCreateProgram();

	glAttachShader(m_ID, vertexShaderID);
	glAttachShader(m_ID, fragmentShaderID);

	glLinkProgram(m_ID);
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(m_ID, 512, NULL, infoLog);

		std::cout << "ERROR::SHADER::PROGRAM::LINKAGE_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(m_ID);
}

void ShaderProgram::bind()
{
	glUseProgram(m_ID);
}

void ShaderProgram::unbind()
{
	glUseProgram(0);
}

const std::string ShaderProgram::readShaderSource(const char* filename)
{
	std::ifstream fileStream(filename);
	std::stringstream stringBuffer;

	stringBuffer << fileStream.rdbuf();

	return stringBuffer.str();
}

const unsigned int ShaderProgram::createShader(const char* shaderFilename, int shaderType)
{
	int success;
	char infoLog[512];

	const std::string shaderSource = readShaderSource(shaderFilename);
	const char* raw = shaderSource.c_str();

	unsigned int shaderID = glCreateShader(shaderType);

	glShaderSource(shaderID, 1, &raw, NULL);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);

		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;

		glDeleteShader(shaderID);
		return 0;
	}

	return shaderID;
}