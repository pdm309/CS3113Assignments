#include "ShaderProgram.h"
// ...

#include "Matrix.h"
// …
int main(int argc, char *argv[])
{
	Matrix projectionMatrix;
		Matrix modelMatrix;
		Matrix viewMatrix;

		projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
		ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);

		void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

		glUseProgram(program.programID);

		float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);

		glEnableVertexAttribArray(program.positionAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(program.positionAttribute);
}
	