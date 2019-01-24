#include "MainComponent.h"

using GL = juce::OpenGLExtensionFunctions;

MainComponent::MainComponent() :
		points{
				{-0.25f, -0.5f},
				{-0.25f, 0.5f},
				{0.25f,  0.25f},
				{0.0f,   0.0f},
				{0.25f,  -0.25f},
				{-0.4f,  -0.25f}
		} {

	// we use modern OpenGL, because we're classy
	openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
	openGLContext.setMultisamplingEnabled(true);

	setSize(800, 800);
}

MainComponent::~MainComponent() {
	// This shuts down the GL system and stops the rendering calls.
	shutdownOpenGL();
}

static void writeVec2(juce::MemoryOutputStream &out, const crushedpixel::Vec2 &vec) {
	// write vertices as two consecutive floats
	out.writeFloat(vec.x);
	out.writeFloat(vec.y);
}

void MainComponent::initialise() {
	// generate VAO
	GL::glGenVertexArrays(1, &vaoHandle);
	GL::glBindVertexArray(vaoHandle);

	// generate VBOs
	GL::glGenBuffers(1, &vboHandle);
	GL::glBindBuffer(GL_ARRAY_BUFFER, vboHandle);

	// generate polyline vertices
	auto vertices = crushedpixel::Polyline2D::create(
			points, 0.1f,
			crushedpixel::Polyline2D::JointStyle::ROUND,
			crushedpixel::Polyline2D::EndCapStyle::SQUARE);

	numPoints = (GLsizei) points.size();
	numVertices = (GLsizei) vertices.size();

	// write original points, followed by vertices, to buffer
	juce::MemoryOutputStream mos;
	for (auto &vertex : points) {
		writeVec2(mos, vertex);
	}
	for (auto &vertex : vertices) {
		writeVec2(mos, vertex);
	}

	GL::glBufferData(GL_ARRAY_BUFFER, mos.getDataSize(), mos.getData(), GL_STATIC_DRAW);

	// generate Shader Program
	auto vert = GL::glCreateShader(GL_VERTEX_SHADER);
	auto frag = GL::glCreateShader(GL_FRAGMENT_SHADER);

	GL::glShaderSource(vert, 1, &BinaryData::Passthrough_vert, &BinaryData::Passthrough_vertSize);
	GL::glShaderSource(frag, 1, &BinaryData::PlainColor_frag, &BinaryData::PlainColor_fragSize);

	GL::glCompileShader(vert);
	GL::glCompileShader(frag);
	// no error checking, I don't make mistakes (lol)

	programHandle = GL::glCreateProgram();
	GL::glAttachShader(programHandle, vert);
	GL::glAttachShader(programHandle, frag);
	GL::glLinkProgram(programHandle);

	GL::glDeleteShader(vert);
	GL::glDeleteShader(frag);

	// get shader attribute locations
	posInAttribLocation = GL::glGetAttribLocation(programHandle, "posIn");
	colorUniformLocation = GL::glGetUniformLocation(programHandle, "color");

	// tell OpenGL to read the input data as pairs of floats
	GL::glVertexAttribPointer(posInAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	GL::glEnableVertexAttribArray(posInAttribLocation);
}

void MainComponent::shutdown() {
	GL::glDeleteVertexArrays(1, &vaoHandle);
	GL::glDeleteBuffers(1, &vboHandle);
	GL::glDeleteProgram(programHandle);
}

void MainComponent::render() {
	OpenGLHelpers::clear(Colours::black);

	GL::glBindVertexArray(vaoHandle);
	GL::glUseProgram(programHandle);

	{   // draw wireframe
		auto color = juce::Colours::lightgreen;
		GL::glUniform4f(colorUniformLocation,
		                color.getFloatRed(),
		                color.getFloatGreen(),
		                color.getFloatBlue(),
		                color.getFloatAlpha());

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, numPoints, numVertices);
	}

	{   // fill solid
		auto color = juce::Colours::green.withAlpha(0.5f);
		GL::glUniform4f(colorUniformLocation,
		                color.getFloatRed(),
		                color.getFloatGreen(),
		                color.getFloatBlue(),
		                color.getFloatAlpha());

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, numPoints, numVertices);
	}

	{   // draw original points
		glPointSize(10);

		auto color = juce::Colours::red;
		GL::glUniform4f(colorUniformLocation,
		                color.getFloatRed(),
		                color.getFloatGreen(),
		                color.getFloatBlue(),
		                color.getFloatAlpha());

		glDrawArrays(GL_POINTS, 0, numPoints);
	}
}

void MainComponent::paint(Graphics &g) {
	// You can add your component specific drawing code here!
	// This will draw over the top of the openGL background.
}

void MainComponent::resized() {
	// This is called when the MainComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
}
