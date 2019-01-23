#pragma once

#include <JuceHeader.h>
#include <Polyline2D/Polyline2D.h>

class MainComponent : public OpenGLAppComponent {
public:
	MainComponent();

	~MainComponent();

	void initialise() override;

	void shutdown() override;

	void render() override;

	void paint(Graphics &g) override;

	void resized() override;

private:
	const std::vector<crushedpixel::Vec2> points;

	GLuint vaoHandle, vboHandle, programHandle;

	GLint posInAttribLocation;
	GLint colorUniformLocation;

	GLsizei numPoints, numVertices;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
