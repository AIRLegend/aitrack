#include "filters.h"

MAFilter::MAFilter(int steps, int array_size)
{
	this->n_steps = steps;
	this->array_size = array_size;
	this->idx = 0;

	this->circular_buffer = new float[steps * array_size];
}

MAFilter::~MAFilter()
{
	delete[] this->circular_buffer;
}

void MAFilter::filter(float* in_array, float* out_array)
{
	int offset = this->idx * this->array_size;
	for (int i = 0; i < this->array_size; i++)
	{
		// Insert current position
		this->circular_buffer[offset + i] = in_array[i];
		out_array[i] = 0;

		// get mean of all steps for this position
		for (int j = 0; j < this->n_steps; j++)
		{
			out_array[i] += this->circular_buffer[j * this->array_size + i];
		}

		out_array[i] /= this->n_steps;
	}

	this->idx = (this->idx + 1) % this->n_steps;
	
}


EAFilter::EAFilter(int array_size)
{
	this->array_size = array_size;
	this->last_value = new float[array_size];
}

EAFilter::~EAFilter()
{
	delete[] this->last_value;
}

void EAFilter::filter(float* in_array, float* out_array)
{
	for (int i = 0; i < array_size; i++)
	{
		out_array[i] = 0.6 * in_array[i] + 0.4 * this->last_value[i];
		this->last_value[i] = out_array[i];
	}

}