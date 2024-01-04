/*
  ==============================================================================

	WaveshaperImage.cpp
	Created: 3 Jan 2024 2:48:39pm
	Author:  ClemensBeissel

  ==============================================================================
*/

#include "WaveshaperImage.h"

/*
  ==============================================================================

	WaveshaperImage.h
	Created: 3 Jan 2024 2:48:39pm
	Author:  ClemensBeissel

  ==============================================================================
*/

template <typename T>
std::vector<T> linspace(T a, T b, size_t N) {
	T h = (b - a) / static_cast<T>(N - 1);
	std::vector<T> xs(N);
	typename std::vector<T>::iterator x;
	T val;
	for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h) {
		*x = val;
	}
	return xs;
}


void WaveshaperImage::createImage(size_t i, Rectangle<int> bounds, float drive)
{
	images[i] = Image(Image::RGB, bounds.getWidth(), bounds.getHeight(), true);

	Graphics g(images[i]);
	g.setColour(Colours::white);
	g.fillRect(bounds);

	size_t samplesN = bounds.getWidth();
	std::vector<float> x_values;

	x_values = linspace(-1.0f, 1.0f, samplesN);

	std::vector<float> y_values;
	y_values.reserve(samplesN);



	float y;
	Path path;
	for (size_t x = 0; x < samplesN; x++)
	{
		y = std::tanh(x_values[x] * -drive) * 0.5 + 0.5;
		y *= bounds.getHeight();
		y_values.push_back(y);

		DBG(y);
		if (x == 0)
			path.startNewSubPath(0, y);
		else
			path.lineTo(x, y);
	}



	g.setColour(juce::Colours::black);
	g.strokePath(path, PathStrokeType(5.0f));


	float zeroCrossing = (float)bounds.getHeight() / 2;

	path.lineTo(bounds.getWidth(), zeroCrossing);
	path.lineTo(0, zeroCrossing);
	path.closeSubPath();

	g.setColour(juce::Colours::red);
	g.fillPath(path);

	g.setColour(juce::Colours::black);
	g.drawLine(bounds.getWidth(), zeroCrossing, 0, zeroCrossing, 1.0f);
}


using namespace juce;
WaveshaperImage::WaveshaperImage()
{

}

void WaveshaperImage::paint(Graphics& g)
{
	g.drawImage(images[imageIndex], getLocalBounds().toFloat());
}

void WaveshaperImage::setImageIndex(size_t imgIndex)
{
	imageIndex = imgIndex;
	repaint();
}

size_t WaveshaperImage::getImageCount()
{
	return images.size();
}

void WaveshaperImage::setMaxDrive(float max_drive)
{
	MAX_DRIVE = max_drive;
}

void WaveshaperImage::resized()
{
	auto bounds = getLocalBounds();
	float drive = 1.0f;
	float driveInterval = MAX_DRIVE / images.size();

	for (size_t i = 0; i < images.size(); i++)
	{
		createImage(i, bounds, drive);
		drive += driveInterval;
	}

}



