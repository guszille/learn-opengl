#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const char* vertexShaderFilepath, const char* fragmentShaderFilepath) : m_ID()
{
	int success;
	char infoLog[512];

	unsigned int vertexShaderID = createShader(vertexShaderFilepath, GL_VERTEX_SHADER);
	unsigned int fragmentShaderID = createShader(fragmentShaderFilepath, GL_FRAGMENT_SHADER);

	m_ID = glCreateProgram();

	glAttachShader(m_ID, vertexShaderID);
	glAttachShader(m_ID, fragmentShaderID);

	glLinkProgram(m_ID);
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(m_ID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Linkage failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
}

ShaderProgram::ShaderProgram(const char* vertexShaderFilepath, const char* geometryShaderFilepath, const char* fragmentShaderFilepath) : m_ID()
{
	int success;
	char infoLog[512];

	unsigned int vertexShaderID = createShader(vertexShaderFilepath, GL_VERTEX_SHADER);
	unsigned int geometryShaderID = createShader(geometryShaderFilepath, GL_GEOMETRY_SHADER);
	unsigned int fragmentShaderID = createShader(fragmentShaderFilepath, GL_FRAGMENT_SHADER);

	m_ID = glCreateProgram();

	glAttachShader(m_ID, vertexShaderID);
	glAttachShader(m_ID, geometryShaderID);
	glAttachShader(m_ID, fragmentShaderID);

	glLinkProgram(m_ID);
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(m_ID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Linkage failed!\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShaderID);
	glDeleteShader(geometryShaderID);
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

void ShaderProgram::setUniform1i(const char* uniformName, const int& data)
{
	int uniformLocation = glGetUniformLocation(m_ID, uniformName);

	if (uniformLocation > -1)
	{
		glUniform1i(uniformLocation, data);
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"" << std::endl;
	}
}

void ShaderProgram::setUniform1f(const char* uniformName, const float& data)
{
	int uniformLocation = glGetUniformLocation(m_ID, uniformName);

	if (uniformLocation > -1)
	{
		glUniform1f(uniformLocation, data);
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"" << std::endl;
	}
}

void ShaderProgram::setUniform3f(const char* uniformName, const glm::vec3& data)
{
	int uniformLocation = glGetUniformLocation(m_ID, uniformName);

	if (uniformLocation > -1)
	{
		glUniform3f(uniformLocation, data.x, data.y, data.z);
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"" << std::endl;
	}
}

void ShaderProgram::setUniform4f(const char* uniformName, const glm::vec4& data)
{
	int uniformLocation = glGetUniformLocation(m_ID, uniformName);

	if (uniformLocation > -1)
	{
		glUniform4f(uniformLocation, data.x, data.y, data.z, data.w);
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"" << std::endl;
	}
}

void ShaderProgram::setUniformMatrix4fv(const char* uniformName, const glm::mat4& data)
{
	int uniformLocation = glGetUniformLocation(m_ID, uniformName);

	if (uniformLocation > -1)
	{
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(data));
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform \"" << uniformName << "\"" << std::endl;
	}
}

void ShaderProgram::setUniformBlock(const char* uniformBlockName, const int bindingPoint)
{
	int uniformBlockLocation = glGetUniformBlockIndex(m_ID, uniformBlockName);

	if (uniformBlockLocation > -1)
	{
		glUniformBlockBinding(m_ID, uniformBlockLocation, bindingPoint);
	}
	else
	{
		std::cout << "[ERROR] SHADER PROGRAM: Failed to get location of uniform block \"" << uniformBlockName << "\"" << std::endl;
	}
}

const std::string ShaderProgram::readShaderSource(const char* filepath)
{
	std::ifstream fileStream(filepath);
	std::stringstream stringBuffer;

	stringBuffer << fileStream.rdbuf();

	return stringBuffer.str();
}

const unsigned int ShaderProgram::createShader(const char* shaderFilepath, int shaderType)
{
	int success;
	char infoLog[512];

	const std::string shaderSource = readShaderSource(shaderFilepath);
	const char* shaderCode = shaderSource.c_str();

	unsigned int shaderID = glCreateShader(shaderType);

	glShaderSource(shaderID, 1, &shaderCode, NULL);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);

		std::cout << "[ERROR] SHADER PROGRAM: Compilation failed!\n" << infoLog << std::endl;

		glDeleteShader(shaderID);
		return 0;
	}

	return shaderID;
}